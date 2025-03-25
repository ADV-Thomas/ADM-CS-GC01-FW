/**************************************************************************************************
 * 
 * \file adc.h
 * 
 * \brief ADC implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "app/adc.h"

#include "inc/lib/iir.h"
#include "inc/lib/nfo.h"
#include "inc/lib/ntc.h"

#include <stddef.h>

/**************************************************************************************************
 * 
 * adc_new()
 * 
 *************************************************************************************************/
const struct adc *
adc_new(const struct nfo *mod, struct mal *mal)
{
    ASSERT(mod);

    int ret;

    ADC_VAR_NEW(button0);
    ADC_VAR_NEW(button1);
    ADC_VAR_NEW(button2);
    ADC_VAR_NEW(button3);
    ADC_VAR_NEW(button_cw);
    ADC_VAR_NEW(coding_a);
    ADC_VAR_NEW(coding_b);

   

    switch (mod->revision) {
    case 0U:
        break;
    case 1U:
        break;
    case 2U:
        break;
    default:
        return NULL;
    }

    ADC_OBJ_NEW(
        OBJ_MEMBER_SET(button0),
        OBJ_MEMBER_SET(button1),
        OBJ_MEMBER_SET(button2),
        OBJ_MEMBER_SET(button3),
        OBJ_MEMBER_SET(button_cw),
        OBJ_MEMBER_SET(coding_a),
        OBJ_MEMBER_SET(coding_b),
    );

    ret = adc_init(&adc, mod, mal);
    if (ret < 0) {
        return NULL;
    }

    return &adc;
}
