/**************************************************************************************************
 * 
 * \file fir.c
 * 
 * \brief Finite impulse response (FIR) filter implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "inc/lib/fir.h"

#include "inc/lib/debug.h"

/**************************************************************************************************
 * 
 * fir_setup()
 * 
 *************************************************************************************************/
int
fir_setup(struct fir *fir, enum fir_type type)
{
    ASSERT(fir);

    struct fir_priv *priv = &fir->priv;
    struct fir_usr *usr = &fir->usr;
    struct fir_prm *prm = &priv->prm;

    fir->out = &priv->mem.y[0];

    priv->type = FIR_NONE;

    switch (type) {
    case FIR_RLIM:
        ASSERT((usr->TS > 0.0f) && (usr->rate > 0.0f));
        
        prm->rate = _IQ(usr->rate * usr->TS);

        break;

    case FIR_MAV:
        ASSERT(usr->buffer && (usr->size > 1U));
        
        prm->buffer = usr->buffer;
        prm->size = usr->size;
        
        break;

    case FIR_NONE:
        /* Intentional fall through */
    default:
        return -1;
    }

    priv->type = type;

    fir_reset(fir);

    return 0;
}

/**************************************************************************************************
 * 
 * fir_reset()
 * 
 *************************************************************************************************/
void
fir_reset(struct fir *fir)
{
    ASSERT(fir);

    fir_force(fir, _IQ(0.L));
}

/**************************************************************************************************
 * 
 * fir_force()
 * 
 *************************************************************************************************/
void
fir_force(struct fir *fir, _iq u0)
{
    ASSERT(fir);

    struct fir_priv *priv = &fir->priv;
    struct fir_mem *mem = &priv->mem;
    struct fir_prm *prm = &priv->prm;

    mem->u[0] = u0;
    mem->y[0] = u0;

    switch (priv->type) {
    case FIR_RLIM:
        break;

    case FIR_MAV:
        mem->acc[0] = u0 * prm->size;

        for (mem->i = 0U; mem->i < prm->size; mem->i++)
            *(prm->buffer + mem->i) = u0;
        
        mem->i = 0U;

        break;

    case FIR_NONE:
        /* Intentional fall through */
    default:
        return;
    }
}

/**************************************************************************************************
 * 
 * fir_run()
 * 
 *************************************************************************************************/
__attribute__((ramfunc))
_iq fir_run(struct fir *fir, _iq u0)
{
    ASSERT(fir);

    struct fir_priv *priv = &fir->priv;
    struct fir_mem *mem = &priv->mem;
    struct fir_prm *prm = &priv->prm;

    _iq delta;
    _iq *elem;

    mem->u[0] = u0;

    switch (priv->type) {
    case FIR_RLIM:
        delta = mem->u[0] - mem->y[0];

        if (delta > prm->rate)
            mem->y[0] += prm->rate;
        else if (delta < -prm->rate)
            mem->y[0] -= prm->rate;
        else
            mem->y[0] = mem->u[0];
        
        break;

    case FIR_MAV:
        elem = prm->buffer + mem->i;

        mem->acc[0] += (mem->u[0] - *elem);

        *elem = mem->u[0];

        if (++mem->i >= prm->size)
            mem->i = 0U;
        
        mem->y[0] = mem->acc[0] / prm->size;

        break;

    case FIR_NONE:
        /* Intentional fall through */
    default:
        ASSERT(0);
        mem->y[0] = mem->u[0];
    }

    return mem->y[0];
}
