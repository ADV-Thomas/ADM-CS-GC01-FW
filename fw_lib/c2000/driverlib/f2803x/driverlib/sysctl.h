/**************************************************************************************************
 * 
 * \file sysctl.h
 * 
 * \brief System control driverlib interface
 * 
 * \note Most of this driverlib has been adapted from f28004x driverlib.
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef SYSCTL_H
#define SYSCTL_H

#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"

#include "cpu.h"
#include "debug.h"

#include <stdint.h>
#include <stdbool.h>

/**************************************************************************************************
 * 
 * Defines for system control functions. Not intended for use by application code.
 * 
 *************************************************************************************************/

#define SYSCTL_WD_CHKBITS       0x0028U     /* Shifted pattern for WDCR[WDCHK] field            */

#define SYSCTL_WD_ENRSTKEY      0x0055U     /* Watchdog timer reset enable key                  */
#define SYSCTL_WD_RSTKEY        0x00AAU     /* Watchdog timer reset key                         */

/* Values to decode items from SysCtl_PeripheralPCLOCKCR */
#define SYSCTL_PERIPH_REG_S     0U          /* Shift for register offset                        */
#define SYSCTL_PERIPH_REG_M     0x001FU     /* Mask for register offset                         */
#define SYSCTL_PERIPH_BIT_S     8U          /* Shift for bit position                           */
#define SYSCTL_PERIPH_BIT_M     0x0F00U     /* Mask for bit position                            */

#define SYSCTL_DEFAULT_OSC_FREQ 10000000U   /* Default internal oscillator frequency (10 MHz)   */

/* Factory-programmed Device_cal() calibration routine in OTP memory */
#define Device_cal      ((void (*)(void))((uintptr_t) 0x3D7C80))

/**************************************************************************************************
 * 
 * The following are values that can be passed to the SysCtl_setClock() API as the config input:
 *  31      PLL enable or disable
 *  30:19   Reserved
 *  18:16   Oscillator source
 *  15:9    Reserved
 *  8:7     System clock divider (1, 2, or 4)
 *  6:4     Reserved
 *  3:0     Integer multiplier (1..12)
 * 
 *************************************************************************************************/

/* System clock divider (SYSDIV) */
#define SYSCTL_SYSDIV_M         0x00000180U     /* Mask for SYSDIV value in config              */
#define SYSCTL_SYSDIV_S         7U              /* Shift for SYSDIV value in config             */

/* Macro to format system clock divider value. x must be 1, 2, or 4. */
#define SYSCTL_SYSDIV(x)        (((x) << SYSCTL_SYSDIV_S) & SYSCTL_SYSDIV_M)

/* Integer multiplier (IMULT) */
#define SYSCTL_IMULT_M          0x0000000FU     /* Mask for IMULT value in config               */
#define SYSCTL_IMULT_S          0U              /* Shift for IMULT value in config              */

/* Macro to format integer multiplier value. x is a number from 1 to 12. */
#define SYSCTL_IMULT(x)         (((x) << SYSCTL_IMULT_S) & SYSCTL_IMULT_M)

/* Oscillator source */
#define SYSCTL_OSCSRC_M         0x00070000U     /* Mask for OSCSRC value in config              */
#define SYSCTL_OSCSRC_S         16U             /* Shift for OSCSRC value in config             */

#define SYSCTL_OSCSRC_OSC2      0x00000000U     /* Internal oscillator INTOSC2                  */
#define SYSCTL_OSCSRC_XTAL      0x00010000U     /* External oscillator in crystal mode          */
#define SYSCTL_OSCSRC_XCLK19    0x00030000U     /* External single-ended clock on GPIO19        */
#define SYSCTL_OSCSRC_XCLK38    0x00070000U     /* External single-ended clock on GPIO38        */
#define SYSCTL_OSCSRC_OSC1      0x00020000U     /* Internal oscillator INTOSC1                  */

#define SYSCTL_PLL_ENABLE       0x80000000U     /* Enable PLL                                   */
#define SYSCTL_PLL_DISABLE      0x00000000U     /* Disable PLL                                  */

/**************************************************************************************************
 * 
 * These enumerations are passed to the SysCtl_enablePeripheral() and SysCtl_disablePeripheral()
 * functions as the \e peripheral parameter.
 * 
 *************************************************************************************************/
