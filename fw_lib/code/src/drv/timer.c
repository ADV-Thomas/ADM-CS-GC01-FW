/**************************************************************************************************
 * 
 * \file timer.h
 * 
 * \brief CPU timer driver implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "inc/drv/timer.h"

#include "driverlib/cputimer.h"

#include "inc/drv/clk.h"

/**************************************************************************************************
 * 
 * Private global variables and defines
 * 
 *************************************************************************************************/

/* CPU timer allocation tokens */
static bool token[] = { false, false, false };

/* Number of available CPU timers */
#define TIMER_COUNT     (sizeof(token) / sizeof(token[0]))

/* Gets CPU timer module base address from identifier */
#define TIMER_BASE(ID)  (CPUTIMER0_BASE + (ID * (CPUTIMER1_BASE - CPUTIMER0_BASE)))

/**************************************************************************************************
 * 
 * timer_setup()
 * 
 *************************************************************************************************/
int
timer_setup(uint32_t freq)
{
    const struct _clk *_clk = clk_get();
    ASSERT(_clk);

    unsigned id;

    /* Find first available timer */
    for (id = 0U; (id < TIMER_COUNT) && token[id]; id++) ;

    /* Check if there is available timer */
    if (id >= TIMER_COUNT) {
        return -1;
    }

    /* Get timer module base address */
    uint32_t base = TIMER_BASE(id);

    uint32_t period = 0xFFFFFFFFU;

    if (freq > 0U) {
        period = _clk->cpu.Hz / freq;

        if (period > 0U) {
            period = period - 1U;
        }
    }

    CPUTimer_stopTimer(base);
    
    CPUTimer_setPeriod(base, period);
    CPUTimer_setPreScaler(base, 0U);

    CPUTimer_setEmulationMode(base, CPUTIMER_EMULATIONMODE_STOPAFTERNEXTDECREMENT);

    /* Make sure CPU timer 2 is clocked by the CPU clock */
    if (base == CPUTIMER2_BASE) {
        CPUTimer_selectClockSource(CPUTIMER2_BASE, CPUTIMER_CLOCK_SOURCE_SYS,
            CPUTIMER_CLOCK_PRESCALER_1);
    }

    CPUTimer_startTimer(base);

    /* Allocate timer */

    return id;
}

/**************************************************************************************************
 * 
 * timer_free()
 * 
 *************************************************************************************************/
void
timer_free(unsigned id)
{
    ASSERT(id < TIMER_COUNT);

    CPUTimer_stopTimer(TIMER_BASE(id));
    token[id] = false;
}

/**************************************************************************************************
 * 
 * timer_status()
 * 
 *************************************************************************************************/
bool
timer_status(unsigned id)
{
    ASSERT(id < TIMER_COUNT);

    uint32_t base = TIMER_BASE(id);

    if (CPUTimer_getTimerOverflowStatus(base)) {
        CPUTimer_clearOverflowFlag(base);
        return true;
    } 

    return false;
}

/**************************************************************************************************
 * 
 * timer_state()
 * 
 *************************************************************************************************/
__attribute__((ramfunc)) uint32_t
timer_state(unsigned id)
{
    ASSERT(id < TIMER_COUNT);

    return CPUTimer_getTimerCount(TIMER_BASE(id));
}
