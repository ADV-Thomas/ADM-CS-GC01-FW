/**************************************************************************************************
 * 
 * \file ntc.c
 * 
 * \brief NTC temperature sensor implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "inc/lib/ntc.h"

#include "inc/lib/debug.h"

#include <math.h>

/**************************************************************************************************
 * 
 * ntc_new()
 * 
 *************************************************************************************************/
const struct ntc *
ntc_new(void)
{
    /** Parameters for Vishay BC Components \b NTCLE100E3103JB0 */
    static struct ntc ntc = {
        .T = { 0.07660619752981f, 0.00077964708529f, 0.00001976368826f },
        .R = { -14.633412298125f, -327.42367426290f, 7867.31001432504f, 256609.033901496f },
        .R_25 = 10000.0f,
        .R_PU = 10000.0f
    };

    return &ntc;
}

/**************************************************************************************************
 * 
 * ntc_temperature()
 * 
 *************************************************************************************************/
float
ntc_temperature(const struct ntc *ntc, _iq norm)
{
    ASSERT(ntc && (ntc->R_25 > 0U) && (ntc->R_PU > 0U));

    float norm_f = _IQtoF(norm);

    ASSERT((norm_f > 0.0f) && (norm_f < 1.0f));
    if ((norm_f <= 0.0f) || (norm_f >= 1.0f)) {
        return -273.15f;
    }

    /* Argument for natural logarithm: R_NTC / R_25 */
    float arg = (norm_f / (1.0f - norm_f)) * (ntc->R_PU / ntc->R_25);

    /* Steinhart-Hart equation base */
    float x1 = logf(arg);
    float x2 = x1 * x1;
    float x3 = x1 * x2;

    /* Steinhart-Hart equation denominator */
    float sum = 1.0f + (ntc->T[0] * x1) + (ntc->T[1] * x2) + (ntc->T[2] * x3);

    /* Temperature in degrees Celsius via Steinhart-Hart equation */
    float T = ((25.0f + 273.15f) / sum) - 273.15f;

    return T;
}

/**************************************************************************************************
 * 
 * ntc_normalized()
 * 
 *************************************************************************************************/
_iq
ntc_normalized(const struct ntc *ntc, float T)
{
    ASSERT(ntc && (ntc->R_25 > 0U) && (ntc->R_PU > 0U));

    /* Input temperature in Kelvins */
    float TK = T + 273.15f;
    ASSERT(TK > 0.0f);

    /* Inverse Steinhart-Hart equation basic term */
    float x0 = 1.0f;
    float x1 = x0 / TK;
    float x2 = x1 / TK;
    float x3 = x2 / TK;

    /* Inverse Steinhart-Hart equation exponent */
    float sum = ntc->R[0] * (1.0f + (ntc->R[1] * x1) + (ntc->R[2] * x2) + (ntc->R[3] * x3));

    /* NTC resistance via inverse Steinhart-Hart equation */
    float R_NTC = ntc->R_25 * expf(sum);

    float norm_f = R_NTC / (R_NTC + ntc->R_PU);

    return _IQ(norm_f);
}
