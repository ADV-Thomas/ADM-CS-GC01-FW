/**************************************************************************************************
 *
 * \file pid.c
 *
 * \brief PID implementation
 *
 * \author Jorge Sola
 *
 *************************************************************************************************/

#include "inc/lib/pid.h"

#include "inc/lib/debug.h"
#include "inc/lib/utils.h"

#define PID_C_2PI ((float)6.28318530717958647692528676655900f)

static int pid_set_parameters(struct pid *self, enum pid_type type);

/**************************************************************************************************
 *
 * pid_setup()
 *
 *************************************************************************************************/
int pid_setup(struct pid *self, enum pid_type type)
{
    int ret;
    /* Pointers for shorter notation */
    ret = pid_set_parameters(self, type);

    /* Reset */
    pid_reset(self);

    /* Return */
    return ret;
}

/**************************************************************************************************
 *
 * pid_setup_online()
 *
 *************************************************************************************************/
int pid_setup_online(struct pid *self, enum pid_type type)
{
    int ret;
    /* Pointers for shorter notation */
    ret = pid_set_parameters(self, type);

    /* Return */
    return ret;
}

/**************************************************************************************************
 *
 * pid_set_parameters()
 *
 *************************************************************************************************/
int pid_set_parameters(struct pid *self, enum pid_type type)
{
    /* Pointers for shorter notation */
    struct pid_priv *priv = &self->priv;
    struct pid_usr *in    = &self->usr;
    struct pid_prm *prm   = &priv->prm;

    /* Reset initialized type */
    priv->type = PID_TYPE_NONE;

    /* Temporary variables */
    float GAIN, DEN;
    float TSBY2, TSBY2TI;
    float WB_TSBY2, WR_TSBY2, WR_TSBY2_SQ;
    float E01, E21, E1, U12, U1;

    self->out = &priv->mem.u0;


    /**
     *  DISCRETIZE WITH BILINEAR TRANSFORMATION
     *      s <- (2/TS)*(z-1)/(z+1)
     */
    switch (type) {
    /**
     *  PI CONTROLLER
     *      G(s) = KP * (1 + 1/(TI*s))
     */
    case PID_TYPE_PI:
        /* Parameters validity check */
        if ((in->ts <= 0.0f) || (in->kp == 0.0f) || (in->ti == 0.0f))
            return -1;

        /* Temporary variables */
        TSBY2   = in->ts / 2.0f;
        TSBY2TI = TSBY2 / in->ti;

        /* Private parameters */
        E01 = in->kp * (1.0f + TSBY2TI);
        E21 = 0.0f;
        E1  = 2.0f * in->kp * TSBY2TI;
        U12 = 0.0f;
        U1  = 1.0f;

        break;

    /**
     *  PD CONTROLLER
     *      G(s) = KP * (1 + ((TD*s)/(1+TV*s)))
     */
    case PID_TYPE_PD:
        /* Parameters validity check */
        if ((in->ts <= 0.0f) || (in->kp == 0.0f) || (in->td == 0.0f) || (in->tv <= 0.0f))
            return -1;

        /* Temporary variables */
        TSBY2 = in->ts / 2.0f;
        DEN   = 1.0f / (in->tv + TSBY2);

        /* Private parameters */
        E01 = in->kp * (1.0f + (in->td * DEN));
        E21 = 0.0f;
        E1  = in->kp * (in->ts * DEN);
        U12 = 0.0f;
        U1  = (2.0f * (in->tv * DEN)) - 1.0f;

        break;

    /**
     *  PID CONTROLLER
     *      G(s) = KP * (1 + 1/(TI*s) + (TD*s)/(1+TV*s))
     */
    case PID_TYPE_PID:
        /* Parameters validity check */
        if ((in->ts <= 0.0f) || (in->kp == 0.0f) || (in->ti == 0.0f) || (in->td == 0.0f)
            || (in->tv <= 0.0f))
            return -1;

        /* Temporary variables */
        TSBY2 = in->ts / 2.0f;
        GAIN  = (in->kp / in->ti) / (in->tv + TSBY2);

        /* Private parameters */
        E01 = GAIN * ((in->ti * (in->tv + in->td + TSBY2)) + (TSBY2 * (+in->tv + TSBY2)));
        E21 = GAIN * ((in->ti * (in->tv + in->td - TSBY2)) + (TSBY2 * (-in->tv + TSBY2)));
        E1  = GAIN * (in->ts * in->ts);
        U12 = (in->tv - TSBY2) / (in->tv + TSBY2);
        U1  = 1.0f;

        break;

    /**
     *  IDEAL PR CONTROLLER
     *      G(s) = KP + ((KR * s) / (s^2 + WR^2))
     */
    case PID_TYPE_PR_IDEAL:
        /* Parameters validity check */
        if ((in->ts <= 0.0f) || (in->kr == 0.0f) || (in->fr <= 0.0f))
            return -1;

        /* Temporary variables */
        TSBY2       = in->ts / 2.0f;
        WR_TSBY2    = PID_C_2PI * in->fr * TSBY2;
        WR_TSBY2_SQ = WR_TSBY2 * WR_TSBY2;
        DEN         = 1.0f + WR_TSBY2_SQ;

        /* Private parameters */
        E01 = in->kp + (in->kr * TSBY2 / DEN);
        E21 = in->kp - (in->kr * TSBY2 / DEN);
        E1  = 4.0f * in->kp * WR_TSBY2_SQ / DEN;
        U12 = 1.0f;
        U1  = 1.0f - (4.0f * WR_TSBY2_SQ / DEN);

        break;

    /**
     *  REAL PR CONTROLLER
     *      G(s) = KP + ((KR * (WB/WC) * (s/WC)) / ((s/WC)^2 + (WB/WC)*(s/WC) + 1))
     */
    case PID_TYPE_PR_REAL:
        /* Parameters validity check */
        if ((in->ts <= 0.0f) || (in->kr == 0.0f) || (in->fr <= 0.0f) || (in->fb <= 0.0f))
            return -1;

        /* Temporary variables */
        TSBY2       = in->ts / 2.0f;
        WB_TSBY2    = PID_C_2PI * in->fb * TSBY2;
        WR_TSBY2    = PID_C_2PI * in->fr * TSBY2;
        WR_TSBY2_SQ = WR_TSBY2 * WR_TSBY2;
        DEN         = 1.0f + WB_TSBY2 + WR_TSBY2_SQ;

        /* Private parameters */
        E01 = in->kp + (in->kr * WB_TSBY2 / DEN);
        E21 = in->kp - (((2.0f * in->kp) + in->kr) * WB_TSBY2 / DEN);
        E1  = 4.0f * in->kp * WR_TSBY2_SQ / DEN;
        U12 = 1.0f - (2.0f * WB_TSBY2 / DEN);
        U1  = 1.0f - (4.0f * WR_TSBY2_SQ / DEN);

        break;

    /**
     *  ERROR
     */
    case PID_TYPE_NONE:
        /* Intentional fall through */
    default:
        return -1;
    }

    prm->E01  = _IQ(E01);
    prm->E1   = _IQ(E1);
    prm->E21  = _IQ(E21);
    prm->U12  = _IQ(U12);
    prm->U1   = _IQ(U1);
    prm->low  = _IQ(in->low);
    prm->high = _IQ(in->high);

#if MATH_TYPE == IQ_MATH
    int ret = 0;

    /*
    ret |= iq_verify(prm->E01, E01, 0.02f);
    ret |= iq_verify(prm->E21, E21, 0.02f);
    ret |= iq_verify(prm->E1, E1, 0.02f);
    ret |= iq_verify(prm->U12, U12, 0.02f);
    ret |= iq_verify(prm->U1,  U1,  0.02f);
    */

    if (ret < 0) {
        return -1;
    }
#endif

    /* Initialization successful */
    priv->type = type;
    return 0;
}

