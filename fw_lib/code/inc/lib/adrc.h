/**************************************************************************************************
 *
 * \file adrc.h
 *
 * \brief ADRC controller interface
 *
 * \author Jorge Sola
 *
 *************************************************************************************************/

#ifndef _INC_LIB_adrc_H
#define _INC_LIB_adrc_H

#include "IQmathLib.h"

#include <stdbool.h>

/**************************************************************************************************
 *
 * Private variables definition
 *
 *************************************************************************************************/
struct adrc_priv {
    float b_inv;
    float beta1;
    float beta2;
    float zeta1;
    float zeta2;
    float kp;
    float out;
    bool init;
    float fal_max_gain;
};

/**************************************************************************************************
 *
 * User-defined parameters
 *
 *************************************************************************************************/
struct adrc_usr {
    float ts;
    float b;
    int f_bw_obs;
    int f_bw_ctl;
    float high;
    float low;
    float delta;
    float alpha;
};

/**************************************************************************************************
 *
 * Filter object definition
 *
 *************************************************************************************************/
struct adrc {
    struct adrc_priv priv; /* Private variables                            *do not access* */
    struct adrc_usr usr;   /* User-defined parameters                         *read-write* */
    const _iq *out;       /* Last filter output                               *read-only* */
};

/**************************************************************************************************
 *
 * \brief Initializes the adrc controller
 *
 * \param adrc filter object handler
 * \param type filter type
 *
 * \return 0 if operation is successful; -1 otherwise
 *
 *************************************************************************************************/
extern int adrc_setup(struct adrc *adrc);

/**************************************************************************************************
 *
 * \brief Resets the adrc internal variables
 *
 * \param adrc filter object handler
 *
 * \return None
 *
 *************************************************************************************************/
extern void adrc_reset(struct adrc *adrc);

/**************************************************************************************************
 *
 * \brief Forces all the internal variables of the adrc to a given value
 *
 * \param adrc filter object handler
 * \param u0
 *
 * \return None
 *
 *************************************************************************************************/
extern void adrc_force(struct adrc *adrc, _iq u0);

/**************************************************************************************************
 *
 * \brief Runs the adrc controller algorithm
 *
 * \param adrc filter object handler
 * \param u0
 *
 * \return
 *
 *************************************************************************************************/
extern _iq adrc_run(struct adrc *adrc, _iq r, _iq y);

#endif /* _INC_LIB_adrc_H */
