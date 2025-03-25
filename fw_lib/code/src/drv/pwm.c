/**************************************************************************************************
 * 
 * \file pwm.h
 * 
 * \brief PWM driver implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "inc/drv/pwm.h"

#include "driverlib/epwm.h"
#include "driverlib/sysctl.h"

#include "inc/drv/clk.h"
#include "inc/lib/utils.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**************************************************************************************************
 * 
 * \brief Connects ePWM peripheral to I/O pin
 * 
 * \param io I/O pin configuration code
 * \param num ePWM generator number (1..8)
 * \param ab \b false for A output, \b true for B output
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
static int
connect(enum io io, uint16_t num, bool ab)
{
    enum io_fun fun;
    enum io_group group;

    io_decode(io, NULL, &fun, &group);

    if (group == IO_NO_GROUP) {
        return 0;
    } else if (group != IO_EPWM) {
        return -1;
    }

    uint16_t _num = ((fun & 0xFU) >> 1) + 1U;
    if (_num != num) {
        return -1;
    }

    bool _ab = (fun & 0x1U) != 0U;
    if (_ab ^ ab) {
        return -1;
    }

    int ret;

    ret = io_connect(io, IO_NONE);
    if (ret < 0) {
        return -1;
    }

    return 0;
}

/**************************************************************************************************
 * 
 * \brief Setup single PWM generator
 * 
 * \param var PWM variable
 * 
 * \return 0 if operations is successful; -1 otherwise
 * 
 *************************************************************************************************/
