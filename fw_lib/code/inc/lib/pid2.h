/**************************************************************************************************
 *
 * \file PID2->h
 *
 * \brief PID2 controller interface (faster PID implementation)
 *
 * \author Jorge Sola
 *
 *************************************************************************************************/

#ifndef _INC_LIB_PID2_H
#define _INC_LIB_PID2_H

#include "IQmathLib.h"

struct pid2{
    _iq low;    /* Param:                                         */
    _iq high;   /* Param:                                         */
    _iq b[3];   /* Multipliers for Error inputs b(k)*e(k) */
    _iq a[3];   /* Multipliers for control outputs a(k)*u(k) */
    _iq e[3];   /* Array of current and past errors */
    _iq u[3];   /* Array of current and past control output */
};

/**************************************************************************************************
 *
 * PID type
 *
 *************************************************************************************************/
enum pid2_type {
    PID2_TYPE_NONE = 0,
    PID2_TYPE_PI,       /* Requires TS>0, KP!=0, TI!=0.                                 */
    PID2_TYPE_PD,       /* Requires TS>0, KP!=0, TD!=0, TV>0.                           */
    PID2_TYPE_PID,      /* Requires TS>0, KP!=0, TI!=0, TD!=0, TV>0.                    */
    PID2_TYPE_PR_IDEAL, /* Requires TS>0, KR!=0, FR>0.                                  */
    PID2_TYPE_PR_REAL,   /* Requires TS>0, KR!=0, FR>0, FB>0.                            */
    PID2_TYPE_LEAD,     /* Requires TS>0, phi>0, f_max>0*/
};

struct pid2_usr {
    float ts;   /* Sample time (s)                                              */
    float kp;   /* Proportional gain (-)                                        */
    float ti;   /* Integral time constant (s)                                   */
    float td;   /* Derivative time constant (s)                                 */
    float tv;   /* Derivative parasitic time constant (s)                       */
    float kr;   /* Resonant gain (-)                                            */
    float fr;   /* Resonant frequency (Hz)                                      */
    float fb;   /* Resonant bandwidth (Hz)                                      */
    float low;  /* Control action low  limit (-)                                */
    float high; /* Control action high limit (-)                                */
    float phi;  /* Phase margin incrase for Lead compensator */
    float f_max; /* Frequency at which phase is maximized in Lead compensator   */
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
extern int pid2_setup(struct pid2 *pid, enum pid2_type type, struct pid2_usr *usr);

/**************************************************************************************************
 *
 * \brief Resets the PID internal variables
 *
 * \param pid filter object handler
 *
 * \return None
 *
 *************************************************************************************************/
extern void pid2_reset(struct pid2 *pid);

/**************************************************************************************************
 *
 * \brief Forces all the internal variables of the PID to a given value
 *
 * \param pid filter object handler
 * \param u0
 *
 * \return None
 *
 *************************************************************************************************/
extern void pid2_force(struct pid2 *pid, _iq u0);

/**************************************************************************************************
 *
 * \brief Forces all the internal variables of the PID to a given value
 *
 * \param pid filter object handler
 * \param u0
 *
 * \return None
 *
 *************************************************************************************************/
extern void pid2_run_resonant(struct pid2 *pid, _iq error);

/**************************************************************************************************
 *
 * \brief Forces all the internal variables of the PID to a given value
 *
 * \param pid filter object handler
 * \param u0
 *
 * \return None
 *
 *************************************************************************************************/
extern void pid2_run_pi(struct pid2 *pid, _iq error);


extern void pid2_run_lead(struct pid2 *pid, _iq error);


/**************************************************************************************************
 *
 * \brief This newer implementation of the Resonant controller takes about ~77 cycles, which
 * is about 35 cycles faster than the previous implementation in pid.c (that one would take about
 * 114 cycles).
 *
 *************************************************************************************************/
#define MACRO_RESONANT_RUN(PID2,ERROR)         \
{                                                   \
    _iq PID2_u0 = _IQmpy(PID2->E01, (ERROR - PID2->e1)) \
                 + _IQmpy(PID2->E21, (PID2->e2-PID2->e1)) \
                 + _IQmpy(PID2->E1, PID2->e1) \
                 + _IQmpy(PID2->U12, (PID2->u1 - PID2->u2)) \
                 + _IQmpy(PID2->U1, PID2->u1); \
    PID2->e2 = PID2->e1;                                                                 \
    PID2->e1 = ERROR;                                                               \
    PID2->u2 = PID2->u1;                                                             \
    PID2->u0 = (PID2_u0 > PID2->high) ? PID2->high : ((PID2_u0 < PID2->low) ? PID2->low : PID2_u0); \
    PID2->u1 = PID2->u0;                                                            \
}


#endif /* _INC_LIB_PID2_H */
