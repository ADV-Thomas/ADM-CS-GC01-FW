/**************************************************************************************************
 *
 * \file pid.h
 *
 * \brief PID controller interface
 *
 * \author Jorge Sola
 *
 *************************************************************************************************/

#ifndef _INC_LIB_PID_H
#define _INC_LIB_PID_H

#include "IQmathLib.h"

/**************************************************************************************************
 *
 * PID type
 *
 *************************************************************************************************/
enum pid_type {
    PID_TYPE_NONE = 0,
    PID_TYPE_PI,       /* Requires TS>0, KP!=0, TI!=0.                                 */
    PID_TYPE_PD,       /* Requires TS>0, KP!=0, TD!=0, TV>0.                           */
    PID_TYPE_PID,      /* Requires TS>0, KP!=0, TI!=0, TD!=0, TV>0.                    */
    PID_TYPE_PR_IDEAL, /* Requires TS>0, KR!=0, FR>0.                                  */
    PID_TYPE_PR_REAL   /* Requires TS>0, KR!=0, FR>0, FB>0.                            */
};

/**************************************************************************************************
 *
 * Multipliers in PID recursive equation
 *
 *************************************************************************************************/
struct pid_prm {
    _iq E01; /* Multiplier for e[k-0]-e[k-1]                                 */
    _iq E21; /* Multiplier for e[k-2]-e[k-1]                                 */
    _iq E1;  /* Multiplier for e[k-1]                                        */
    _iq U12; /* Multiplier for u[k-1]-u[k-2]                                 */
    _iq U1;  /* Multiplier for u[k-1]                                        */
    _iq low;
    _iq high;
};

/**************************************************************************************************
 *
 * PID input and output states
 *
 *************************************************************************************************/
struct pid_mem {
    _iq e1; /* Controller input  state e[k-1]                               */
    _iq e2; /* Controller input  state e[k-2]                               */
    _iq u0; /* Controller output state u[k-0]                               */
    _iq u1; /* Controller output state u[k-1]                               */
    _iq u2; /* Controller output state u[k-2]                               */
};

/**************************************************************************************************
 *
 * Private variables definition
 *
 *************************************************************************************************/
struct pid_priv {
    enum pid_type type; /* Initialized filter type                                      */
    struct pid_prm prm; /* Internal parameters                                          */
    struct pid_mem mem; /* Memory elements                                              */
};

/**************************************************************************************************
 *
 * User-defined parameters
 *
 *************************************************************************************************/
struct pid_usr {
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
};

/**************************************************************************************************
 *
 * Filter object definition
 *
 *************************************************************************************************/
struct pid {
    struct pid_priv priv; /* Private variables                            *do not access* */
    struct pid_usr usr;   /* User-defined parameters                         *read-write* */
    const _iq *out;       /* Last filter output                               *read-only* */
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
extern int pid_setup(struct pid *pid, enum pid_type type);


/**************************************************************************************************
 *
 * \brief Reconfigures the PID parameters without reseting the internal variables.
 *
 * \param pid filter object handler
 * \param type filter type
 *
 * \return 0 if operation is successful; -1 otherwise
 *
 *************************************************************************************************/
extern int pid_setup_online(struct pid *self, enum pid_type type);

/**************************************************************************************************
 *
 * \brief Resets the PID internal variables
 *
 * \param pid filter object handler
 *
 * \return None
 *
 *************************************************************************************************/
extern void pid_reset(struct pid *pid);

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
extern void pid_force(struct pid *pid, _iq u0);

/**************************************************************************************************
 *
 * \brief Runs the PID controller algorithm
 *
 * \param pid filter object handler
 * \param u0
 *
 * \return
 *
 *************************************************************************************************/
extern _iq pid_run(struct pid *pid, _iq r, _iq y);

#endif /* _INC_LIB_PID_H */
