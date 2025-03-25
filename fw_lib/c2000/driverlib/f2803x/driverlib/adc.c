/**************************************************************************************************
 * 
 * \file adc.h
 * 
 * \brief ADC driverlib implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "adc.h"

/**************************************************************************************************
 * 
 * ADC_setVREF()
 * 
 *************************************************************************************************/
void
ADC_setVREF(uint32_t base, ADC_ReferenceMode mode)
{
    ASSERT(ADC_isBaseValid(base));

    EALLOW;

    if (mode == ADC_REFERENCE_INTERNAL) {
        HW_BIT_CLR(base, ADC, CTL1, ADCREFSEL);
    } else { /* mode == ADC_REFERENCE_EXTERNAL */
        HW_BIT_SET(base, ADC, CTL1, ADCREFSEL);
    }

    EDIS;
}