typedef enum {
    SYSCTL_PERIPH_CLK_HRPWM     = 0x000CU,      /* HRPWM clock                                  */
    SYSCTL_PERIPH_CLK_LINA      = 0x010CU,      /* LIN-A clock                                  */
    SYSCTL_PERIPH_CLK_TBCLKSYNC = 0x020CU,      /* ePWM Module Time Base Clock Sync             */
    SYSCTL_PERIPH_CLK_ADC       = 0x030CU,      /* ADC clock                                    */
    SYSCTL_PERIPH_CLK_I2CA      = 0x040CU,      /* I2C clock                                    */
    SYSCTL_PERIPH_CLK_SPIA      = 0x080CU,      /* SPI-A clock                                  */
    SYSCTL_PERIPH_CLK_SPIB      = 0x090CU,      /* SPI-B clock                                  */
    SYSCTL_PERIPH_CLK_SCIA      = 0x0A0CU,      /* SCI-A clock                                  */
    SYSCTL_PERIPH_CLK_ECANA     = 0x0E0CU,      /* ECAN-A clock                                 */
    SYSCTL_PERIPH_CLK_EPWM1     = 0x000DU,      /* ePWM1 clock                                  */
    SYSCTL_PERIPH_CLK_EPWM2     = 0x010DU,      /* ePWM2 clock                                  */
    SYSCTL_PERIPH_CLK_EPWM3     = 0x020DU,      /* ePWM3 clock                                  */
    SYSCTL_PERIPH_CLK_EPWM4     = 0x030DU,      /* ePWM4 clock                                  */
    SYSCTL_PERIPH_CLK_EPWM5     = 0x040DU,      /* ePWM5 clock                                  */
    SYSCTL_PERIPH_CLK_EPWM6     = 0x050DU,      /* ePWM6 clock                                  */
    SYSCTL_PERIPH_CLK_EPWM7     = 0x060DU,      /* ePWM7 clock                                  */
    SYSCTL_PERIPH_CLK_ECAP1     = 0x080DU,      /* eCAP1 clock                                  */
    SYSCTL_PERIPH_CLK_EQEP1     = 0x0E0DU,      /* eQEP1 clock                                  */
    SYSCTL_PERIPH_CLK_HRCAP1    = 0x0809U,      /* HRCAP1 clock                                 */
    SYSCTL_PERIPH_CLK_HRCAP2    = 0x0909U,      /* HRCAP2 clock                                 */
    SYSCTL_PERIPH_CLK_COMP1     = 0x0010U,      /* Comparator 1 clock                           */
    SYSCTL_PERIPH_CLK_COMP2     = 0x0110U,      /* Comparator 2 clock                           */
    SYSCTL_PERIPH_CLK_COMP3     = 0x0210U,      /* Comparator 3 clock                           */
    SYSCTL_PERIPH_CLK_CPUTIMER0 = 0x0810U,      /* CPU Timer 0 clock                            */
    SYSCTL_PERIPH_CLK_CPUTIMER1 = 0x0910U,      /* CPU Timer 1 clock                            */
    SYSCTL_PERIPH_CLK_CPUTIMER2 = 0x0A10U,      /* CPU Timer 2 clock                            */
    SYSCTL_PERIPH_CLK_CLA1      = 0x0E10U       /* CLA 1 clock                                  */
} SysCtl_PeripheralPCLOCKCR;

/**************************************************************************************************
 * 
 * These enumerations are passed to the SysCtl_setWatchdogPrescaler() function as the \e prescaler
 * parameter.
 * 
 *************************************************************************************************/
typedef enum {
    SYSCTL_WD_PRESCALE_1  = 1,                  /* WDCLK = OSCCLK / 512 / 1                     */
    SYSCTL_WD_PRESCALE_2  = 2,                  /* WDCLK = OSCCLK / 512 / 2                     */
    SYSCTL_WD_PRESCALE_4  = 3,                  /* WDCLK = OSCCLK / 512 / 4                     */
    SYSCTL_WD_PRESCALE_8  = 4,                  /* WDCLK = OSCCLK / 512 / 8                     */
    SYSCTL_WD_PRESCALE_16 = 5,                  /* WDCLK = OSCCLK / 512 / 16                    */
    SYSCTL_WD_PRESCALE_32 = 6,                  /* WDCLK = OSCCLK / 512 / 32                    */
    SYSCTL_WD_PRESCALE_64 = 7                   /* WDCLK = OSCCLK / 512 / 64                    */
} SysCtl_WDPrescaler;

/**************************************************************************************************
 * 
 * These enumerations are passed to the SysCtl_setLowSpeedClock() function as the \e prescaler
 * parameter.
 * 
 *************************************************************************************************/
typedef enum {
    SYSCTL_LSPCLK_PRESCALE_1  = 0,              /* LSPCLK = SYSCLK / 1                          */
    SYSCTL_LSPCLK_PRESCALE_2  = 1,              /* LSPCLK = SYSCLK / 2                          */
    SYSCTL_LSPCLK_PRESCALE_4  = 2,              /* LSPCLK = SYSCLK / 4 (default)                */
    SYSCTL_LSPCLK_PRESCALE_6  = 3,              /* LSPCLK = SYSCLK / 6                          */
    SYSCTL_LSPCLK_PRESCALE_8  = 4,              /* LSPCLK = SYSCLK / 8                          */
    SYSCTL_LSPCLK_PRESCALE_10 = 5,              /* LSPCLK = SYSCLK / 10                         */
    SYSCTL_LSPCLK_PRESCALE_12 = 6,              /* LSPCLK = SYSCLK / 12                         */
    SYSCTL_LSPCLK_PRESCALE_14 = 7               /* LSPCLK = SYSCLK / 14                         */
} SysCtl_LSPCLKPrescaler;

