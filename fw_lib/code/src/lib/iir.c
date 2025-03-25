/**************************************************************************************************
 * 
 * \file iir.c
 * 
 * \brief Infinite impulse response (IIR) filter implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "inc/lib/iir.h"

#include "inc/lib/debug.h"
#include "inc/lib/utils.h"

/**************************************************************************************************
 * 
 * iir_setup()
 * 
 *************************************************************************************************/
int
iir_setup(struct iir *iir, enum iir_type type)
{
    ASSERT(iir);

    struct iir_priv *priv = &iir->priv;
    struct iir_usr *usr = &iir->usr;
    struct iir_prm *prm = &priv->prm;

    iir->out = &priv->mem.y[0];

    priv->type = IIR_NONE;

    float U01, U21, UY1, Y12, Y1;
    float TSBY2, WC_TSBY2, WC_TSBY2_POW2, WB_TSBY2, DEN;

    /* Bilinear transformation: 1/s <- (TS/2)*(z+1)/(z-1) */
    switch (type) {
    /* G(s) = (1 + (s*ALPHA*TLL)) / (1 + (s*TLL)) */
    case IIR_LEAD_LAG:
        ASSERT((usr->TS > 0.0f) && (usr->TLL > 0.0f) && (usr->ALPHA != 0.0f));

        TSBY2 = usr->TS / 2.0f;
        DEN = 1.0f + (usr->TLL / TSBY2);

        U01 = (1.0f + ((usr->ALPHA * usr->TLL) / TSBY2)) / DEN;
        U21 = 0.0f;
        UY1 = 2.0f / DEN;
        Y12 = 0.0f;
        Y1  = 1.0f;

        break;

    /* G(s) = 1 / ((s/WC) + 1) */
    case IIR_LOW_PASS:
        ASSERT((usr->TS > 0.0f) && (usr->FC > 0.0f));
        
        TSBY2 = usr->TS / 2.0f;
        WC_TSBY2 = (C_2PI * usr->FC) * TSBY2;
        DEN = (1.0f + WC_TSBY2);

        U01 = WC_TSBY2 / DEN;
        U21 = 0.0f;
        UY1 = (2.0f * WC_TSBY2) / DEN;
        Y12 = 0.0f;
        Y1  = 1.0f;

        break;

    /* G(s) = 1 / ((s/(sqrt(3)*WC))^2 + (s/WC) + 1) */
    case IIR_BESSEL:
        ASSERT((usr->TS > 0.0f) && (usr->FC > 0.0f));
        
        TSBY2 = usr->TS / 2.0f;
        WC_TSBY2 = C_SQRT3 * (C_2PI * usr->FC) * TSBY2;
        WB_TSBY2 = (C_SQRT3 * WC_TSBY2);
        WC_TSBY2_POW2 = (WC_TSBY2 * WC_TSBY2);
        DEN = (1.0f + WB_TSBY2 + WC_TSBY2_POW2);

        U01 = WC_TSBY2_POW2 / DEN;
        U21 = U01;
        UY1 = (4.0f * WC_TSBY2_POW2) / DEN;
        Y12 = (1.0f - WB_TSBY2 + WC_TSBY2_POW2) / DEN;
        Y1  = 1.0f;

        break;

    /* G(s) = 1 / ((s/WC)^2 + sqrt(2)*(s/WC) + 1) */
    case IIR_BUTTERWORTH:
        ASSERT((usr->TS > 0.0f) && (usr->FC > 0.0f));
        
        TSBY2 = usr->TS / 2.0f;
        WC_TSBY2 = (C_2PI * usr->FC) * TSBY2;
        WB_TSBY2 = (C_SQRT2 * WC_TSBY2);
        WC_TSBY2_POW2 = (WC_TSBY2 * WC_TSBY2);
        DEN = (1.0f + WB_TSBY2 + WC_TSBY2_POW2);

        U01 = WC_TSBY2_POW2 / DEN;
        U21 = U01;
        UY1 = (4.0f * WC_TSBY2_POW2) / DEN;
        Y12 = (1.0f - WB_TSBY2 + WC_TSBY2_POW2) / DEN;
        Y1  = 1.0f;

        break;

    /* G(s) = ((s/WC)^2 + 1) / ((s/WC)^2 + (WB/WC)*(s/WC) + 1) */
    case IIR_BAND_STOP:
        ASSERT((usr->TS > 0.0f) && (usr->FC > 0.0f) && (usr->FB > 0.0f));
        
        TSBY2 = usr->TS / 2.0f;
        WC_TSBY2 = (C_2PI * usr->FC) * TSBY2;
        WB_TSBY2 = (C_2PI * usr->FB) * TSBY2;
        WC_TSBY2_POW2 = (WC_TSBY2 * WC_TSBY2);
        DEN = (1.0f + WB_TSBY2 + WC_TSBY2_POW2);

        U01 = (1.0f + WC_TSBY2_POW2) / DEN;
        U21 = U01;
        UY1 = (4.0f * WC_TSBY2_POW2) / DEN;
        Y12 = (1.0f - WB_TSBY2 + WC_TSBY2_POW2) / DEN;
        Y1  = 1.0f;

        break;

    /* G(s) = ((WB/WC)*(s/WC)) / ((s/WC)^2 + (WB/WC)*(s/WC) + 1) */
    case IIR_BAND_PASS:
        ASSERT((usr->TS > 0.0f) && (usr->FC > 0.0f) && (usr->FB > 0.0f));
        
        TSBY2 = usr->TS / 2.0f;
        WC_TSBY2 = (C_2PI * usr->FC) * TSBY2;
        WB_TSBY2 = (C_2PI * usr->FB) * TSBY2;
        WC_TSBY2_POW2 = (WC_TSBY2 * WC_TSBY2);
        DEN = (1.0f + WB_TSBY2 + WC_TSBY2_POW2);

        U01 = WB_TSBY2 / DEN;
        U21 = -U01;
        UY1 = 0.0f;
        Y12 = (1.0f - WB_TSBY2 + WC_TSBY2_POW2) / DEN;
        Y1  = (1.0f + WB_TSBY2 - (3.0f*WC_TSBY2_POW2)) / DEN;

        break;

    case IIR_NONE:
        /* Intentional fall through */
    default:
        return -1;
    }

    prm->U01 = _IQ(U01);
    prm->U21 = _IQ(U21);
    prm->UY1 = _IQ(UY1);
    prm->Y12 = _IQ(Y12);
    prm->Y1  = _IQ(Y1);

#if MATH_TYPE == IQ_MATH
    int ret = 0;

    ret |= iq_verify(prm->U01, U01, 0.01f);
    ret |= iq_verify(prm->U21, U21, 0.01f);
    ret |= iq_verify(prm->UY1, UY1, 0.01f);
    ret |= iq_verify(prm->Y12, Y12, 0.01f);
    ret |= iq_verify(prm->Y1,  Y1,  0.01f);

    if (ret < 0) {
        return -1;
    }
#endif
    
    priv->type = type;

    iir_reset(iir);

    return 0;
}

