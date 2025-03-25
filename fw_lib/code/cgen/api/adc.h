/**************************************************************************************************
 * 
 * \file adc.h
 * 
 * \brief ADC abstraction code generator
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _CGEN_API_ADC_H
#define _CGEN_API_ADC_H

#include "inc/api/obj.h"

/**************************************************************************************************
 * 
 * \brief Generates new static private variables structure
 * 
 * \param TYPE corresponding object structure type
 * 
 * \details This function generates private variables structure for ADC object:
 * - variable type is 'struct adc_priv'
 * - variable name is 'priv'
 * - object structure type is 'struct adc'
 * - object type is 'OBJ_ADC'
 * 
 *************************************************************************************************/
#define ADC_PRIV_NEW()                                                                          \
    OBJ_PRIV_NEW(adc_priv, priv, adc, OBJ_ADC,                                                  \
        .init = false                                                                           \
    )

/**************************************************************************************************
 * 
 * \brief Generates new static ADC object
 * 
 * \param ... optional initializers
 * 
 * \details This function generates ADC object with its private variables structure:
 * - object structure type is defined by \e TYPE parameter
 * - object name is 'adc'
 * 
 *************************************************************************************************/
#define ADC_OBJ_NEW(...)                                                                        \
    ADC_PRIV_NEW();                                                                             \
    OBJ_NEW(adc, adc, &priv, __VA_ARGS__)

/**************************************************************************************************
 * 
 * \brief Generates new ADC variable with low-pass filter and conversion object
 * 
 * \param NAME variable name
 * 
 *************************************************************************************************/
#define ADC_VAR_NEW(NAME)                                                                       \
    static struct iir NAME##_iir;                                                               \
    CONV_OBJ_NEW(NAME##_conv);                                                                  \
    static struct adc_var NAME = {                                                              \
        .enable = true,                                                                         \
        .meas = {                                                                               \
            .raw = 0U,                                                                          \
            .norm = _IQ(0.L),                                                                   \
            .flt = _IQ(0.L),                                                                    \
            .phy = 0.0f                                                                         \
        },                                                                                      \
        .iir = &NAME##_iir,                                                                     \
        .conv = &NAME##_conv                                                                    \
    }

/**************************************************************************************************
 * 
 * \brief Sets custom normalized-physical transformation
 * 
 * \param NAME variable name
 * \param OBJ transformation object
 * \param N2P normalized to physical function handler
 * \param P2N physical to normalized function handler
 * 
 *************************************************************************************************/
#define ADC_SET_NPT(NAME, OBJ, N2P, P2N)                                                        \
    CONV_SET_NPT(NAME##_conv, OBJ, N2P, P2N)

/**************************************************************************************************
 * 
 * \brief Sets conversion data in ADC variable
 * 
 * \param NAME variable name
 * \param RES resolution (number of bins)
 * \param FS fullscale value
 * \param OFFSET offset
 * 
 *************************************************************************************************/
#define ADC_SET_CONV(NAME, RES, FS, OFFSET)                                                     \
    adc->NAME->conv->cop = (struct conv_cop) {                                                  \
        .res = RES,                                                                             \
        .fs = FS                                                                                \
    };                                                                                          \
    adc->NAME->conv->hapi = (struct conv_nop) {                                                 \
        .offset = OFFSET,                                                                       \
        .gain_c = 0U                                                                            \
    }


/**************************************************************************************************
 * 
 * \brief Disables ADC variable
 * 
 * \param NAME variable name
 * 
 *************************************************************************************************/
#define ADC_VAR_IS_INVERTED(NAME)                                                                   \
    adc->NAME->conv->invert = true


/**************************************************************************************************
 * 
 * \brief Disables ADC variable
 * 
 * \param NAME variable name
 * 
 *************************************************************************************************/
#define ADC_VAR_DISABLE(NAME)                                                                   \
    NAME.enable = false

/**************************************************************************************************
 * 
 * \brief Generates ADC object structure definition
 * 
 * \param ... optional structure members
 * 
 *************************************************************************************************/
#define ADC_OBJ_STRUCT(...)                                                                     \
    OBJ_STRUCT(adc, adc_priv, __VA_ARGS__)

/**************************************************************************************************
 * 
 * \brief Adds ADC variable to ADC object definition
 * 
 * \param NAME variable name
 * 
 *************************************************************************************************/
#define ADC_OBJ_STRUCT_MEMBER(NAME)                                                             \
    OBJ_STRUCT_MEMBER(adc_var, NAME)

#endif /* _CGEN_API_ADC_H */
