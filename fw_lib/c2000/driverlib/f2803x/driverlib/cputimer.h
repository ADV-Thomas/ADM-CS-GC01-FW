/**************************************************************************************************
 * 
 * \file cputimer.h
 * 
 * \brief CPU timer driverlib interface
 * 
 * \note Most of this driverlib has been adapted from f28004x driverlib.
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef CPUTIMER_H
#define CPUTIMER_H

#ifdef __TMS320C28XX__

#include "inc/hw_cputimer.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"

#include "cpu.h"
#include "debug.h"

#include <stdint.h>
#include <stdbool.h>

/**************************************************************************************************
 * 
 * Defines for the API
 * 
 *************************************************************************************************/

/**************************************************************************************************
 * 
 * The following are values that can be passed to CPUTimer_setEmulationMode() as the
 * \e mode parameter.
 * 
 *************************************************************************************************/
typedef enum {
    CPUTIMER_EMULATIONMODE_STOPAFTERNEXTDECREMENT = 0,
    CPUTIMER_EMULATIONMODE_STOPATZERO = 1,
    CPUTIMER_EMULATIONMODE_RUNFREE = 2
} CPUTimer_EmulationMode;

/**************************************************************************************************
 * 
 * The following are values that can be passed to CPUTimer_selectClockSource() as the
 * \e source parameter.
 * 
 *************************************************************************************************/
typedef enum {
    CPUTIMER_CLOCK_SOURCE_SYS     = 0,      /* System clock (default)                           */
    CPUTIMER_CLOCK_SOURCE_XTAL    = 1,      /* External crystal/clock                           */
    CPUTIMER_CLOCK_SOURCE_INTOSC1 = 2,      /* Internal oscillator 1                            */
    CPUTIMER_CLOCK_SOURCE_INTOSC2 = 3       /* Internal oscillator 2                            */
} CPUTimer_ClockSource;

/**************************************************************************************************
 * 
 * The following are values that can be passed to CPUTimer_selectClockSource() as the
 * \e prescaler parameter.
 * 
 *************************************************************************************************/
typedef enum {
    CPUTIMER_CLOCK_PRESCALER_1  = 0,        /* Divide by 1 (default)                            */
    CPUTIMER_CLOCK_PRESCALER_2  = 1,        /* Divide by 2                                      */
    CPUTIMER_CLOCK_PRESCALER_4  = 2,        /* Divide by 4                                      */
    CPUTIMER_CLOCK_PRESCALER_8  = 3,        /* Divide by 8                                      */
    CPUTIMER_CLOCK_PRESCALER_16 = 4         /* Divide by 16                                     */
} CPUTimer_Prescaler;

#ifdef DEBUG
static inline bool
/**************************************************************************************************
 * 
 * \brief Checks CPU timer base address
 * 
 * \param base base address of the timer module
 * 
 * \return \b true if the base address is valid
 * 
 *************************************************************************************************/
CPUTimer_isBaseValid(uint32_t base)
{
    return ((base == CPUTIMER0_BASE) || (base == CPUTIMER1_BASE) ||
        (base == CPUTIMER2_BASE));
}
#endif

/**************************************************************************************************
 * 
 * \brief Clears the overflow flag
 * 
 * \param base base address of the timer module
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CPUTimer_clearOverflowFlag(uint32_t base)
{
    ASSERT(CPUTimer_isBaseValid(base));

    HW_BIT_SET(base, CPUTIMER, TCR, TIF);
}

/**************************************************************************************************
 * 
 * \brief Disables CPU timer interrupt
 * 
 * \param base base address of the timer module
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CPUTimer_disableInterrupt(uint32_t base)
{
    ASSERT(CPUTimer_isBaseValid(base));

    HW_BIT_CLR(base, CPUTIMER, TCR, TIE);
}

/**************************************************************************************************
 * 
 * \brief Enables CPU timer interrupt
 * 
 * \param base base address of the timer module
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CPUTimer_enableInterrupt(uint32_t base)
{
    ASSERT(CPUTimer_isBaseValid(base));

    HW_BIT_SET(base, CPUTIMER, TCR, TIE);
}

/**************************************************************************************************
 * 
 * \brief Reloads CPU timer counter
 * 
 * \param base base address of the timer module
 * 
 * \details This function reloads the CPU timer counter with the values contained in the
 * CPU timer period register.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CPUTimer_reloadTimerCounter(uint32_t base)
{
    ASSERT(CPUTimer_isBaseValid(base));

    HW_BIT_SET(base, CPUTIMER, TCR, TRB);
}

/**************************************************************************************************
 * 
 * \brief Stops the CPU timer
 * 
 * \param base base address of the timer module
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CPUTimer_stopTimer(uint32_t base)
{
    ASSERT(CPUTimer_isBaseValid(base));

    HW_BIT_SET(base, CPUTIMER, TCR, TSS);
}

/**************************************************************************************************
 * 
 * \brief Resumes the CPU timer
 * 
 * \param base base address of the timer module
 * 
 * \note This function does not reload the timer counter.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CPUTimer_resumeTimer(uint32_t base)
{
    ASSERT(CPUTimer_isBaseValid(base));

    HW_BIT_CLR(base, CPUTIMER, TCR, TSS);
}

/**************************************************************************************************
 * 
 * \brief Starts (restarts) the CPU timer
 * 
 * \param base base address of the timer module
 * 
 * \note This function reloads the timer counter.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CPUTimer_startTimer(uint32_t base)
{
    ASSERT(CPUTimer_isBaseValid(base));

    HW_BIT_SET(base, CPUTIMER, TCR, TRB);
    HW_BIT_CLR(base, CPUTIMER, TCR, TSS);
}

/**************************************************************************************************
 * 
 * \brief Sets CPU timer period
 * 
 * \param base base address of the timer module
 * \param periodCount CPU timer period count
 * 
 * \details This function sets the CPU timer period count. For every value of (periodCount+1U)
 * the CPU timer overflows.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CPUTimer_setPeriod(uint32_t base, uint32_t periodCount)
{
    ASSERT(CPUTimer_isBaseValid(base));

    HW_REG_SET(base, CPUTIMER, PRD, periodCount);
}

/**************************************************************************************************
 * 
 * \brief Gets current CPU timer counter value
 * 
 * \param base base address of the timer module
 * 
 * \return Current CPU timer counter value
 * 
 *************************************************************************************************/
