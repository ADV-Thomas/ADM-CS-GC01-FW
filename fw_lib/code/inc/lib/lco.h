/**************************************************************************************************
 * 
 * \file lco.h
 * 
 * \brief Load Current Observer (LCO) (first order extended state observer)
 * 
 * \author Jorge Sola
 * 
 *************************************************************************************************/

#ifndef _INC_LIB_LCO_H
#define _INC_LIB_LCO_H

#include "IQmathLib.h"

struct lco{
    float i_ind;            /* Input        */
    float v_cap;            /* Input        */
    float ts;
    float b;                /* Parameter    */
    float b_inv;            /* Parameter    */
    float beta[2];          /* Parameter    */
    float zeta[2];          /* Output       */
    float i_load;           /* Output       */
};

struct lco_usr{
    float ts;               
    float f_bw;
    float b;
};

/**************************************************************************************************
 *
 * \brief Initializes the PID controller
 *
 * \param pid filter object handler
 * \param type filter type
 *
 * \return 0 if operation is successful; -1 otherwise
 *
 *************************************************************************************************/
extern void lco_setup(struct lco *lco,  struct lco_usr *usr);

/**************************************************************************************************
 *
 * \brief Initializes the PID controller
 *
 * \param pid filter object handler
 * \param type filter type
 *
 * \return 0 if operation is successful; -1 otherwise
 *
 *************************************************************************************************/
extern void lco_force(struct lco *lco, float i_ind, float v_cap);


#define MACRO_LCO_RUN(LCO,I_IND,V_CAP,PHASE)              \
{                                                   \
    float PHASE##_error = LCO.zeta[0] - V_CAP;       \
    LCO.zeta[0] = LCO.zeta[0] + LCO.ts * LCO.zeta[1] - LCO.beta[0]*PHASE##_error + LCO.ts * LCO.b * I_IND;\
    LCO.zeta[1] = LCO.zeta[1] - LCO.beta[1] * PHASE##_error;\
    LCO.i_load = -LCO.b_inv * LCO.zeta[1];        \
}

#endif /* _INC_LIB_LCO_H */
