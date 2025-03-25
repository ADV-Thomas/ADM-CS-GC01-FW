/**************************************************************************************************
 *
 * \file pid2.c
 *
 * \brief PID implementation
 *
 * \author Jorge Sola
 *
 *************************************************************************************************/

#include "inc/lib/pid2.h"

#include "inc/lib/debug.h"
#include "inc/lib/utils.h"

#define PID_C_2PI ((float)6.28318530717958647692528676655900f)

    /**
     *  CONTROLLER IMPLEMENTED IN THE FOLLOWING FORM:
     * 
     * C(z) = U(z)/E(z) = B(z)/A(z) 
     * 
     * ====> U(k) = b0*e(0)+b1*e(1)+b2*e(2)+a1*u(1)+a2*u(2)
     */
int pid2_setup(struct pid2 *pid, enum pid2_type type, struct pid2_usr *usr)
{

    /* Temporary variables */
    //float TSBY2;
    //float WB_TSBY2, WR_TSBY2, WR_TSBY2_SQ;
    //float E01, E21, E1, U12, U1;
    float WR;
    float WB;
    float T;
    float alpha[3]; /* intermediate vector */
    float beta[3];/* intermediate vector */
    float b[3];/* intermediate vector */
    float a[3];/* intermediate vector */


    /**
     *  DISCRETIZE WITH BILINEAR TRANSFORMATION
     *      s <- (2/TS)*(z-1)/(z+1)
     */
    switch (type) {

    case PID2_TYPE_PI:
        /* Parameters validity check */
        if ((usr->ts <= 0.0f) || (usr->kp == 0.0f) || (usr->ti == 0.0f))
            return -1;


        alpha[0] = 2*usr->ti;
        alpha[1] = -alpha[0];
        beta[0] = 2*usr->kp*usr->ti+usr->kp*usr->ts;
        beta[1] = usr->kp*usr->ts-2*usr->kp*usr->ti;

        b[0] = beta[0]/alpha[0];
        b[1] = beta[1]/alpha[0];
        a[1] = -alpha[1]/alpha[0];

        break;
    /**
     *  REAL PR CONTROLLER
     *      G(s) = KP + ((KR * (WB/WC) * (s/WC)) / ((s/WC)^2 + (WB/WC)*(s/WC) + 1))
     */
    case PID2_TYPE_PR_REAL:
        /* Parameters validity check */
        if ((usr->ts <= 0.0f) || (usr->kr == 0.0f) || (usr->fr <= 0.0f) || (usr->fb <= 0.0f))
            return -1;

        /* Temporary variables */
        T = usr->ts;
        WR = usr->fr * C_2PI;
        WB = usr->fb * C_2PI;

        alpha[0] = 4+2*WB*T+WR*WR*T*T;
        alpha[1] = 2*WR*WR*T*T-8;
        alpha[2] = 4-2*WB*T+WR*WR*T*T;
        beta[0] = usr->kp*alpha[0]+usr->kr*WB*2*T;
        beta[1] = usr->kp*alpha[1];
        beta[2] = usr->kp*alpha[2]-usr->kr*WB*2*T;

        b[0] = beta[0]/alpha[0];
        b[1] = beta[1]/alpha[0];
        b[2] = beta[2]/alpha[0];
        a[1] = -alpha[1]/alpha[0];
        a[2] = -alpha[2]/alpha[0];

        break;

    /**
     *  ERROR
     */
    case PID2_TYPE_NONE:
        /* Intentional fall through */
    default:
        return -1;
    }

    pid->b[2]  = _IQ(b[2]);
    pid->b[1]  = _IQ(b[1]);
    pid->b[0]  = _IQ(b[0]);
    pid->a[2]  = _IQ(a[2]);
    pid->a[1]  = _IQ(a[1]);
    pid->a[0]  = _IQ(a[0]);
    pid->low  = _IQ(usr->low);
    pid->high = _IQ(usr->high);

#if MATH_TYPE == IQ_MATH
    int ret = 0;

    /*
    ret |= iq_verify(pid->E01, E01, 0.02f);
    ret |= iq_verify(pid->E21, E21, 0.02f);
    ret |= iq_verify(pid->E1, E1, 0.02f);
    ret |= iq_verify(pid->U12, U12, 0.02f);
    ret |= iq_verify(pid->U1,  U1,  0.02f);
    */

    if (ret < 0) {
        return -1;
    }
#endif

    pid->e[1] = _IQ(0.0);
    pid->e[2] = _IQ(0.0);
    pid->u[0] = _IQ(0.0);
    pid->u[1] = _IQ(0.0);
    pid->u[2] = _IQ(0.0);
    return 0;
}

