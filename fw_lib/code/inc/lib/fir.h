/**************************************************************************************************
 * 
 * \file fir.h
 * 
 * \brief Finite impulse response (FIR) filter interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_LIB_FIR_H
#define _INC_LIB_FIR_H

#include "IQmathLib.h"

/**************************************************************************************************
 * 
 * Filter type
 * 
 *************************************************************************************************/
enum fir_type {
    FIR_NONE = 0,
    FIR_RLIM,                   /* Rate limiter. Requires TS and rate.                          */
    FIR_MAV                     /* Moving average filter. Requires buffer and size.             */
};

/**************************************************************************************************
 * 
 * Filter internal parameters
 * 
 *************************************************************************************************/
struct fir_prm {
    float rate;                 /* Rate of change normalized to sample time period (-/TS)       */
    _iq *buffer;                /* Pointer to allocated circular buffer                         */
    unsigned size;              /* Circular buffer size                                         */
};

/**************************************************************************************************
 * 
 * Filter input and output states
 * 
 *************************************************************************************************/
struct fir_mem {
    _iq u[1];                   /* Input states where u[i] is u(k-i)                            */
    _iq y[1];                   /* Output states where y[i] is y(k-i)                           */
    _iq acc[1];                 /* Array of accumulators                                        */
    unsigned i;                 /* Circular buffer iterator                                     */
};

/**************************************************************************************************
 * 
 * Private variables definition
 * 
 *************************************************************************************************/
struct fir_priv {
    enum fir_type type;         /* Initialized filter type                                      */
    struct fir_prm prm;         /* Internal parameters                                          */
    struct fir_mem mem;         /* Memory elements                                              */
};

/**************************************************************************************************
 * 
 * User-defined parameters
 * 
 *************************************************************************************************/
struct fir_usr {
    float TS;                   /* Sample time (s)                                              */
    float rate;                 /* Rate of change (-/s)                                         */
    _iq *buffer;                /* Pointer to allocated circular buffer                         */
    unsigned size;              /* Circular buffer size                                         */
};

/**************************************************************************************************
 * 
 * Filter object definition
 * 
 *************************************************************************************************/
struct fir {
    struct fir_priv priv;       /* Private variables                            *do not access* */
    struct fir_usr usr;         /* User-defined parameters                         *read-write* */
    const _iq *out;             /* Last filter output                               *read-only* */
};

/**************************************************************************************************
 * 
 * \brief
 * 
 * \param fir filter object handler
 * \param type filter type
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
fir_setup(struct fir *fir, enum fir_type type);

/**************************************************************************************************
 * 
 * \brief
 * 
 * \param fir filter object handler
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
fir_reset(struct fir *fir);

/**************************************************************************************************
 * 
 * \brief
 * 
 * \param fir filter object handler
 * \param u0
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
fir_force(struct fir *fir, _iq u0);

/**************************************************************************************************
 * 
 * \brief
 * 
 * \param fir filter object handler
 * \param u0
 * 
 * \return
 * 
 *************************************************************************************************/
extern _iq
fir_run(struct fir *fir, _iq u0);

#endif /* _INC_LIB_FIR_H */