static inline uint32_t
CPUTimer_getTimerCount(uint32_t base)
{
    ASSERT(CPUTimer_isBaseValid(base));

    return HW_REG_GET(base, CPUTIMER, TIM);
}

/**************************************************************************************************
 * 
 * \brief Sets CPU timer prescaler value
 * 
 * \param base base address of the timer module
 * \param prescaler CPU timer clock prescaler value
 * 
 * \details This functions sets the prescaler value for the CPU timer. For every value of
 * (prescaler+1U) the CPU timer counter decrements by 1.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CPUTimer_setPreScaler(uint32_t base, uint16_t prescaler)
{
    ASSERT(CPUTimer_isBaseValid(base));

    HW_FLD_SET(base, CPUTIMER, TPRH, TDDRH, (prescaler >> 8U));
    HW_FLD_SET(base, CPUTIMER, TPR,  TDDR,  (prescaler >> 0U));
}

/**************************************************************************************************
 * 
 * \brief Gets CPU timer overflow status
 * 
 * \param base base address of the timer module
 * 
 * \return \b true if CPU timer has overflowed
 * 
 *************************************************************************************************/
static inline bool
CPUTimer_getTimerOverflowStatus(uint32_t base)
{
    ASSERT(CPUTimer_isBaseValid(base));

    return HW_BIT_GET(base, CPUTIMER, TCR, TIF);
}

/**************************************************************************************************
 * 
 * \brief Sets CPU Timer 2 clock source and prescaler
 * 
 * \param base base address of the timer module
 * \param source enumerated value from CPUTimer_ClockSource
 * \param prescaler enumerated value from CPUTimer_Prescaler
 * 
 * \details This function selects the specified clock source and prescaler value for CPU timer 2.
 * 
 * The \e source parameter can be any one of the following:
 * - \b CPUTIMER_CLOCK_SOURCE_SYS       - System clock
 * - \b CPUTIMER_CLOCK_SOURCE_XTAL      - External crystal/clock
 * - \b CPUTIMER_CLOCK_SOURCE_INTOSC1   - Internal oscillator 1
 * - \b CPUTIMER_CLOCK_SOURCE_INTOSC2   - Internal oscillator 2
 * 
 * The \e prescaler parameter can be any one of the following:
 * - \b CPUTIMER_CLOCK_PRESCALER_1      - Divide by 1 (default)
 * - \b CPUTIMER_CLOCK_PRESCALER_2      - Divide by 2
 * - \b CPUTIMER_CLOCK_PRESCALER_4      - Divide by 4
 * - \b CPUTIMER_CLOCK_PRESCALER_8      - Divide by 8
 * - \b CPUTIMER_CLOCK_PRESCALER_16     - Divide by 16
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CPUTimer_selectClockSource(uint32_t base, CPUTimer_ClockSource source,
    CPUTimer_Prescaler prescaler)
{
    ASSERT(base == CPUTIMER2_BASE);

    if (base == CPUTIMER2_BASE) {
        EALLOW;
        HW_FLD_SET(CLKCFG_BASE, SYSCTL, CLKCTL, TMR2CLKSRCSEL, source);
        HW_FLD_SET(CLKCFG_BASE, SYSCTL, CLKCTL, TMR2CLKPRESCALE, prescaler);
        EDIS;
    }
}

/**************************************************************************************************
 * 
 * \brief Sets emulation mode for the CPU timer
 * 
 * \param base base address of the timer module
 * \param mode enumerated value from CPUTimer_EmulationMode
 * 
 * \details This function sets the behaviour of the CPU timer during emulation. Valid values are
 * \b CPUTIMER_EMULATIONMODE_STOPAFTERNEXTDECREMENT, \b CPUTIMER_EMULATIONMODE_STOPATZERO, and
 * \b CPUTIMER_EMULATIONMODE_RUNFREE.
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
CPUTimer_setEmulationMode(uint32_t base, CPUTimer_EmulationMode mode);

#endif /* __TMS320C28XX__ */

#endif /* CPUTIMER_H */