/**************************************************************************************************
 * 
 * iir_reset()
 * 
 *************************************************************************************************/
void
iir_reset(struct iir *iir)
{
    ASSERT(iir);

    iir_force(iir, _IQ(0.L));
}

/**************************************************************************************************
 * 
 * iir_force()
 * 
 *************************************************************************************************/
__attribute__((ramfunc)) void
iir_force(struct iir *iir, _iq u0)
{
    ASSERT(iir);

    struct iir_priv *priv = &iir->priv;
    struct iir_mem *mem = &priv->mem;

    mem->u[0] = u0;
    mem->u[1] = u0;
    mem->u[2] = u0;

    mem->y[0] = u0;
    mem->y[1] = u0;
    mem->y[2] = u0;
}

/**************************************************************************************************
 * 
 * iir_run()
 * 
 *************************************************************************************************/
__attribute__((ramfunc)) _iq
iir_run(struct iir *iir, _iq u0)
{
    ASSERT(iir);

    struct iir_priv *priv = &iir->priv;
    struct iir_mem *mem = &priv->mem;
    struct iir_prm *prm = &priv->prm;

    _iq u01, u21, uy1, y12, y1;

    mem->u[0] = u0;

    switch (priv->type) {
    case IIR_LEAD_LAG:
        /* Intentional fall through */
    case IIR_LOW_PASS:
        u01 = _IQmpy(prm->U01, (mem->u[0]-mem->u[1]));
        uy1 = _IQmpy(prm->UY1, (mem->u[1]-mem->y[1]));
        y1  = _IQmpy(prm->Y1,  (mem->y[1]));

        mem->y[0] = u01 + uy1 + y1;

        break;

    case IIR_BESSEL:
        /* Intentional fall through */
    case IIR_BUTTERWORTH:
        /* Intentional fall through */
    case IIR_BAND_STOP:
        /* Intentional fall through */
    case IIR_BAND_PASS:
        u01 = _IQmpy(prm->U01, (mem->u[0]-mem->u[1]));
        u21 = _IQmpy(prm->U21, (mem->u[2]-mem->u[1]));
        uy1 = _IQmpy(prm->UY1, (mem->u[1]-mem->y[1]));
        y12 = _IQmpy(prm->Y12, (mem->y[1]-mem->y[2]));
        y1  = _IQmpy(prm->Y1,  (mem->y[1]));

        mem->y[0] = u01 + u21 + uy1 + y12 + y1;

        break;

    case IIR_NONE:
        /* Intentional fall through */
    default:
        mem->y[0] = _IQ(0.L);
        break;
    }

    mem->u[2] = mem->u[1];
    mem->u[1] = mem->u[0];
    mem->y[2] = mem->y[1];
    mem->y[1] = mem->y[0];

    return mem->y[0];
}