static int
setup_generator(struct _pwm_var *var)
{
    ASSERT(var && (var->base > 0U));

    /**
     * ENABLE EPWM PERIPHERAL CLOCK
     * - enables ePWM peripheral clock based on ePWM generator number
     */

#if defined(F2803X)
    uint16_t _clock = 0x000DU;
#elif defined(F28004X) ||  defined(F28P65X)
    uint16_t _clock = 0x0002U;
#endif

    SysCtl_enablePeripheral((SysCtl_PeripheralPCLOCKCR) (((var->num - 1U) << 8U) | _clock));

    /**
     * It takes 2-5 CPU cycles before peripheral clock is enabled. Configuration registers cannot
     * be modified before peripheral clock has been enabled!
     */
#if defined(F2803X)
    asm(" RPT #2 || NOP");
#elif defined(F28004X)
    asm(" RPT #5 || NOP");
#endif

    const struct _clk *_clk = clk_get();
    const struct clk *clk = &_clk->pwm;

    ASSERT(_clk);

    /**
     * TIME-BASE COUNTER CONFIGURATION
     * - clock prescaler is selected for highest PWM resolution
     * - time-base counter is configured for up-down count mode
     */

    ASSERT(var->freq > 0U);

    uint16_t div;
    for (div = 0U; div < 8U; div++) {
        uint32_t period = div_round_u32(clk->Hz, ((1U << div) * 2U * var->freq));
        if (period == 0U) {
            return -1;
        } else if (period <= 0xFFFFU) {
            var->period = period;
            break;
        } else if (div == 8U) {
            return -1;
        }
    }

    EPWM_setClockPrescaler(var->base, (EPWM_ClockDivider) div, EPWM_HSCLOCK_DIVIDER_1);

    EPWM_setPeriodLoadMode(var->base, EPWM_PERIOD_SHADOW_LOAD);
    EPWM_setTimeBaseCounterMode(var->base, EPWM_COUNTER_MODE_UP_DOWN);
    EPWM_setTimeBasePeriod(var->base, var->period);
    EPWM_setTimeBaseCounter(var->base, 0U);

    if (var->phase_e) {
        uint32_t period = (uint32_t) var->period * 2U;
        uint32_t phase = (uint32_t) ((var->phase_0 * period) + 0.5f);
        EPWM_SyncCountMode mode = EPWM_COUNT_MODE_UP_AFTER_SYNC;

        if (phase > var->period) {
            phase = period - phase;
            mode = EPWM_COUNT_MODE_DOWN_AFTER_SYNC;
        }

        EPWM_setPhaseShift(var->base, phase);
        EPWM_setCountModeAfterSync(var->base, mode);

        EPWM_enablePhaseShiftLoad(var->base);
    } else { /* !phase_e */
        EPWM_disablePhaseShiftLoad(var->base);
    }

    /**
     * ACTION QUALIFIER CONFIGURATION
     * - duty-cycle is updated when counter reaches PERIOD value
     * - initial duty-cycle is set to 0%
     * - CMPA controls both outA and outB if out_B_e is false
     * - CMPB controls outB if out_B_e is true
     */

    EPWM_setActionQualifierAction(var->base, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_HIGH,
        EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPA);
    EPWM_setActionQualifierAction(var->base, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_LOW,
        EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);
    
    EPWM_setCounterCompareShadowLoadMode(var->base, EPWM_COUNTER_COMPARE_A,
        EPWM_COMP_LOAD_ON_CNTR_ZERO);
    
    EPWM_setCounterCompareValue(var->base, EPWM_COUNTER_COMPARE_A, 0U);
    
    if (var->out_B_e) {
        EPWM_setActionQualifierAction(var->base, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_LOW,
            EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPB);
        EPWM_setActionQualifierAction(var->base, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_HIGH,
            EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB);
        
        EPWM_setCounterCompareShadowLoadMode(var->base, EPWM_COUNTER_COMPARE_B,
            EPWM_COMP_LOAD_ON_CNTR_PERIOD);
        
        EPWM_setCounterCompareValue(var->base, EPWM_COUNTER_COMPARE_B, 0U);
    }

    /** CAUTION: The following ePWM synchronization settings assumes that 
     * ePWM will always be the master PWM generator, and that the rest of
     * PWM variables are consecutive (i.e. EPWM2, EPWM3, etc.). If not,
     * the app layer can always configure this in the _hapi_setup() function
     */
    //TODO F28P65X
    /*if(var->base == EPWM1_BASE){
        EPWM_setSyncOutPulseMode(var->base, EPWM_SYNC_OUT_PULSE_ON_COUNTER_ZERO);
    }else{
        EPWM_setSyncOutPulseMode(var->base, EPWM_SYNC_OUT_PULSE_ON_EPWMxSYNCIN);
    }*/

    /**
     * DEAD-BAND SUBMODULE CONFIGURATION
     * - enables dead-band insertion if outputs A and B are used together
     * - disables dead-band insertion if output B is used independently
     */
    
    if (!var->out_B_e) {
        EPWM_setRisingEdgeDeadBandDelayInput(var->base, EPWM_DB_INPUT_EPWMA);
        EPWM_setFallingEdgeDeadBandDelayInput(var->base, EPWM_DB_INPUT_EPWMA);

        EPWM_setDeadBandDelayPolarity(var->base, EPWM_DB_RED, EPWM_DB_POLARITY_ACTIVE_HIGH);
        EPWM_setDeadBandDelayPolarity(var->base, EPWM_DB_FED, EPWM_DB_POLARITY_ACTIVE_LOW);

        EPWM_setDeadBandDelayMode(var->base, EPWM_DB_RED, true);
        EPWM_setDeadBandDelayMode(var->base, EPWM_DB_FED, true);

        EPWM_setDeadBandCounterClock(var->base, EPWM_DB_COUNTER_CLOCK_FULL_CYCLE);

        uint16_t dead = (uint16_t) (((var->dead_0 * clk->MHz) / (1U << div)) + 0.5f);

        EPWM_setRisingEdgeDelayCount(var->base, dead);
        EPWM_setFallingEdgeDelayCount(var->base, dead);
    } else { /* out_B_e */
        EPWM_setDeadBandDelayMode(var->base, EPWM_DB_RED, false);
        EPWM_setDeadBandDelayMode(var->base, EPWM_DB_FED, false);
    }

    /**
     * DIGITAL COMPARE SUMODULE CONFIGURATION
     * Configure digital compare A event 2 (DCAEVT2) for cycle-by-cycle trip
     * This digital compare input is wired (hardcoded) to TRIP 4, allowing asynchronous
     * cycle by cycle trip of the pwm.
     * 
     * NOTE: here only the digital compare is configured, but nothing is wired to its
     * inputs. This is done in the Hardware Comparator submodules (WCH) if desired
     */

    // TODO: for the moment, the trip input is HARDCODED to TRIP 4. Maybe make it configurable?
    EPWM_selectDigitalCompareTripInput(var->base, EPWM_DC_TRIP_TRIPIN4, EPWM_DC_TYPE_DCAH);
    EPWM_setTripZoneDigitalCompareEventCondition(var->base, EPWM_TZ_DC_OUTPUT_A2, EPWM_TZ_EVENT_DCXH_HIGH);
    EPWM_setDigitalCompareEventSource(var->base, EPWM_DC_MODULE_A, EPWM_DC_EVENT_2, EPWM_DC_EVENT_SOURCE_ORIG_SIGNAL);
    EPWM_setDigitalCompareEventSyncMode(var->base, EPWM_DC_MODULE_A, EPWM_DC_EVENT_2, EPWM_DC_EVENT_INPUT_NOT_SYNCED);
    EPWM_enableTripZoneSignals(var->base, EPWM_TZ_SIGNAL_DCAEVT2);
    
    /**
     * TRIP-ZONE SUBMODULE CONFIGURATION
     * - sets safe state for outputs A and B to low
     * - forces one-shot trip event which puts outputs A and B in safe state
     */
    

    EPWM_setTripZoneAction(var->base, EPWM_TZ_ACTION_EVENT_TZA, EPWM_TZ_ACTION_LOW);
    EPWM_setTripZoneAction(var->base, EPWM_TZ_ACTION_EVENT_TZB, EPWM_TZ_ACTION_LOW);

    EPWM_clearTripZoneFlag(var->base, EPWM_TZ_INTERRUPT_OST);

    EPWM_forceTripZoneEvent(var->base, EPWM_TZ_FORCE_EVENT_OST);

    /**
     * CONNECT TO I/O PINS
     * - checks if I/O pins correspond to selected ePWM generator
     * - checks if I/O pins correspond to A and B outputs
     */

    int ret = 0;

    ret |= connect(var->io_A, var->num, false);
    ret |= connect(var->io_B, var->num, true);

    if (ret < 0) {
        return -1;
    }

    return 0;
}

