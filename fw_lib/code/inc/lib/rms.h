/**************************************************************************************************
 * 
 * \file rms.h
 * 
 * \brief RMS abstraction interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_LIB_RMS_H
#define _INC_LIB_RMS_H

#include "cgen/lib/rms.h"

#include <stdint.h>
#include <stdbool.h>

#include "IQmathLib.h"

/**************************************************************************************************
 * 
 * Forward declarations
 * 
 *************************************************************************************************/

struct rms;
struct nfo;
struct iir;

struct adc;
struct adc_var;

/**************************************************************************************************
 * 
 * Values that can be passed to \e rms_run() as the \e op parameter.
 * 
 *************************************************************************************************/
enum rms_op {
    RMS_OP_UPDATE,                  /* Update RMS measurements from normalized ADC measurements */
    RMS_OP_PHYSICAL                 /* Calculate physical from normalized measurements          */
};

/**************************************************************************************************
 * 
 * Private variables definition
 * 
 *************************************************************************************************/
OBJ_PRIV_STRUCT(rms_priv,
    bool init;                      /* Indicates if RMS object has been initialized             */
);

/**************************************************************************************************
 * 
 * RMS measurement variable
 * 
 *************************************************************************************************/
struct rms_meas {
    _iq norm;                       /* Normalized value                                         */
    float phy;                      /* Physical value                                           */
};

/**************************************************************************************************
 * 
 * RMS variable definition
 * 
 *************************************************************************************************/
struct rms_var {
    struct rms_meas meas;           /* Measurement variables (normalized, physical)             */
    struct iir * const iir;         /* External low-pass filter                                 */
    const struct adc_var *adc;      /* ADC variable with measurement and conversion objects     */
};

/**************************************************************************************************
 * 
 * \brief Initializes RMS object
 * 
 * \param rms RMS object handler
 * 
 * \details This function initializes RMS object. It also setups the IIR filter as first-order
 * low-pass with the following parameters:
 * - 0.25 Hz cut-off frequency,
 * - 1 ms sample time.
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
rms_init(const struct rms *rms);

/**************************************************************************************************
 * 
 * \brief Runs RMS operation
 * 
 * \param rms RMS object handler
 * \param op run operation
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
rms_run(const struct rms *rms, enum rms_op op);

/**************************************************************************************************
 * 
 * \brief Creates new RMS object
 * 
 * \param adc ADC object handler
 * \param mod module data object handler
 * 
 * \note This function is implemented in the application layer.
 * 
 * \return RMS object handler
 * 
 *************************************************************************************************/
extern const struct rms *
rms_new(const struct adc *adc, const struct nfo *mod);

#endif /* _INC_LIB_RMS_H */
