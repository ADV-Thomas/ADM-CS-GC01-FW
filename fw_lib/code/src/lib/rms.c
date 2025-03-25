/**************************************************************************************************
 * 
 * \file rms.c
 * 
 * \brief RMS abstraction implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "inc/lib/rms.h"

#include "inc/api/adc.h"
#include "inc/lib/conv.h"
#include "inc/lib/debug.h"
#include "inc/lib/iir.h"

/**************************************************************************************************
 * 
 * rms_init()
 * 
 *************************************************************************************************/
int
rms_init(const struct rms *rms)
{
    ASSERT(obj_check(rms, OBJ_RMS));

    int ret;

    void **_rms = (void **) rms;
    struct rms_priv *priv = (struct rms_priv *) _rms[0];
    struct rms_var **var = (struct rms_var **) &_rms[1];

    priv->init = false;

    unsigned i, j;
    for (i = 0U; i < priv->size; i++) {
        ASSERT(var[i] && var[i]->iir && var[i]->adc);

        /* RMS variables cannot share filter object */
        for (j = 0U; j < i; j++) {
            if (var[j]->iir == var[i]->iir) {
                return -1;
            }
        }

        var[i]->iir->usr = (struct iir_usr) {
            .TS    = 0.001f, /* 1 ms */
            .TLL   = 0.0f,
            .ALPHA = 0.0f,
            .FC    = 0.25f, /* 0.25 Hz */
            .FB    = 0.0f
        };

        ret = iir_setup(var[i]->iir, IIR_LOW_PASS);
        if (ret < 0) {
            return -1;
        }
    }

    priv->init = true;

    return 0;
}

/**************************************************************************************************
 * 
 * \brief Updates normalized RMS measurement
 * 
 * \param iir low-pass filter handler
 * \param adc ADC measurement variable handler
 * 
 * \return normalized RMS measurement
 * 
 *************************************************************************************************/
static _iq
rms_update(struct iir *iir, const struct adc_meas *adc)
{
    ASSERT(iir && adc);

    /* Remove DC component from AC measurements */
    const _iq ac = adc->norm - adc->flt;
    const _iq ac_sq = _IQmpy(ac, ac);

    /* DC component of squared AC waveform corresponds to MSE */
    _iq mse = iir_run(iir, ac_sq);

    return _IQsqrt(mse);
}

/**************************************************************************************************
 * 
 * rms_run()
 * 
 *************************************************************************************************/
void
rms_run(const struct rms *rms, enum rms_op op)
{
    ASSERT(obj_check(rms, OBJ_RMS));

    void **_rms = (void **) rms;
    struct rms_priv *priv = (struct rms_priv *) _rms[0];
    struct rms_var **var = (struct rms_var **) &_rms[1];

    ASSERT(priv->init);

    unsigned i;
    for (i = 0U; i < priv->size; i++) {
        switch (op) {
        case RMS_OP_UPDATE:
            var[i]->meas.norm = rms_update(var[i]->iir, &var[i]->adc->meas);
            break;
        case RMS_OP_PHYSICAL:
            var[i]->meas.phy = conv_n2p(var[i]->adc->conv, var[i]->meas.norm);
            break;
        default:
            return;
        }
    }
}
