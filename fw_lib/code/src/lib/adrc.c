/**************************************************************************************************
 *
 * \file adrc.c
 *
 * \brief ADRC controller interface
 *
 * \author Jorge Sola
 *
 *************************************************************************************************/

#include "inc/lib/adrc.h"
#include "inc/lib/utils.h"

#include "math.h"

/**************************************************************************************************
 * 
 * adrc_setup()
 * 
 *************************************************************************************************/
int adrc_setup(struct adrc *adrc)
{
    if(!adrc->usr.b || !adrc->usr.ts || !adrc->usr.f_bw_obs ||  !adrc->usr.f_bw_ctl){
        return -1;
    }
    adrc->priv.b_inv = 1.0f/adrc->usr.b;
    adrc->priv.beta1 = adrc->usr.ts * 2 * adrc->usr.f_bw_obs * C_2PI;
    adrc->priv.beta2 = adrc->usr.ts * adrc->usr.f_bw_obs * C_2PI * adrc->usr.f_bw_obs * C_2PI;
    adrc->priv.kp = adrc->usr.f_bw_ctl * C_2PI;
    adrc->out = &adrc->priv.out;
    
    if(!adrc->usr.delta){
        adrc->usr.delta = 0;
    }

    adrc->priv.fal_max_gain = 1.0f/sqrtf(adrc->usr.delta);

    adrc_reset(adrc);
    

    return 0;
}

/**************************************************************************************************
 * 
 * adrc_reset()
 * 
 *************************************************************************************************/
void adrc_reset(struct adrc *adrc)
{
    adrc_force(adrc,0.0);
}

/**************************************************************************************************
 * 
 * adrc_force()
 * 
 *************************************************************************************************/
void adrc_force(struct adrc *adrc, _iq u0)
{
    struct adrc_priv *priv = &adrc->priv;

    priv->out = u0;
    priv->zeta2 = -adrc->usr.b * u0;
    priv->init = false;
}

static float fal(struct adrc *adrc, float e)
{
    float e0_abs = fabsf(e);
    if(e0_abs >= adrc->usr.delta){
        return (sqrtf(e0_abs) * sign(e));
    }else{
        return (adrc->priv.fal_max_gain * e);
    }
}

/**************************************************************************************************
 * 
 * adrc_run()
 * 
 *************************************************************************************************/
__attribute__((ramfunc)) _iq adrc_run(struct adrc *adrc, _iq r, _iq y)
{
    struct adrc_priv *priv = &adrc->priv;
    struct adrc_usr *usr = &adrc->usr;
    float state_error;
    float tracking_error;

    //float fal;

    /* The first time that ADRC is run (not initialized), the observer state is initialized to
    the measured output.*/
    if(!priv->init){
        priv->zeta1 = y;
        priv->init = true;
    }

    state_error = adrc->priv.zeta1 - y;
    priv->zeta1 = priv->zeta1 + usr->ts * priv->zeta2 - priv->beta1*state_error + usr->ts * usr->b * priv->out;
    priv->zeta2 = priv->zeta2 - priv->beta2 * state_error;
    
    tracking_error = r - adrc->priv.zeta1;

    /* Here we use non-linear feedback of the error */
    priv->out = priv->b_inv * (priv->kp * fal(adrc,tracking_error) - priv->zeta2);
    priv->out = priv->out > usr->high ? usr->high : priv->out < usr->low ? usr->low : priv->out;

    return priv->out;

}