/**************************************************************************************************
 *
 * pid_reset()
 *
 *************************************************************************************************/
void pid2_reset(struct pid2 *pid)
{
    /* Memory elements */
    pid->e[1] = _IQ(0.0);
    pid->e[2] = _IQ(0.0);
    pid->u[0] = _IQ(0.0);
    pid->u[1] = _IQ(0.0);
    pid->u[2] = _IQ(0.0);
}

void pid2_force(struct pid2 *pid, _iq u0)
{
    pid->u[0] = u0;
    pid->u[1] = 0.5*u0/pid->a[1];
    pid->u[2] = 0.5*u0/pid->a[2];
    pid->e[0] = 0;
    pid->e[1] = 0;
    pid->e[2] = 0;
}

__attribute__((ramfunc))
void pid2_run_resonant(struct pid2 *pid, _iq error)
{
    /*_iq u0 = _IQmpy(pid->E01, (error - pid->e1)) 
                 + _IQmpy(pid->E21, (pid->e2-pid->e1)) 
                 + _IQmpy(pid->E1, pid->e1) 
                 + _IQmpy(pid->U12, (pid->u1 - pid->u2)) 
                 + _IQmpy(pid->U1, pid->u1); 
    pid->e2 = pid->e1;                                                                 
    pid->e1 = error;                                                               
    pid->u2 = pid->u1;                                                             
    pid->u0 = (u0 > pid->high) ? pid->high : ((u0 < pid->low) ? pid->low : u0); 
    pid->u1 = pid->u0;     */
    _iq sum = _IQ(0.0f);

    sum += _IQmpy(pid->b[0],error);
    sum += _IQmpy(pid->b[1],pid->e[1]);
    sum += _IQmpy(pid->b[2],pid->e[2]);
    sum += _IQmpy(pid->a[1],pid->u[1]);
    sum += _IQmpy(pid->a[2],pid->u[2]);

    pid->u[0] = (sum > pid->high) ? pid->high : ((sum < pid->low) ? pid->low : sum); 
    pid->u[2] = pid->u[1];
    pid->u[1] = pid->u[0];
    pid->e[2] = pid->e[1];
    pid->e[1] = error;                                
}

__attribute__((ramfunc))
void pid2_run_pi(struct pid2 *pid, _iq error)
{
    _iq sum = _IQ(0.0f);

    sum += _IQmpy(pid->b[0],error);
    sum += _IQmpy(pid->b[1],pid->e[1]);
    sum += _IQmpy(pid->a[1],pid->u[1]);

    pid->u[0] = (sum > pid->high) ? pid->high : ((sum < pid->low) ? pid->low : sum);
    pid->u[1] = pid->u[0];
    pid->e[1] = error;                                
}

__attribute__((ramfunc))
void pid2_run_lead(struct pid2 *pid, _iq error)
{
    /*_iq u0 = _IQmpy(pid->E01, (error - pid->e1))
                 + _IQmpy(pid->E1, pid->e1)
                 + _IQmpy(pid->U1, pid->u1);                                     
    pid->e1 = error;
    pid->u0 = (u0 > pid->high) ? pid->high : ((u0 < pid->low) ? pid->low : u0); 
    pid->u1 = pid->u0;    */                                                      
}