/**************************************************************************************************
 * 
 * These enumerations are passed to the SysCtl_getDeviceParametric() function as the \e parametric
 * parameter.
 * 
 *************************************************************************************************/
typedef enum {
    SYSCTL_DEVICE_PARTTYPE = 0,                 /* Device part type                             */
    SYSCTL_DEVICE_PARTNO   = 1,                 /* Device part number                           */
    SYSCTL_DEVICE_CLASSID  = 2                  /* Device class identifier                      */
} SysCtl_DeviceParametric;

/**************************************************************************************************
 * 
 * These enumerations are passed to the SysCtl_setXClk() functions as the \e divider parameter.
 * 
 *************************************************************************************************/
typedef enum {
    SYSCTL_XCLKOUT_DIV_4 = 0,                   /* XCLKOUT = SYSCLKOUT / 4 (default)            */
    SYSCTL_XCLKOUT_DIV_2 = 1,                   /* XCLKOUT = SYSCLKOUT / 2                      */
    SYSCTL_XCLKOUT_DIV_1 = 2,                   /* XCLKOUT = SYSCLKOUT / 1                      */
    SYSCTL_XCLKOUT_DIV_OFF = 3                  /* XCLKOUT = OFF                                */
} SysCtl_XClkDivider;

/**************************************************************************************************
 * 
 * The following are values that can be passed to SysCtl_setCputimer2Clk() as \e divider parameter.
 * 
 *************************************************************************************************/
typedef enum {
    SYSCTL_TMR2CLKPRESCALE_1  = 0,              /* Divide by 1 (default)                        */
    SYSCTL_TMR2CLKPRESCALE_2  = 1,              /* Divide by 2                                  */
    SYSCTL_TMR2CLKPRESCALE_4  = 2,              /* Divide by 4                                  */
    SYSCTL_TMR2CLKPRESCALE_8  = 3,              /* Divide by 8                                  */
    SYSCTL_TMR2CLKPRESCALE_16 = 4               /* Divide by 16                                 */
} SysCtl_Cputimer2ClkDivider;

/**************************************************************************************************
 * 
 * The following are values that can be passed to SysCtl_setCputimer2Clk() as \e source parameter.
 * 
 *************************************************************************************************/
typedef enum {
    SYSCTL_TMR2CLKSRCSEL_SYSCLK  = 0,           /* System clock (default)                       */
    SYSCTL_TMR2CLKSRCSEL_XTAL    = 1,           /* External crystal/clock                       */
    SYSCTL_TMR2CLKSRCSEL_INTOSC1 = 2,           /* Internal oscillator 1                        */
    SYSCTL_TMR2CLKSRCSEL_INTOSC2 = 3            /* Internal oscillator 2                        */
} SysCtl_Cputimer2ClkSource;

/**************************************************************************************************
 * 
 * Prototypes for the APIs
 * 
 *************************************************************************************************/

/**************************************************************************************************
 * 
 * \brief Wrapper function for Device_cal function
 * 
 * \param None
 * 
 * \details This is a wrapper function for the Device_cal function programmed into the TI reserved
 * OTP memory. The boot ROM automatically calls the Device_cal routine which calibrates internal
 * oscillators and ADC with device-specific data. Normally this process occurs automatically and
 * no action is required by the user. If boot ROM is bypassed, eg during the development process,
 * then calibration must be initialized by application.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
SysCtl_deviceCal(void)
{
    Device_cal();
}

/**************************************************************************************************
 * 
 * \brief Enables a peripheral clock
 * 
 * \param peripheral peripheral to enable
 * 
 * \details Peripheral clocks are enabled with this function. At power-up, all peripheral clocks
 * are disabled. They must be enabled in order to operate or respond to register reads/writes.
 * 
 * \note There should be at least 2 clock cycles delay between enabling the peripheral clock and
 * accessing the peripheral registers. The delay should be added by the user if the peripheral is
 * accessed immediately after this function call. Use asm(" RPT #2 || NOP"); to add 2 cycle delay
 * post this function call.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
SysCtl_enablePeripheral(SysCtl_PeripheralPCLOCKCR peripheral)
{
    uint16_t regOffset = ((uint16_t) peripheral & SYSCTL_PERIPH_REG_M) >> SYSCTL_PERIPH_REG_S;
    uint16_t bitIndex = ((uint16_t) peripheral & SYSCTL_PERIPH_BIT_M) >> SYSCTL_PERIPH_BIT_S;

    uint16_t clkMask = (uint32_t) 1U << bitIndex;

    EALLOW;
    HWREGH(CLKCFG_BASE + regOffset) |= clkMask;
    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Disables a peripheral clock
 * 
 * \param peripheral enumerated value from SysCtl_PeripheralPCLOCKCR
 * 
 * \details Peripheral clocks are disabled with this function. Once disabled they will not operate
 * or respond to register reads or writes.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
SysCtl_disablePeripheral(SysCtl_PeripheralPCLOCKCR peripheral)
{
    uint16_t regOffset = ((uint16_t) peripheral & SYSCTL_PERIPH_REG_M) >> SYSCTL_PERIPH_REG_S;
    uint16_t bitIndex = ((uint16_t) peripheral & SYSCTL_PERIPH_BIT_M) >> SYSCTL_PERIPH_BIT_S;

    uint16_t clkMask = (uint32_t) 1U << bitIndex;

    EALLOW;
    HWREGH(CLKCFG_BASE + regOffset) &= ~clkMask;
    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Resets the device
 * 
 * \param None
 * 
 * \details This function performs a watchdog reset of the device.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
SysCtl_resetDevice(void)
{
    EALLOW;

    /* Enable the watchdog */
    HW_REG_SET(CLKCFG_BASE, SYSCTL, WDCR, SYSCTL_WD_CHKBITS);

    /* Write a bad check value */
    HW_REG_SET(CLKCFG_BASE, SYSCTL, WDCR, 0U);

    EDIS;

    /* The device should have reset. Loop forever just in case. */
    while (true);
}

