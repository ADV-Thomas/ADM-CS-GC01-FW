/**************************************************************************************************
 * 
 * \file clk.c
 * 
 * \brief System clock driver implementation
 * 
 * \note Only 20 MHz , 25Mhz and 30 MHz external crystal oscillators are supported.
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "inc/drv/clk.h"

#include "driverlib/cputimer.h"
#include "driverlib/sysctl.h"

#include <stdint.h>
#include <stddef.h>

/**************************************************************************************************
 * 
 * Defines for the driver
 * 
 *************************************************************************************************/

#define CLK_FREQ_1MHZ       1000000U
#define CLK_FREQ_5MHZ       5000000U
#define CLK_FREQ_20MHZ      20000000U
#define CLK_FREQ_25MHZ      25000000U
#define CLK_FREQ_30MHZ      30000000U


#if defined(F28P65X)
#define CLK_CPU_FREQ        200000000U          /* Target (maximum) CPU frequency in Hz         */
#define CLK_XTAL_DELAY      40000U              /* 2 ms delay at  200MHz CPU frequency          */
#elif defined(F28004X)
#define CLK_CPU_FREQ        100000000U          /* Target (maximum) CPU frequency in Hz         */
#define CLK_XTAL_DELAY      20000U              /* 1 ms delay at 100 MHz CPU frequency          */
#elif defined(F2803X)
#define CLK_CPU_FREQ        60000000U           /* Target (maximum) CPU frequency in Hz         */
#define CLK_XTAL_DELAY      12000U              /* 1 ms delay at 60 MHz CPU frequency           */
#else
#error "hal.c - dsp not defined"
#endif

#define CLK_EST_CYCLES      1000U

/* Common clock configuration */
#define CLK_CONFIG          (SYSCTL_PLL_ENABLE | SYSCTL_SYSDIV(2))

/* Calculates PLL multiplier to produce 2*CPU_FREQ from SRC source */
#define CLK_PLL_IMULT(SRC)  (SYSCTL_IMULT((2U*CLK_CPU_FREQ)/SRC))

#ifndef VIRTUAL
/**************************************************************************************************
 * 
 * \brief Estimates external crystal frequency
 * 
 * \param align boundary to round estimated crystal frequency in Hz
 * 
 * \details This function estimates external crystal oscillator frequency. It is assumed that CPU
 * is clocked at its maximum frequency defined by \b CLK_CPU_FREQ macro. The crystal frequency is
 * estimated by running two timers - timer 1 clocked by CPU clock source, and timer 2 clocked by
 * external crystal oscillator. The estimated crystal frequency is rounded to the nearest boundary
 * defined by \e align parameter.
 * 
 * \note It is assumed that external crystal frequency is less than maximum CPU frequency.
 * 
 * \return Estimated external crystal frequency in Hz
 * 
 *************************************************************************************************/