/**************************************************************************************************
 * 
 * Generic PWM object definition
 * 
 *************************************************************************************************/
_PWM_OBJ_STRUCT(
    _PWM_OBJ_STRUCT_MEMBER(var);
);

/**************************************************************************************************
 * 
 * pwm_setup()
 * 
 *************************************************************************************************/
int
pwm_setup(const struct _pwm *pwm)
{
    struct _pwm_priv *priv = pwm->priv;
    struct _pwm_var * const *var = &pwm->var;

    ASSERT(obj_check(pwm, OBJ__PWM));

    int ret;

    /** TODO: Explain! */
    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

    unsigned i;
    for (i = 0U; i < priv->size; i++) {
        ASSERT(var[i]);

        if (!var[i]->enable) {
            continue;
        }

#if defined(F2803X)
        ASSERT((var[i]->num >= 1U) && (var[i]->num <= 7U));
#elif defined(F28004X)
        ASSERT((var[i]->num >= 1U) && (var[i]->num <= 8U));
#endif

        var[i]->base = EPWM1_BASE + ((var[i]->num - 1U) * (EPWM2_BASE - EPWM1_BASE));

        uint16_t mask = 1U << var[i]->num;
        if ((priv->init & mask) == 0U) {
            ret = setup_generator(var[i]);
            if (ret < 0) {
                return -1;
            } else {
                priv->init |= mask;
            }
        } else {
            return -1;
        }
    }

    /** TODO: Explain! */
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

    return 0;
}

/**************************************************************************************************
 * 
 * pwm_trigger()
 * 
 *************************************************************************************************/
void
pwm_trigger(uint32_t base, EPWM_ADCStartOfConversionType type,
    EPWM_ADCStartOfConversionSource source, uint16_t prescale)
{
    ASSERT(EPWM_isBadeValid(base));
    ASSERT(prescale < 4U);

    EPWM_setADCTriggerSource(base, type, source);
    EPWM_setADCTriggerEventPrescale(base, type, prescale);

    EPWM_enableADCTrigger(base, type);
}