/**************************************************************************************************
 * 
 * \brief Sets the low-speed peripheral clock rate prescaler
 * 
 * \param prescaler enumerated value from SysCtl_LSPCLKPrescaler
 * 
 * \details This functions configures clock rate for the low-speed peripherals. The \e prescaler
 * parameter is the value by which the SYSCLK rate is divided to get the LSPCLK rate. For example,
 * a \e prescaler of \b SYSCTL_LSPCLK_PRESCALE_4 will result in a LSPCLK rate that is a quarter of
 * the SYSCLK rate.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
SysCtl_setLowSpeedClock(SysCtl_LSPCLKPrescaler prescaler)
{
    EALLOW;
    HW_FLD_SET(CLKCFG_BASE, SYSCTL, LOSPCP, LSPCLK, prescaler);
    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Sets the internal oscillator mode
 * 
 * \param oscSource oscillator source to be configured
 * 
 * \details This functions sets the internal oscillator mode specified by the \e oscSource
 * parameter which may take a value of \b SYSCTL_OSCSRC_OSC1 or \b SYSCTL_OSCSRC_OSC2.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
SysCtl_setInternalOscMode(uint32_t oscSource)
{
    EALLOW;

    switch (oscSource) {
    case SYSCTL_OSCSRC_OSC1:
        HW_BIT_CLR(CLKCFG_BASE, SYSCTL, CLKCTL, OSCCLKSRCSEL);
        break;

    case SYSCTL_OSCSRC_OSC2:
        HW_BIT_SET(CLKCFG_BASE, SYSCTL, CLKCTL, OSCCLKSRC2SEL);
        HW_BIT_SET(CLKCFG_BASE, SYSCTL, CLKCTL, OSCCLKSRCSEL);
        break;

    default:
        /* Invalid oscillator source */
        ASSERT(false);
        break;
    }

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Sets the external oscillator mode
 * 
 * \param oscSource oscillator source to be configured
 * 
 * \details This function sets the external oscillator mode specified by the \e oscSource
 * parameter which may take a value of \b SYSCTL_OSCSRC_XTAL, \b SYSCTL_OSCSRC_XCLK19, or
 * \b SYSCTL_OSCSRC_XCLK38.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