static uint32_t
clk_xtal(uint32_t align)
{
    ASSERT(align > 0U);

    /* Turn on external crystal oscillator */
    #if defined(F28P65X) 
        SysCtl_turnOnOsc(SYSCTL_OSCSRC_XTAL_SE);
    #else 
        SysCtl_turnOnOsc(SYSCTL_OSCSRC_XTAL);
    #endif

    /* Delay at least 1 ms to ensure crystal oscillator is up and running */
    SysCtl_delay(CLK_XTAL_DELAY);

    /* Configure timer clocked by CPU clock source */
    CPUTimer_stopTimer(CPUTIMER1_BASE);
    CPUTimer_setPeriod(CPUTIMER1_BASE, 100U);
    CPUTimer_setPreScaler(CPUTIMER1_BASE, 0U);
    CPUTimer_selectClockSource(CPUTIMER1_BASE, CPUTIMER_CLOCK_SOURCE_SYS,
        CPUTIMER_CLOCK_PRESCALER_1);


    /* Configure timer clocked by external crystal oscillator */
    CPUTimer_stopTimer(CPUTIMER2_BASE);
    CPUTimer_setPeriod(CPUTIMER2_BASE, 100U);
    CPUTimer_setPreScaler(CPUTIMER2_BASE, 0U);
    CPUTimer_selectClockSource(CPUTIMER2_BASE, CPUTIMER_CLOCK_SOURCE_XTAL,
        CPUTIMER_CLOCK_PRESCALER_1);

    uint32_t sys = 0U;
    uint32_t xtal = 0U;

    CPUTimer_startTimer(CPUTIMER1_BASE);
    CPUTimer_startTimer(CPUTIMER2_BASE);

    /* Determine frequency ratio */
    while (sys < CLK_EST_CYCLES) {
        if (CPUTimer_getTimerOverflowStatus(CPUTIMER1_BASE)) {
            CPUTimer_clearOverflowFlag(CPUTIMER1_BASE);
            sys++;
        } else {
            /* Keep waiting until timer 1 overflows */
            continue;
        }

        if (CPUTimer_getTimerOverflowStatus(CPUTIMER2_BASE)) {
            CPUTimer_clearOverflowFlag(CPUTIMER2_BASE);
            xtal++;
        }
    }

    CPUTimer_stopTimer(CPUTIMER1_BASE);
    CPUTimer_stopTimer(CPUTIMER2_BASE);

    /* Configure system (CPU) clock as CPU timer 2 clock source */
    CPUTimer_selectClockSource(CPUTIMER2_BASE, CPUTIMER_CLOCK_SOURCE_SYS,
        CPUTIMER_CLOCK_PRESCALER_1);

    /**
     * Estimate external crystal frequency. Since \b CLK_CPU_FREQ is multiple of \b CLK_EST_CYCLES,
     * it is safe to first divide and then multiply. Otherwise, the 64-bit integer arithmetic would
     * have to be used which is not supported in hardware, i.e. it requires special routines from
     * real-time support (RTS) library (see \e ll_div28.asm).
     */
    uint32_t f = (CLK_CPU_FREQ / CLK_EST_CYCLES) * xtal;

    /* Align to nearest frequency boundary */
    f = ((f + (align >> 1)) / align) * align;

    return f;
}
#endif

/**************************************************************************************************
 * 
 * Create clock object
 * 
 *************************************************************************************************/

static struct _clk clk = {
    .xtal = { .Hz = 0U, .MHz = 0U },
    .cpu  = { .Hz = 0U, .MHz = 0U },
    .can  = { .Hz = 0U, .MHz = 0U },
    .pwm  = { .Hz = 0U, .MHz = 0U },
    .adc  = { .Hz = 0U, .MHz = 0U }
};

/**************************************************************************************************
 * 
 * clk_init()
 * 
 *************************************************************************************************/
