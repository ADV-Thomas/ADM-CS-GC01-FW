/**************************************************************************************************
 *
 * \file mppt.h
 *
 * \brief Maximum Power Point Tracking module
 *
 * \author Jorge Sola
 *
 *************************************************************************************************/

#ifndef _INC_LIB_MPPT_H
#define _INC_LIB_MPPT_H


#include <stdint.h>

struct mppt_priv {
    float v_old;
    float p_old;
    float v_oc;
    float v_max_step;
    float v_min_step;
    float i_sc;
    float v_ref;
    float p_mppt;
    float dPdV;
    float debug_dV;
    float debug_dP;
    float debug_step;
    float debug_dCdV;
    float debug_dE;
    float debug_dPdV_prev;
    float debug_p_mppt;
};

struct mppt_usr {
    float max_v_step;
    float min_v_step;
};

struct mppt_out {
    float *v_ref;
    uint16_t mppt_reached;
};

struct mppt {
    struct mppt_usr usr;
    struct mppt_priv priv;
    struct mppt_out out;
};

/**************************************************************************************************
 *
 * @brief Initializes an MPPT controller
 *
 * @param mppt Pointer to a MPPT struct
 *
 *************************************************************************************************/
void mppt_init(struct mppt *mppt, float v_init);

/**************************************************************************************************
 *
 * @brief Runs the MPPT controller. It can (should) be run from OUTSIDE the interrupt, for example
 * in the ctl_background() task
 *
 * @param mppt Pointer to a MPPT struct
 * @param pv_voltage Measured solar panel voltage
 * @param power Measured solar panel power
 *
 * @return Voltage reference for maximum power point.
 *
 *************************************************************************************************/
float mppt_run(struct mppt *mppt, float pv_voltage, float power);
#endif /* _INC_LIB_MPPT_H */