SysCtl_setExternalOscMode(uint32_t oscSource)
{
    EALLOW;

    switch (oscSource) {
    case SYSCTL_OSCSRC_XTAL:
        break;

    case SYSCTL_OSCSRC_XCLK19:
        HW_BIT_SET(CLKCFG_BASE, SYSCTL, XCLK, XCLKINSEL);
        break;

    case SYSCTL_OSCSRC_XCLK38:
        HW_BIT_CLR(CLKCFG_BASE, SYSCTL, XCLK, XCLKINSEL);
        break;

    default:
        /* Invalid oscillator source */
        ASSERT(false);
        break;
    }

    HW_BIT_CLR(CLKCFG_BASE, SYSCTL, CLKCTL, OSCCLKSRC2SEL);
    HW_BIT_SET(CLKCFG_BASE, SYSCTL, CLKCTL, OSCCLKSRCSEL);

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Turns on the specified oscillator source
 * 
 * \param oscSource oscillator source to be configured
 * 
 * \details This function turns on the oscillator specified by the \e oscSource parameter which
 * may take a value of \b SYSCTL_OSCSRC_OSC1, \b SYSCTL_OSCSRC_OSC2, \b SYSCTL_OSCSRC_XTAL,
 * \b SYSCTL_OSCSRC_XCLK19, or \b SYSCTL_OSCSRC_XCLK38
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
SysCtl_turnOnOsc(uint32_t oscSource)
{
    EALLOW;

    switch (oscSource) {
    case SYSCTL_OSCSRC_OSC1:
        HW_BIT_CLR(CLKCFG_BASE, SYSCTL, CLKCTL, INTOSC1OFF);
        break;

    case SYSCTL_OSCSRC_OSC2:
        HW_BIT_CLR(CLKCFG_BASE, SYSCTL, CLKCTL, INTOSC2OFF);
        break;

    case SYSCTL_OSCSRC_XTAL:
        HW_BIT_CLR(CLKCFG_BASE, SYSCTL, CLKCTL, XTALOSCOFF);
        break;

    case SYSCTL_OSCSRC_XCLK19:
        /* Intentional fall through */
    case SYSCTL_OSCSRC_XCLK38:
        HW_BIT_CLR(CLKCFG_BASE, SYSCTL, CLKCTL, XCLKINOFF);
        break;

    default:
        /* Invalid oscillator source */
        ASSERT(false);
        break;
    }

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Turns off the specified oscillator source
 * 
 * \param oscSource oscillator source to be configured
 * 
 * \details This function turns off the oscillator specified by the \e oscSource parameter which
 * may take a value of \b SYSCTL_OSCSRC_OSC1, \b SYSCTL_OSCSRC_OSC2, \b SYSCTL_OSCSRC_XTAL,
 * \b SYSCTL_OSCSRC_XCLK19, or \b SYSCTL_OSCSRC_XCLK38
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
SysCtl_turnOffOsc(uint32_t oscSource)
{
    EALLOW;

    switch (oscSource) {
    case SYSCTL_OSCSRC_OSC1:
        HW_BIT_SET(CLKCFG_BASE, SYSCTL, CLKCTL, INTOSC1OFF);
        break;

    case SYSCTL_OSCSRC_OSC2:
        HW_BIT_SET(CLKCFG_BASE, SYSCTL, CLKCTL, INTOSC2OFF);
        break;

    case SYSCTL_OSCSRC_XTAL:
        HW_BIT_SET(CLKCFG_BASE, SYSCTL, CLKCTL, XTALOSCOFF);
        break;

    case SYSCTL_OSCSRC_XCLK19:
        /* Intentional fall through */
    case SYSCTL_OSCSRC_XCLK38:
        HW_BIT_SET(CLKCFG_BASE, SYSCTL, CLKCTL, XCLKINOFF);
        break;

    default:
        /* Invalid oscillator source */
        ASSERT(false);
        break;
    }

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Enables the watchdog to run while in HALT mode
 * 
 * \param None
 * 
 * \details This function configures the watchdog to continue to run while in HALT mode.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
SysCtl_enableWatchdogInHalt(void)
{
    EALLOW;
    HW_BIT_SET(CLKCFG_BASE, SYSCTL, CLKCTL, WDHALTI);
    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Disables the watchdog from running while in HALT mode
 * 
 * \param None
 * 
 * \details This function gates the watchdog when the system enters HALT mode. INTOSC1 and INTOSC2
 * will be powered down. This is the default behavior of the device.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
SysCtl_disableWatchdogInHalt(void)
{
    EALLOW;
    HW_BIT_CLR(CLKCFG_BASE, SYSCTL, CLKCTL, WDHALTI);
    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Disables the watchdog
 * 
 * \param None
 * 
 * \details This function disables the watchdog timer.
 * 
 * \note The watchdog timer is enabled by default on reset. It is sometimes disabled before the
 * program reaches _c_int00 routine in the codestartbranch assembly routine.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
SysCtl_disableWatchdog(void)
{
    uint16_t wdcrReg = HW_REG_GET(CLKCFG_BASE, SYSCTL, WDCR);
    wdcrReg &= ~SYSCTL_WDCR_WDFLAG;

    EALLOW;
    HW_REG_SET(CLKCFG_BASE, SYSCTL, WDCR, (wdcrReg | SYSCTL_WD_CHKBITS | SYSCTL_WDCR_WDDIS));
    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Enables the watchdog
 * 
 * \param None
 * 
 * \details This function enables the watchdog timer.
 * 
 * \note The watchdog timer is enabled by default on reset. It is sometimes disabled before the
 * program reaches _c_int00 routine in the codestartbranch assembly routine.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
SysCtl_enableWatchdog(void)
{
    uint16_t wdcrReg = HW_REG_GET(CLKCFG_BASE, SYSCTL, WDCR);
    wdcrReg &= ~(SYSCTL_WDCR_WDDIS | SYSCTL_WDCR_WDFLAG);

    EALLOW;
    HW_REG_SET(CLKCFG_BASE, SYSCTL, WDCR, (wdcrReg | SYSCTL_WD_CHKBITS));
    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Resets the watchdog
 * 
 * \param None
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
SysCtl_serviceWatchdog(void)
{
    EALLOW;
    HW_REG_SET(CLKCFG_BASE, SYSCTL, WDKEY, SYSCTL_WD_ENRSTKEY);
    HW_REG_SET(CLKCFG_BASE, SYSCTL, WDKEY, SYSCTL_WD_RSTKEY);
    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Writes the first key to enter the watchdog reset
 * 
 * \param None
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
SysCtl_enableWatchdogReset(void)
{
    EALLOW;
    HW_REG_SET(CLKCFG_BASE, SYSCTL, WDKEY, SYSCTL_WD_ENRSTKEY);
    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Writes the second key to reset the watchdog
 * 
 * \param None
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
SysCtl_resetWatchdog(void)
{
    EALLOW;
    HW_REG_SET(CLKCFG_BASE, SYSCTL, WDKEY, SYSCTL_WD_RSTKEY);
    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Sets up watchdog clock (WDCLK) prescaler
 * 
 * \param prescaler enumerated value from SysCtl_WDPrescaler
 * 
 * \details This function sets up the watchdog clock (WDCLK) prescaler. This divides the clock
 * source that is already pre-divided by a fixed factor of 512.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
SysCtl_setWatchdogPrescaler(SysCtl_WDPrescaler prescaler)
{
    uint16_t wdcrReg = HW_REG_GET(CLKCFG_BASE, SYSCTL, WDCR);
    wdcrReg &= ~(SYSCTL_WDCR_WDPS_M | SYSCTL_WDCR_WDFLAG);
    
    uint16_t wdpsFld = ((uint16_t) prescaler << SYSCTL_WDCR_WDPS_S) & SYSCTL_WDCR_WDPS_M;

    EALLOW;
    HW_REG_SET(CLKCFG_BASE, SYSCTL, WDCR, (wdcrReg | wdpsFld | SYSCTL_WD_CHKBITS));
    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Gets the watchdog counter value
 * 
 * \param None
 * 
 * \return Current value of the 8-bit watchdog counter
 * 
 *************************************************************************************************/
static inline uint16_t
SysCtl_getWatchdogCounterValue(void)
{
    return HW_FLD_GET(CLKCFG_BASE, SYSCTL, WDCNTR, WDCNTR);
}

/**************************************************************************************************
 * 
 * \brief Gets the watchdog reset status
 * 
 * \param None
 * 
 * \return \b \true if the watchdog generated the last reset condition
 * 
 *************************************************************************************************/
static inline bool
SysCtl_getWatchdogResetStatus(void)
{
    return HW_BIT_GET(CLKCFG_BASE, SYSCTL, WDCR, WDFLAG);
}

/**************************************************************************************************
 * 
 * \brief Clears the watchdog reset status
 * 
 * \param None
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
SysCtl_clearWatchdogResetStatus(void)
{
    uint16_t wdcrReg = HW_REG_GET(CLKCFG_BASE, SYSCTL, WDCR);

    EALLOW;
    HW_REG_SET(CLKCFG_BASE, SYSCTL, WDCR, (wdcrReg | SYSCTL_WDCR_WDFLAG | SYSCTL_WD_CHKBITS));
    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Clears the watchdog override
 * 
 * \param None
 * 
 * \details This function clears the watchdog override and locks the watchdog time module to
 * remain in its prior state which could be either enable or disable. The watchdog timer will
 * remain in this state until the next system reset.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
SysCtl_clearWatchdogOverride(void)
{
    EALLOW;
    HW_BIT_SET(CLKCFG_BASE, SYSCTL, SCSR, WDOVERRIDE);
    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Enable the missing clock detection logic
 * 
 * \param None
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
SysCtl_enableMCD(void)
{
    EALLOW;
    HW_BIT_CLR(CLKCFG_BASE, SYSCTL, PLLSTS, MCLKOFF);
    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Disable the missing clock detection logic
 * 
 * \param None
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
SysCtl_disableMCD(void)
{
    EALLOW;
    HW_BIT_SET(CLKCFG_BASE, SYSCTL, PLLSTS, MCLKOFF);
    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Get the missing clock detection failure status
 * 
 * \param None
 * 
 * \note Failure means that the oscillator clock is missing
 * 
 * \return \b true if failure is detected or \b false if failure is not detected
 * 
 *************************************************************************************************/
static inline bool
SysCtl_isMCDClockFailureDetected(void)
{
    return HW_BIT_GET(CLKCFG_BASE, SYSCTL, PLLSTS, MCLKSTS);
}

/**************************************************************************************************
 * 
 * \brief Reset missing clock detection logic after clock failure
 * 
 * \param None
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
SysCtl_resetMCD(void)
{
    EALLOW;
    HW_BIT_SET(CLKCFG_BASE, SYSCTL, PLLSTS, MCLKCLR);
    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Connects missing clock detection clock source to stop simulating clock failure
 * 
 * \param None
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
SysCtl_connectMCDClockSource(void)
{
    EALLOW;
    HW_BIT_CLR(CLKCFG_BASE, SYSCTL, PLLSTS, OSCOFF);
    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Disconnects missing clock detection clock source to simulate clock failure
 * 
 * \param None
 * 
 * \details This is for testing the missing clock detection functionality.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
SysCtl_disconnectMCDClockSource(void)
{
    EALLOW;
    HW_BIT_SET(CLKCFG_BASE, SYSCTL, PLLSTS, OSCOFF);
    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Get the device silicon revision identifier
 * 
 * \param None
 * 
 * \details This function returns the silicon revision identifier for the device.
 * 
 * \return Silicon revision ID
 * 
 *************************************************************************************************/
static inline uint32_t
SysCtl_getDeviceRevision(void)
{
    return HW_FLD_GET(DEVCFG_BASE, SYSCTL, REVID, REVID);
}

/**************************************************************************************************
 * 
 * \brief Sets XCLKOUT divide ratio
 * 
 * \param divider enumerated value from SysCtl_XClkDivider
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
SysCtl_setXClk(SysCtl_XClkDivider divider)
{
    EALLOW;
    HW_FLD_SET(CLKCFG_BASE, SYSCTL, XCLK, XCLKOUTDIV, divider);
    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Enables the PLL
 * 
 * \param None
 * 
 * \note The PLL must be in bypass mode before altering PLLOFF bit.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
SysCtl_enablePLL(void)
{
    uint16_t pllMult = HW_FLD_GET(CLKCFG_BASE, SYSCTL, PLLCR, DIV);

    if (pllMult == 0U) {
        EALLOW;
        HW_BIT_CLR(CLKCFG_BASE, SYSCTL, PLLSTS, PLLOFF);
        EDIS;
    }
}

/**************************************************************************************************
 * 
 * \brief Disables the PLL
 * 
 * \param None
 * 
 * \note The PLL must be in bypass mode before altering PLLOFF bit.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
SysCtl_disablePLL(void)
{
    uint16_t pllMult = HW_FLD_GET(CLKCFG_BASE, SYSCTL, PLLCR, DIV);

    if (pllMult == 0U) {
        EALLOW;
        HW_BIT_SET(CLKCFG_BASE, SYSCTL, PLLSTS, PLLOFF);
        EDIS;
    }
}

/**************************************************************************************************
 * 
 * \brief Sets the PLL multiplier
 * 
 * \param multiplier value that configures the multiplier (0..12)
 * 
 * \note System clock divider must be set to divide-by-4 and missing clock detection logic must be
 * disabled before updating the PLL multiplier.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
SysCtl_setPLLMultiplier(uint16_t multiplier)
{
    ASSERT(multiplier <= 12U);

    bool mclkOff = HW_BIT_GET(CLKCFG_BASE, SYSCTL, PLLSTS, MCLKOFF);
    uint16_t divSel = HW_FLD_GET(CLKCFG_BASE, SYSCTL, PLLSTS, DIVSEL);

    if (mclkOff && (divSel == 0U)) {
        EALLOW;
        HW_FLD_SET(CLKCFG_BASE, SYSCTL, PLLCR, DIV, multiplier);
        EDIS;

        /* Wait for PLL to lock with new multiplier */
        while (!HW_BIT_GET(CLKCFG_BASE, SYSCTL, PLLSTS, PLLLOCKS)) {
            /**
             * If watchdog is enabled, it must be serviced in order not to reset the device while
             * waiting for PLL to lock. If watchdog is disabled, this will do no harm.
             */
            SysCtl_serviceWatchdog();
        }
    }
}

/**************************************************************************************************
 * 
 * \brief Sets up PLL clock divider
 * 
 * \param divider value that configures the divider (1, 2, or 4)
 * 
 * \details This functions sets up the PLL clock divider. The \e divider parameter is translated
 * to bit pattern as follows:
 *      0x0 = /4 (default on reset)
 *      0x1 = /4
 *      0x2 = /2
 *      0x3 = /1
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
SysCtl_setPLLSysClk(uint16_t divider)
{
    ASSERT((divider == 1U) || (divider == 2U) || (divider == 4U));

    divider = 4U - divider;

    EALLOW;
    HW_FLD_SET(CLKCFG_BASE, SYSCTL, PLLSTS, DIVSEL, divider);
    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Sets CPU Timer 2 clock source and divider
 * 
 * \param divider enumerated value from SysCtl_Cputimer2ClkDivider
 * \param source enumerated value from SysCtl_Cputimer2ClkSource
 * 
 * \details This function sets CPU Timer 2 clock source and divider.
 * 
 * The \e source parameter can be any one of the following:
 * - \b SYSCTL_TMR2CLKSRCSEL_SYSCLK     - System clock
 * - \b SYSCTL_TMR2CLKSRCSEL_XTAL       - External crystal/clock
 * - \b SYSCTL_TMR2CLKSRCSEL_INTOSC1    - Internal oscillator 1
 * - \b SYSCTL_TMR2CLKSRCSEL_INTOSC2    - Internal oscillator 2
 * 
 * The \e divider parameter can be any one of the following:
 * - \b SYSCTL_TMR2CLKPRESCALE_1        - Divide by 1 (default)
 * - \b SYSCTL_TMR2CLKPRESCALE_2        - Divide by 2
 * - \b SYSCTL_TMR2CLKPRESCALE_4        - Divide by 4
 * - \b SYSCTL_TMR2CLKPRESCALE_8        - Divide by 8
 * - \b SYSCTL_TMR2CLKPRESCALE_16       - Divide by 16
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
SysCtl_setCputimer2Clk(SysCtl_Cputimer2ClkDivider divider, SysCtl_Cputimer2ClkSource source)
{
    EALLOW;
    HW_FLD_SET(CLKCFG_BASE, SYSCTL, CLKCTL, TMR2CLKSRCSEL, source);
    HW_FLD_SET(CLKCFG_BASE, SYSCTL, CLKCTL, TMR2CLKPRESCALE, divider);
    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Delays for a fixed number of clock cycles
 * 
 * \param count number of delay loop iterations to perform
 * 
 * \details This function generates a constant length delay using assembly code. The loop takes
 * 5 cycles per iteration plus 9 cycles of overhead.
 * 
 * \note If count is equal to zero, the loop will underflow and run for a very long time.
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
SysCtl_delay(uint32_t count);

/**************************************************************************************************
 * 
 * \brief Calculates the system clock frequency (SYSCLK)
 * 
 * \param clockInHz frequency of the oscillator clock source (OSCCLK)
 * 
 * \details This function determines frequency of the system clock based on frequency of the
 * oscillator clock source and the PLL and clock divider configuration registers.
 * 
 * \return System clock frequency. Returns INTOSC1 frequency if a missing clock error is detected.
 * Error needs to be cleared and corrected before calling this function.
 * 
 *************************************************************************************************/
extern uint32_t
SysCtl_getClock(uint32_t clockInHz);

/**************************************************************************************************
 * 
 * \brief Configures clocking of the device
 * 
 * \param config required configuration of the device clocking
 * 
 * \details This function configures clocking of the device. The input crystal frequency,
 * oscillator to be used, use of the PLL, and the system clock divider are all configured
 * with this function.
 * 
 * The \e config parameter is the OR of several different values, many of which are grouped into
 * sets where only one can be chosen.
 * 
 *  - The system clock divider is chosen with the macro \b SYSCTL_SYSDIV(x) where x is 1, 2, or 4.
 * 
 *  - The use of the PLL is chosen with either \b SYSCTL_PLL_ENABLE or \b SYSCTL_PLL_DISABLE.
 * 
 *  - The integer multiplier is chosen with the macro \b SYSCTL_IMULT(x) where x is 0..12.
 * 
 *  - The oscillator source is chosen with \b SYSCTL_OSCSRC_OSC1, \b SYSCTL_OSCSRC_OSC2,
 *    \b SYSCTL_OSCSRC_XTAL, \b SYSCTL_OSCSRC_XCLK19, or \b SYSCTL_OSCSRC_XCLK38.
 * 
 * Locking PLL sequence is only done if PLL is enabled.
 * 
 * Function loops forever if missing clock error is detected.
 * 
 * \note See your device errata for more details about locking the PLL.
 * 
 * \return Always returns \b true
 * 
 *************************************************************************************************/
extern bool
SysCtl_setClock(uint32_t config);

/**************************************************************************************************
 * 
 * \brief Selects oscillator to be used as the system clock
 * 
 * \param oscSource oscillator source to be configured
 * 
 * \details This function configures oscillator to be used as the system clock. The \e oscSource
 * parameter may take a value defined by \b SYSCTL_OSCSRC_OSC1, \b SYSCTL_OSCSRC_OSC2,
 * \b SYSCTL_OSCSRC_XTAL, \b SYSCTL_OSCSRC_XCLK19, or \b SYSCTL_OSCSRC_XCLK38.
 * 
 * \note If missing clock error is detected, function stops emulation (ESTOP0) and loops forever.
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
SysCtl_selectOscSource(uint32_t oscSource);

/**************************************************************************************************
 * 
 * \brief Calculates the low-speed peripheral clock frequency (LSPCLK)
 * 
 * \param clockInHz frequency of the oscillator clock source (OSCCLK)
 * 
 * \details This function determines the low-speed peripheral clock frequency based on the
 * oscillator clock source frequency (from \e clockInHz) and the PLL and clock divider
 * configuration registers.
 * 
 * \return Low-speed peripheral clock frequency
 * 
 *************************************************************************************************/
extern uint32_t
SysCtl_getLowSpeedClock(uint32_t clockInHz);

/**************************************************************************************************
 * 
 * \brief Get the device part parametric value
 * 
 * \param parametric requested device parametric value
 * 
 * \details This function gets the device part parametric value.
 * 
 * The \e parametric parameter can have one of the following enumerated values:
 *  - \b SYSCTL_DEVICE_PARTTYPE - Device Part Type
 *  - \b SYSCTL_DEVICE_PARTNO   - Device Part Number
 *  - \b SYSCTL_DEVICE_CLASSID  - Device Class Identifier
 * 
 * \return Specified parametric value
 * 
 *************************************************************************************************/
extern uint16_t
SysCtl_getDeviceParametric(SysCtl_DeviceParametric parametric);

#endif /* SYSCTL_H */
