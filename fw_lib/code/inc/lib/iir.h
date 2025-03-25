/**************************************************************************************************
 * 
 * \file iir.h
 * 
 * \brief Infinite impulse response (IIR) filter interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_LIB_IIR_H
#define _INC_LIB_IIR_H

#include "IQmathLib.h"

/**************************************************************************************************
 * 
 * Filter type
 * 
 *************************************************************************************************/
enum iir_type {
    IIR_NONE = 0,
    IIR_LEAD_LAG,               /* Lead-lag filter. Requires TS, TLL, ALPHA.                    */
    IIR_LOW_PASS,               /* First-order low-pass filter. Requires TS, FC.                */
    IIR_BESSEL,                 /* Second-order low-pass Bessel filter. Requires TS, FC.        */
    IIR_BUTTERWORTH,            /* Second-order low-pass Butterworth filter. Requires TS, FC.   */
    IIR_BAND_STOP,              /* Second-order band-stop filter. Requires TS, FC, FB.          */
    IIR_BAND_PASS               /* Second-order band-pass filter. Requires TS, FC, FB.          */
};

/**************************************************************************************************
 * 
 * Multipliers in filter recursive equation
 * 
 *************************************************************************************************/
struct iir_prm {
    _iq U01;                    /* Multiplier for u(k-0)-u(k-1)                                 */
    _iq U21;                    /* Multiplier for u(k-2)-u(k-1)                                 */
    _iq UY1;                    /* Multiplier for u(k-1)-y(k-1)                                 */
    _iq Y12;                    /* Multiplier for y(k-1)-y(k-2)                                 */
    _iq Y1;                     /* Multiplier for y(k-1)                                        */
};

/**************************************************************************************************
 * 
 * Filter input and output states
 * 
 *************************************************************************************************/
struct iir_mem {
    _iq u[3];                   /* Input states where u[i] is u(k-i)                            */
    _iq y[3];                   /* Output states where y[i] is y(k-i)                           */
};

/**************************************************************************************************
 * 
 * Private variables definition
 * 
 *************************************************************************************************/
struct iir_priv {
    enum iir_type type;         /* Initialized filter type                                      */
    struct iir_prm prm;         /* Internal parameters                                          */
    struct iir_mem mem;         /* Memory elements                                              */
};

/**************************************************************************************************
 * 
 * User-defined parameters
 * 
 *************************************************************************************************/
struct iir_usr {
    float TS;                   /* Sample time (s)                                              */
    float TLL;                  /* Lead-lag time constant (s)                                   */
    float ALPHA;                /* Lead-lag parameter (-)                                       */
    float FC;                   /* Cut-off frequency (Hz)                                       */
    float FB;                   /* -3 dB bandwidth for band filters (Hz)                        */
};

/**************************************************************************************************
 * 
 * Filter object definition
 * 
 *************************************************************************************************/
struct iir {
    struct iir_priv priv;       /* Private variables                            *do not access* */
    struct iir_usr usr;         /* User-defined parameters                         *read-write* */
    const _iq *out;             /* Last filter output                               *read-only* */
};

/**************************************************************************************************
 * 
 * \brief
 * 
 * \param iir filter object handler
 * \param type filter type
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
iir_setup(struct iir *iir, enum iir_type type);

/**************************************************************************************************
 * 
 * \brief
 * 
 * \param iir filter object handler
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
iir_reset(struct iir *iir);

/**************************************************************************************************
 * 
 * \brief
 * 
 * \param iir filter object handler
 * \param u0
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
iir_force(struct iir *iir, _iq u0);

/**************************************************************************************************
 * 
 * \brief
 * 
 * \param iir filter object handler
 * \param u0
 * 
 * \return 
 * 
 *************************************************************************************************/
extern _iq
iir_run(struct iir *iir, _iq u0);

#endif /* _INC_LIB_IIR_H */
