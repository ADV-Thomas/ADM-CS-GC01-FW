/**************************************************************************************************
 * 
 * \file droop.c
 * 
 * \brief Droop control module interface
 * 
 * \author Jorge Sola
 * 
 *************************************************************************************************/

#include "inc/lib/droop.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>

struct droop_priv{
    _iq node_currents[32];
    int node_timeout[32];
    _iq avg_current;
    _iq droop_coeff;
    _iq shift_coeff;
    _iq max_droop;
};


/**************************************************************************************************
 * 
 *  droop_new()
 * 
 *************************************************************************************************/
struct droop * droop_new(struct droop_usr *usr)
{
    static struct droop_priv priv;
    static struct droop droop;

    float shift_coeff;
    float droop_coeff;
    float max_droop;
    int ret = 0;

    memset(&priv, 0u, sizeof(priv));

    // Normalize user input droop parameters
    droop_coeff = usr->droop_coeff * (usr->current_fs / usr->voltage_fs); 
    shift_coeff = usr->droop_shift * (usr->current_fs / usr->voltage_fs);
    max_droop =  usr->max_droop / usr->voltage_fs;

    priv.droop_coeff = _IQ(droop_coeff);
    priv.shift_coeff = _IQ(shift_coeff);
    priv.max_droop = _IQ(max_droop);

#if MATH_TYPE == IQ_MATH
    ret |= iq_verify(priv.droop_coeff, droop_coeff, 0.01f);
    ret |= iq_verify(priv.shift_coeff, shift_coeff, 0.01f);
    ret |= iq_verify(priv.max_droop, max_droop, 0.01f);
#endif

    if(ret < 0)
    {
        return NULL;
    }
    droop.priv = &priv;
    droop.usr = *usr;
    droop.group_id = 0;

    return &droop;
}

/**************************************************************************************************
 * 
 * droop_update_node()
 * 
 *************************************************************************************************/
void droop_update_node(struct droop *droop, uint16_t node_id, _iq node_current)
{
    struct droop_priv *priv = droop->priv;
    
    priv->node_currents[node_id] = node_current;
    priv->node_timeout[node_id] = 3;
}

/**************************************************************************************************
 * 
 * droop_run()
 * 
 *************************************************************************************************/
__attribute__((ramfunc))
_iq droop_run(struct droop *droop, _iq v_ref, _iq node_current)
{
    _iq v_droop;
    _iq v_shift;
    _iq v_corrected;
    struct droop_priv *priv = droop->priv;

    v_droop = _IQmpy(priv->droop_coeff, node_current);
    v_shift = _IQmpy(priv->shift_coeff, priv->avg_current);

    if (v_droop > priv->max_droop) {
        v_droop = priv->max_droop;
    } else if (v_droop < -priv->max_droop) {
         v_droop = -priv->max_droop;
    }
    if (v_shift > priv->max_droop) {
        v_shift = priv->max_droop;
    } else if (v_shift < -priv->max_droop) {
        v_shift = -priv->max_droop;
    }

    v_corrected = v_ref - v_droop + v_shift;

    return v_corrected;
}

void droop_avg_current(struct droop *droop)
{
    int i;
    int n_devices = 0;
    _iq total_current = 0.0;
    struct droop_priv *priv = droop->priv;
    for(i=0; i<32; i++){
        if(priv->node_timeout[i] > 0){
            total_current += priv->node_currents[i];
            n_devices++;
            priv->node_timeout[i]--;
        }
        else{
            priv->node_timeout[i] = 0;
        }
    }
    priv->avg_current = _IQdiv(total_current, n_devices);
}
