/**************************************************************************************************
 * 
 * \file adc.h
 * 
 * \brief ADC abstraction interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_API_ADC_H
#define _INC_API_ADC_H

#include "cgen/api/adc.h"
#include "inc/lib/conv.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "IQmathLib.h"

/**************************************************************************************************
 * 
 * Forward declarations
 * 
 *************************************************************************************************/

struct adc;
struct iir;
struct nfo;
struct mal;

/**************************************************************************************************
 * 
 * Values that can be passed to \e adc_run() as the \e op parameter.
 * 
 *************************************************************************************************/
enum adc_op {
    ADC_OP_UPDATE,                  /* Update raw measurements and do normalization             */
    ADC_OP_NORMALIZE,               /* Normalize raw measurements                               */
    ADC_OP_FILTER,                  /* Filter normalized measurements                           */
    ADC_OP_PHYSICAL                 /* Calculate physical from filtered measurements            */
};

/**************************************************************************************************
 * 
 * Private variables definition
 * 
 *************************************************************************************************/
OBJ_PRIV_STRUCT(adc_priv,
    bool init;                      /* Indicates if ADC object has been initialized             */
);

/**************************************************************************************************
 * 
 * ADC measurement variable
 * 
 *************************************************************************************************/
struct adc_meas {
    uint32_t raw;                   /* Raw (unprocessed) value                                  */
    _iq norm;                       /* Normalized value                                         */
    _iq flt;                        /* Filtered value                                           */
    float phy;                      /* Physical value (filtered)                                */
};

/**************************************************************************************************
 * 
 * ADC variable definition
 * 
 *************************************************************************************************/
struct adc_var {
    bool enable;                    /* Enable or disable ADC variable                           */
    struct adc_meas meas;           /* Measurement variables (raw, normalized, physical)        */
    struct iir * const iir;         /* Low-pass filter handler                                  */
    struct conv * const conv;       /* Measurement conversion object handler                    */
};

/**************************************************************************************************
 * 
 * \brief Initializes ADC object
 * 
 * \param adc ADC object handler
 * \param mod module data object handler
 * \param mal memory handler
 * 
 * \details This function initializes ADC object. If low-pass filter is provided, it also setups
 * the filter as second-order low-pass Bessel with the following parameters:
 * - 1 Hz cut-off frequency,
 * - 1 ms sample time.
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
adc_init(const struct adc *adc, const struct nfo *mod, struct mal *mal);

/**************************************************************************************************
 * 
 * \brief Initializes ADC conversion data
 * 
 * \param adc ADC object handler
 * \param src conversion data source
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
adc_conv(const struct adc *adc, enum conv_src src);

/**************************************************************************************************
 * 
 * \brief Runs ADC operation
 * 
 * \param adc ADC object handler
 * \param op run operation
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
adc_run(const struct adc *adc, enum adc_op op);

/**************************************************************************************************
 * 
 * \brief Creates new ADC object
 * 
 * \param mod module data object handler
 * \param mal memory handler
 * 
 * \note This function is implemented in the application layer.
 * 
 * \return ADC object handler
 * 
 *************************************************************************************************/
extern const struct adc *
adc_new(const struct nfo *mod, struct mal *mal);

#endif /* _INC_API_ADC_H */
