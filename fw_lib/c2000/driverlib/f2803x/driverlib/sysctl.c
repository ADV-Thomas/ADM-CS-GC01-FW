/**************************************************************************************************
 * 
 * \file sysctl.c
 * 
 * \brief System control driverlib implementation
 * 
 * \note Most of this driverlib has been adapted from f28004x driverlib.
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "sysctl.h"
#include "cputimer.h"

#define SYSCTL_DELAY    __asm(" .if __TI_EABI__\n"                                              \
                              " .asg    SysCtl_delay    , _SysCtl_delay\n"                      \
                              " .endif\n"                                                       \
                              " .def _SysCtl_delay\n"                                           \
                              " .sect \".TI.ramfunc\"\n"                                        \
                              " .global  _SysCtl_delay\n"                                       \
                              "_SysCtl_delay:\n"                                                \
                              " SUB    ACC,#1\n"                                                \
                              " BF     _SysCtl_delay, GEQ\n"                                    \
                              " LRETR\n")

/**************************************************************************************************
 * 
 * SysCtl_delay()
 * 
 *************************************************************************************************/
SYSCTL_DELAY;

/**************************************************************************************************
 * 
 * \brief Loops forever if something is wrong with the oscillator module
 * 
 * \param None
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
SysCtl_checkClock(void)
{
    /** TODO: Think about replacing ESTOP0 with more appropriate error-handling routine */
    while (SysCtl_isMCDClockFailureDetected()) {
        ESTOP0;
    }
}

/**************************************************************************************************
 * 
 * SysCtl_getClock()
 * 
 *************************************************************************************************/
uint32_t
SysCtl_getClock(uint32_t clockInHz)
{
    uint32_t clockOut;

    if (SysCtl_isMCDClockFailureDetected()) {
        return SYSCTL_DEFAULT_OSC_FREQ;
    }

    /* Get oscillator source flags */
    bool srcSel = HW_BIT_GET(CLKCFG_BASE, SYSCTL, CLKCTL, OSCCLKSRCSEL);
    bool srcSel2 = HW_BIT_GET(CLKCFG_BASE, SYSCTL, CLKCTL, OSCCLKSRC2SEL);

    /* Get OSCCLK frequency */
    clockOut = (!srcSel || srcSel2) ? SYSCTL_DEFAULT_OSC_FREQ : clockInHz;

    /* Get PLL enable flag and integer multiplier */
    bool pllOff = HW_BIT_GET(CLKCFG_BASE, SYSCTL, PLLSTS, PLLOFF);
    uint16_t pllMult = HW_FLD_GET(CLKCFG_BASE, SYSCTL, PLLCR, DIV);

    /* Aplly PLL multiplier */
    if (!pllOff && (pllMult > 0U) && (pllMult <= 12U)) {
        clockOut *= pllMult;
    }

    /* Get system clock divider */
    uint16_t divSel = HW_FLD_GET(CLKCFG_BASE, SYSCTL, PLLSTS, DIVSEL);
    divSel = (divSel == 1U) ? 4U : (4U - divSel);

    /* Apply system clock divider */
    clockOut /= divSel;

    return clockOut;
}

/**************************************************************************************************
 * 
 * SysCtl_setClock()
 * 
 *************************************************************************************************/
bool
SysCtl_setClock(uint32_t config)
{
    SysCtl_checkClock();

    /* Decode configuration parameters */
    uint32_t oscSource = config & SYSCTL_OSCSRC_M;
    uint16_t divSel = (config & SYSCTL_SYSDIV_M) >> SYSCTL_SYSDIV_S;
    uint16_t pllMult = (config & SYSCTL_IMULT_M) >> SYSCTL_IMULT_S;

    SysCtl_selectOscSource(oscSource);

    /**
     * Before configuring PLL multiplier, missing clock detection logic must be disabled and the
     * system clock divider must be set to divide-by-4.
     */
    SysCtl_disableMCD();
    SysCtl_setPLLSysClk(4U);

    SysCtl_setPLLMultiplier(0U);

    if (config & SYSCTL_PLL_ENABLE) {
        SysCtl_enablePLL();
        SysCtl_setPLLMultiplier(pllMult);
    } else { /* SYSCTL_PLL_DISABLE */
        SysCtl_disablePLL();
    }

    SysCtl_enableMCD();

    if (divSel == 1U) {
        /* First program system clock divider to divide-by-2 */
        SysCtl_setPLLSysClk(2U);

        /**
         * Wait at least 50 us to let the power settle:
         *  - 600U delay loop cycles equals 3000U clock cycles
         *  - 3000U clock cycles equals 50 us at 60 MHz and 300 us at 10 MHz
         */
        SysCtl_delay(600U);
    }

    SysCtl_setPLLSysClk(divSel);

    SysCtl_checkClock();

    return true;
}

/**************************************************************************************************
 * 
 * SysCtl_selectOscSource()
 * 
 *************************************************************************************************/
