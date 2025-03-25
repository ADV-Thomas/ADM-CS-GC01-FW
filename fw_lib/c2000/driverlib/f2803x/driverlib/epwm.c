/**************************************************************************************************
 * 
 * \file epwm.c
 * 
 * \brief ePWM driverlib implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "epwm.h"

/**************************************************************************************************
 * 
 * EPWM_setEmulationMode()
 * 
 *************************************************************************************************/
void
EPWM_setEmulationMode(uint32_t base, EPWM_EmulationMode mode)
{
    ASSERT(EPWM_isBaseValid(base));

    HW_FLD_SET(base, EPWM, TBCTL, FREESOFT, mode);
}

/**************************************************************************************************
 * 
 * EPWM_configureSignal()
 * 
 * TODO: Double check this function! It is just copied from f28004x driverlib.
 * 
 *************************************************************************************************/
void
EPWM_configureSignal(uint32_t base, const EPWM_SignalParams *signal)
{
    ASSERT(EPWM_isBaseValid(base));

    EPWM_setClockPrescaler(base, signal->tbClkDiv, signal->tbHSClkDiv);

    EPWM_setTimeBaseCounterMode(base, signal->tbCtrMode);

    float32_t tbClkInHz = (signal->sysClkInHz / (1U << (uint16_t) signal->tbClkDiv));

    if (signal->tbHSClkDiv <= EPWM_HSCLOCK_DIVIDER_4) {
        tbClkInHz /= (1U << (uint16_t) signal->tbHSClkDiv);
    } else {
        tbClkInHz /= (2U * (uint16_t) signal->tbHSClkDiv);
    }

    uint16_t tbPrdVal = 0U;
    uint16_t cmpAVal = 0U;
    uint16_t cmpBVal = 0U;

    if (signal->tbCtrMode == EPWM_COUNTER_MODE_STOP_FREEZE) {
        /* Do nothing */
    } else if (signal->tbCtrMode == EPWM_COUNTER_MODE_UP_DOWN) {
        tbPrdVal = (uint16_t) (tbClkInHz / (2.0f * signal->freqInHz));
        cmpAVal = (uint16_t) (tbPrdVal - (signal->dutyValA * tbPrdVal) + 0.5f);
        cmpBVal = (uint16_t) (tbPrdVal - (signal->dutyValB * tbPrdVal) + 0.5f);
    } else {
        tbPrdVal = (uint16_t) ((tbClkInHz / signal->freqInHz) - 1.0f);
        cmpAVal = (uint16_t) (signal->dutyValA * (tbPrdVal + 1U));
        cmpBVal = (uint16_t) (signal->dutyValB * (tbPrdVal + 1U));

        if (signal->tbCtrMode == EPWM_COUNTER_MODE_DOWN) {
            cmpAVal = (tbPrdVal + 1U) - cmpAVal;
            cmpBVal = (tbPrdVal + 1U) - cmpBVal;
        }
    }

    EPWM_setTimeBasePeriod(base, tbPrdVal);

    EPWM_disablePhaseShiftLoad(base);
    EPWM_setPhaseShift(base, 0U);
    EPWM_setTimeBaseCounter(base, 0U);

    EPWM_setCounterCompareShadowLoadMode(base,
        EPWM_COUNTER_COMPARE_A, EPWM_COMP_LOAD_ON_CNTR_ZERO);
    EPWM_setCounterCompareShadowLoadMode(base,
        EPWM_COUNTER_COMPARE_B, EPWM_COMP_LOAD_ON_CNTR_ZERO);
    
    EPWM_setCounterCompareValue(base, EPWM_COUNTER_COMPARE_A, cmpAVal);
    EPWM_setCounterCompareValue(base, EPWM_COUNTER_COMPARE_B, cmpBVal);

    if (signal->tbCtrMode == EPWM_COUNTER_MODE_UP) {
        EPWM_setActionQualifierAction(base,
            EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_HIGH, EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
        EPWM_setActionQualifierAction(base,
            EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_LOW,  EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);
        
        if (signal->invertSignalB) {
            EPWM_setActionQualifierAction(base,
                EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_LOW,  EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
            EPWM_setActionQualifierAction(base,
                EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_HIGH, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB);
        } else { /* !signal->invertSignalB */
            EPWM_setActionQualifierAction(base,
                EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_HIGH, EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
            EPWM_setActionQualifierAction(base,
                EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_LOW,  EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB);
        }
    } else if (signal->tbCtrMode == EPWM_COUNTER_MODE_DOWN) {
        EPWM_setActionQualifierAction(base,
            EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_HIGH, EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
        EPWM_setActionQualifierAction(base,
            EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_LOW,  EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPA);
        
        if (signal->invertSignalB) {
            EPWM_setActionQualifierAction(base,
                EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_LOW,  EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
            EPWM_setActionQualifierAction(base,
                EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_HIGH, EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPB);
        } else { /* !signal->invertSignalB */
            EPWM_setActionQualifierAction(base,
                EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_HIGH, EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
            EPWM_setActionQualifierAction(base,
                EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_LOW,  EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPB);
        }
    } else if (signal->tbCtrMode == EPWM_COUNTER_MODE_UP_DOWN) {
        EPWM_setActionQualifierAction(base,
            EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_LOW,  EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
        EPWM_setActionQualifierAction(base,
            EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_HIGH, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);
        EPWM_setActionQualifierAction(base,
            EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_LOW,  EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPA);
        
        if (signal->invertSignalB) {
            EPWM_setActionQualifierAction(base,
                EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_HIGH, EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
            EPWM_setActionQualifierAction(base,
                EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_LOW,  EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB);
            EPWM_setActionQualifierAction(base,
                EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_HIGH, EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPB);
        } else { /* !signal->invertSignalB */
            EPWM_setActionQualifierAction(base,
                EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_LOW,  EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
            EPWM_setActionQualifierAction(base,
                EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_HIGH, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB);
            EPWM_setActionQualifierAction(base,
                EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_LOW,  EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPB);
        }
    }
}