/**************************************************************************************************
 *
 * pid_reset()
 *
 *************************************************************************************************/
void pid_reset(struct pid *self)
{
    /* Pointers for shorter notation */
    struct pid_mem *mem = &self->priv.mem;

    /* Memory elements */
    mem->e1 = _IQ(0.0f);
    mem->e2 = _IQ(0.0f);
    mem->u0 = _IQ(0.0f);
    mem->u1 = _IQ(0.0f);
    mem->u2 = _IQ(0.0f);
}

/**************************************************************************************************
 *
 * pid_force()
 *
 *************************************************************************************************/
void pid_force(struct pid *self, _iq u)
{
    /* Pointers for shorter notation */
    struct pid_priv *priv = &self->priv;
    struct pid_prm *prm   = &priv->prm;
    struct pid_mem *mem   = &priv->mem;

    /* Force output with anti-windup limit */
#if MATH_TYPE == IQ_MATH
    mem->u0 = (u > prm->high) ? prm->high : ((u < prm->low) ? prm->low : u);
#else
    mem->u0 = (u > self->usr.high) ? self->usr.high : ((u < self->usr.low) ? self->usr.low : u);
#endif

    /* Force states */
    mem->e1 = _IQ(0.0f);
    mem->e2 = _IQ(0.0f);
    mem->u1 = _IQdiv(mem->u0, prm->U1);
    mem->u2 = mem->u1;
}