void
SysCtl_selectOscSource(uint32_t oscSource)
{
    uint16_t clkCtl;

    uint16_t mask = SYSCTL_CLKCTL_XCLKINOFF | SYSCTL_CLKCTL_XTALOSCOFF;

    SysCtl_turnOnOsc(SYSCTL_OSCSRC_OSC1);
    SysCtl_turnOnOsc(oscSource);

    if (oscSource == SYSCTL_OSCSRC_XTAL) {
        /**
         * Delay at least 1 ms to ensure crystal oscillator is up and running:
         *  - 12000U delay loop cycles equals 60000U clock cycles
         *  - 60000U clock cycles equals 1 ms at 60 MHz, 6 ms at 10 MHz, and 24 ms at 2.5 MHz
         * Maximum clock frequency for this device is 60 MHz, which means that this delay will
         * never be less than 1 ms.
         */
        SysCtl_delay(12000U);
    }

    /* Internal oscillator 1 is always used as watchdog clock source */
    EALLOW;
    HW_BIT_CLR(CLKCFG_BASE, SYSCTL, CLKCTL, WDCLKSRCSEL);
    EDIS;

    switch (oscSource) {
    case SYSCTL_OSCSRC_OSC2:
        /* Get clock configuration register */
        clkCtl = HW_REG_GET(CLKCFG_BASE, SYSCTL, CLKCTL);

        /**
         * External clocks must be disabled first (Sec. 1.3.2.3.2). See "CPU Clock Switching to
         * INTOSC2 May Result in Missing Clock Condition After Reset" in SPRZ295Q silicon errata.
         */
        EALLOW;
        HW_REG_SET(CLKCFG_BASE, SYSCTL, CLKCTL, (clkCtl | mask));
        HW_REG_SET(CLKCFG_BASE, SYSCTL, CLKCTL, (clkCtl & ~mask));
        HW_REG_SET(CLKCFG_BASE, SYSCTL, CLKCTL, (clkCtl | mask));
        HW_REG_SET(CLKCFG_BASE, SYSCTL, CLKCTL, (clkCtl & ~mask));
        HW_REG_SET(CLKCFG_BASE, SYSCTL, CLKCTL, (clkCtl | mask));
        EDIS;

        SysCtl_setInternalOscMode(oscSource);

        break;

    case SYSCTL_OSCSRC_XTAL:
        SysCtl_turnOffOsc(SYSCTL_OSCSRC_XCLK19);
        SysCtl_turnOffOsc(SYSCTL_OSCSRC_OSC2);

        SysCtl_setExternalOscMode(oscSource);

        break;

    case SYSCTL_OSCSRC_XCLK19:
        /* Intentional fall through */
    case SYSCTL_OSCSRC_XCLK38:
        SysCtl_turnOffOsc(SYSCTL_OSCSRC_XTAL);
        SysCtl_turnOffOsc(SYSCTL_OSCSRC_OSC2);

        SysCtl_setExternalOscMode(oscSource);

        break;

    case SYSCTL_OSCSRC_OSC1:
        SysCtl_setInternalOscMode(oscSource);

        SysCtl_turnOffOsc(SYSCTL_OSCSRC_OSC2);
        SysCtl_turnOffOsc(SYSCTL_OSCSRC_XTAL);
        SysCtl_turnOffOsc(SYSCTL_OSCSRC_XCLK19);

        break;

    default:
        /* Invalid oscSource value */
        ASSERT(false);
        break;
    }

    SysCtl_checkClock();
}

/**************************************************************************************************
 * 
 * SysCtl_getLowSpeedClock()
 * 
 *************************************************************************************************/
uint32_t
SysCtl_getLowSpeedClock(uint32_t clockInHz)
{
    uint32_t clockOut;

    /* Get the main system clock */
    clockOut = SysCtl_getClock(clockInHz);

    /* Get the low-speed peripheral clock prescaler */
    uint16_t prescaler = HW_FLD_GET(CLKCFG_BASE, SYSCTL, LOSPCP, LSPCLK);

    /* Apply prescaler to the main clock */
    clockOut /= ((prescaler == 0U) ? 1U : (2U * prescaler));

    return clockOut;
}

/**************************************************************************************************
 * 
 * SysCtl_getDeviceParametric()
 * 
 *************************************************************************************************/
uint16_t
SysCtl_getDeviceParametric(SysCtl_DeviceParametric parametric)
{
    uint16_t value = 0U;

    switch (parametric) {
    case SYSCTL_DEVICE_PARTTYPE:
        /* Get device part type */
        value = HW_FLD_GET(PARTID_BASE, SYSCTL, PARTID, PARTTYPE);
        break;

    case SYSCTL_DEVICE_PARTNO:
        /* Get device part number */
        value = HW_FLD_GET(PARTID_BASE, SYSCTL, PARTID, PARTNO);
        break;

    case SYSCTL_DEVICE_CLASSID:
        /* Get device class identifier */
        value = HW_FLD_GET(DEVCFG_BASE, SYSCTL, CLASSID, CLASSID);
        break;
        
    default:
        /* Invalid parametric value */
        ASSERT(false);
        break;
    }

    return value;
}
