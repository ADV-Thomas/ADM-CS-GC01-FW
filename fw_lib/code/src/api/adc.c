/**************************************************************************************************
 * 
 * \file adc.c
 * 
 * \brief ADC absctraction implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "inc/api/adc.h"

#include "inc/hal/hapi.h"
#include "inc/lib/alert.h"
#include "inc/lib/data.h"
#include "inc/lib/debug.h"
#include "inc/lib/iir.h"

/**************************************************************************************************
 * 
 * adc_init()
 * 
 *************************************************************************************************/
int
adc_init(const struct adc *adc, const struct nfo *mod, struct mal *mal)
{
    ASSERT(obj_check(adc, OBJ_ADC));

    int ret;

    void **_adc = (void **) adc;
    struct adc_priv *priv = (struct adc_priv *) _adc[0];
    struct adc_var **var = (struct adc_var **) &_adc[1];

    priv->init = false;

    ret = hapi_adc_data(adc);
    if (ret < 0) {
        return -1;
    }

    unsigned i, j;
    for (i = 0U; i < priv->size; i++) {
        ASSERT(var[i] && var[i]->conv);

        if (!var[i]->enable) {
            continue;
        }

        if (var[i]->iir) {
            /* ADC variables cannot share low-pass filter */
            for (j = 0U; j < i; j++) {
                if (var[j]->iir == var[i]->iir) {
                    return -1;
                }
            }

            var[i]->iir->usr = (struct iir_usr) {
                .TS    = 0.001f, /* 1 ms */
                .TLL   = 0.0f,
                .ALPHA = 0.0f,
                .FC    = 5.0f, /* 5 Hz */
                .FB    = 0.0f
            };

            ret = iir_setup(var[i]->iir, IIR_BESSEL);
            if (ret < 0) {
                return -1;
            }
        }
    }

    ret = adc_conv(adc, CONV_HAPI);
    if (ret < 0) {
        return -1;
    }

    priv->init = true;

    ret = data_adc(mal, adc, mod, CONV_MEM, true);
    if (ret >= 0) {
        ret |= adc_conv(adc, CONV_MEM);
    }

    alert_set(ALERT_ADC, ret < 0);

    return 0;
}

/**************************************************************************************************
 * 
 * adc_conv()
 * 
 *************************************************************************************************/
int
adc_conv(const struct adc *adc, enum conv_src src)
{
    ASSERT(obj_check(adc, OBJ_ADC));

    int ret;
    
    void **_adc = (void **) adc;
    struct adc_priv *priv = (struct adc_priv *) _adc[0];
    struct adc_var **var = (struct adc_var **) &_adc[1];

    unsigned i;
    for (i = 0U; i < priv->size; i++) {
        ret = conv_update(var[i]->conv, src);
        if (ret < 0) {
            return -1;
        }
    }

    return 0;
}

/**************************************************************************************************
 * 
 * adc_run()
 * 
 *************************************************************************************************/
__attribute__((ramfunc)) void
adc_run(const struct adc *adc, enum adc_op op)
{
    ASSERT(obj_check(adc, OBJ_ADC));

    int ret;

    void **_adc = (void **) adc;
    struct adc_priv *priv = (struct adc_priv *) _adc[0];
    struct adc_var **var = (struct adc_var **) &_adc[1];

    ASSERT(priv->init);

    /* Read all (raw) measurements at once */
    if (op == ADC_OP_UPDATE) {
        ret = hapi_adc_read(adc);
        if (ret < 0) {
            return;
        }
    }
    
    unsigned i;
    for (i = 0U; i < priv->size; i++) {
        struct adc_meas *meas = &var[i]->meas;
        struct iir *iir = var[i]->iir;
        struct conv *conv = var[i]->conv;

        if (!var[i]->enable) {
            continue;
        }

        switch (op) {
        case ADC_OP_UPDATE:
            /* Intentional fall through */
        case ADC_OP_NORMALIZE:
            meas->norm = conv_r2n(conv, meas->raw);
            break;
        case ADC_OP_FILTER:
            meas->flt = iir ? iir_run(iir, meas->norm) : meas->norm;
            break;
        case ADC_OP_PHYSICAL:
            meas->phy = conv_n2p(conv, meas->flt);
            break;
        default:
            return;
        }
    }
}