/**************************************************************************************************
 *
 * pid_run()
 *
 *************************************************************************************************/
__attribute__((ramfunc)) _iq pid_run(struct pid *self, _iq r, _iq y)
{
    /* Pointers for shorter notation */
    struct pid_priv *priv = &self->priv;
    struct pid_prm *prm   = &priv->prm;
    struct pid_mem *mem   = &priv->mem;

    /* Temporary variables */
    _iq e0 = r - y;
    _iq u0, e01, e21, u12;

    switch (priv->type) {
    case PID_TYPE_PI:
        /* Intentional fall through */
    case PID_TYPE_PD:
        /* Temporary variables */
        e01 = e0 - mem->e1;

        /* Control action */
        u0 = _IQmpy(prm->E01, e01) + _IQmpy(prm->E1, mem->e1) + _IQmpy(prm->U1, mem->u1);

        /* Update states */
        mem->e1 = e0;

        break;

    case PID_TYPE_PID:
        /* Intentional fall through */
    case PID_TYPE_PR_IDEAL:
        /* Intentional fall through */
    case PID_TYPE_PR_REAL:
        /* Temporary variables */
        e01 = e0 - mem->e1;
        e21 = mem->e2 - mem->e1;
        u12 = mem->u1 - mem->u2;

        /* Control action */
        u0 = _IQmpy(prm->E01, e01) + _IQmpy(prm->E21, e21) + _IQmpy(prm->E1, mem->e1)
             + _IQmpy(prm->U12, u12) + _IQmpy(prm->U1, mem->u1);

        /* Update states */
        mem->e2 = mem->e1;
        mem->e1 = e0;
        mem->u2 = mem->u1;

        break;

    case PID_TYPE_NONE:
        /* Intentional fall through */
    default:
        /* Control action */
        u0 = _IQ(0.0f);
    }

    /* Update output with anti-windup limit */
#if MATH_TYPE == IQ_MATH
    mem->u0 = (u0 > prm->high) ? prm->high : ((u0 < prm->low) ? prm->low : u0);
#else
    mem->u0 = (u0 > self->usr.high) ? self->usr.high : ((u0 < self->usr.low) ? self->usr.low : u0);
#endif

    /* Update state u[k-1] */
    mem->u1 = mem->u0;

    /* Return */
    return mem->u0;
}