int
clk_init(void)
{
#ifndef VIRTUAL
    /**
     * Configure system (CPU) clock to operate at target (maximum) frequency derived from internal
     * oscillator 1 that operates at 10 MHz.
     */
    SysCtl_setClock(SYSCTL_OSCSRC_OSC1 | CLK_CONFIG | CLK_PLL_IMULT(SYSCTL_DEFAULT_OSC_FREQ));

    /* Check CPU clock frequency */
    uint32_t cpu = SysCtl_getClock(SYSCTL_DEFAULT_OSC_FREQ);
    if (cpu != CLK_CPU_FREQ) {
        return -1;
    }

    /**
     * Estimate external crystal oscillator frequency. It is expected that the estimated crystal
     * frequency is either 20 MHz or 30 MHz.
     */
    uint32_t xtal = clk_xtal(CLK_FREQ_5MHZ);
    #if !defined(F28P65X)
        if ((xtal != CLK_FREQ_20MHZ) && (xtal != CLK_FREQ_30MHZ)) {
            return -1;
        }
    #else
        if ((xtal != CLK_FREQ_25MHZ) ) {
                return -1;
        }
    #endif
    
    clk.xtal = (struct clk) {
        .Hz = xtal,
        .MHz = xtal / CLK_FREQ_1MHZ
    };

    /**
     * Configure system (CPU) clock to operate at target (maximum) frequency derived from external
     * 20-MHz or 30-MHz crystal oscillator.
     */
    if (xtal == CLK_FREQ_20MHZ) {
        SysCtl_setClock(SYSCTL_OSCSRC_XTAL | CLK_CONFIG | CLK_PLL_IMULT(CLK_FREQ_20MHZ));
    } else if (xtal == CLK_FREQ_30MHZ) {
        #if defined(F2803X)
                /**
                 * Note that 30 MHz external crystal is outside of recommended operating range for f2803x
                 * devices, which is specified to 5..20 MHz. This bug has been fixed in new designs!
                 */
                SysCtl_setClock(SYSCTL_OSCSRC_XTAL | CLK_CONFIG | CLK_PLL_IMULT(CLK_FREQ_30MHZ));
        #else 
                return -1;
        #endif
    } else if (xtal == CLK_FREQ_25MHZ) {
      
        #if defined(F28P65X)
            SysCtl_setClock(SYSCTL_OSCSRC_XTAL_SE | CLK_CONFIG | CLK_PLL_IMULT(CLK_FREQ_25MHZ));
        #else 
            return -1;
        #endif

    } else {
        return -1;
    }

    /* Check CPU clock frequency */
    cpu = SysCtl_getClock(clk.xtal.Hz);
    if (cpu != CLK_CPU_FREQ) {
        return -1;
    }

    clk.cpu = (struct clk) {
        .Hz = cpu,
        .MHz = cpu / CLK_FREQ_1MHZ
    };

    /* Low-speed peripheral clock runs at half the maximum CPU frequency */
    SysCtl_setLowSpeedClock(SYSCTL_LSPCLK_PRESCALE_2);

    /* XCLKOUT is not used */
    SysCtl_setXClk(SYSCTL_XCLKOUT_DIV_1);
#else
    /**
     * Clock does not need to be configured in virtual environment. Initialize clock handlers to
     * their expected values since other program modules might need this information.
     */

    clk.xtal = (struct clk) {
        .Hz = CLK_FREQ_20MHZ,
        .MHz = CLK_FREQ_20MHZ / CLK_FREQ_1MHZ
    };

    /* CPU clock operates at 60 MHz (f2803x) or 100 MHz (f28004x) */
    clk.cpu = (struct clk) {
        .Hz = CLK_CPU_FREQ,
        .MHz = CLK_CPU_FREQ / CLK_FREQ_1MHZ
    };
#endif

#if defined(F2803X)
    clk.can = (struct clk) {
        .Hz = clk.cpu.Hz / 2U,
        .MHz = clk.cpu.MHz / 2U
    };
#elif defined(F28004X)  ||  defined(F28P65X) 
    clk.can = clk.cpu;
#else
#error "hal.c - dsp not defined"
#endif
    
    clk.pwm = clk.cpu;

    return 0;
}

/**************************************************************************************************
 * 
 * clk_get()
 * 
 *************************************************************************************************/
const struct _clk *
clk_get()
{
    return &clk;
}

/**************************************************************************************************
 * 
 * clk_set_adc()
 * 
 *************************************************************************************************/
void
clk_set_adc(uint32_t f)
{
    clk.adc = (struct clk) {
        .Hz = f,
        .MHz = f / CLK_FREQ_1MHZ
    };
}

/**************************************************************************************************
 * 
 * clk_delay()
 * 
 *************************************************************************************************/
__attribute__((ramfunc)) void
clk_delay(uint32_t time)
{
    if (time > 0U) {
        /**
         * The \e SysCtl_delay() function decrements the \e count variable in a loop until it gets
         * to \b 0. If \e count is initially 0 the \e SysCtl_delay() will overflow for a very long
         * delay. It takes 5 CPU cycles per one loop iteration and the total function overhead is
         * calculated to be 29 CPU cycles (equals 6 delay loops).
         */
        uint32_t count = (time * (CLK_CPU_FREQ / CLK_FREQ_1MHZ / 5U)) - 6U;

        SysCtl_delay(count);
    }
}
