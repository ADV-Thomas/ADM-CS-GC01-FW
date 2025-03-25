/**************************************************************************************************
 * 
 * \file rms.h
 * 
 * \brief RMS abstraction code generator
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _CGEN_LIB_RMS_H
#define _CGEN_LIB_RMS_H

#include "inc/api/obj.h"

/**************************************************************************************************
 * 
 * \brief Generates new static private variables structure
 * 
 * \param None
 * 
 * \details This function generates private variables structure for RMS object:
 * - variable type is 'struct rms_priv'
 * - variable name is 'priv'
 * - object structure type is 'struct rms'
 * - object type is 'OBJ_RMS'
 * 
 *************************************************************************************************/
#define RMS_PRIV_NEW()                                                                          \
    OBJ_PRIV_NEW(rms_priv, priv, rms, OBJ_RMS,                                                  \
        .init = false                                                                           \
    )

/**************************************************************************************************
 * 
 * \brief Generates new static RMS object
 * 
 * \param ... optional initializers
 * 
 * \details This function generates RMS object with its private variables structure:
 * - object structure type is 'struct rms'
 * - object name is 'rms'
 * 
 *************************************************************************************************/
#define RMS_OBJ_NEW(...)                                                                        \
    RMS_PRIV_NEW();                                                                             \
    OBJ_NEW(rms, rms, &priv, __VA_ARGS__)

/**************************************************************************************************
 * 
 * \brief Generates new RMS variable with low-pass filter and assigns ADC variable
 * 
 * \param NAME variable name
 * 
 *************************************************************************************************/
#define RMS_VAR_NEW(NAME)                                                                       \
    static struct iir NAME##_iir;                                                               \
    static struct rms_var NAME = {                                                              \
        .meas = {                                                                               \
            .norm = 0U,                                                                         \
            .phy = 0.0f                                                                         \
        },                                                                                      \
        .iir = &NAME##_iir,                                                                     \
        .adc = NULL                                                                             \
    };                                                                                          \
    NAME.adc = adc->NAME

/**************************************************************************************************
 * 
 * \brief Generates RMS object structure definition
 * 
 * \param ... optional structure members
 * 
 *************************************************************************************************/
#define RMS_OBJ_STRUCT(...)                                                                     \
    OBJ_STRUCT(rms, rms_priv, __VA_ARGS__)

/**************************************************************************************************
 * 
 * \brief Adds RMS variable to RMS object definition
 * 
 * \param NAME variable name
 * 
 *************************************************************************************************/
#define RMS_OBJ_STRUCT_MEMBER(NAME)                                                             \
    OBJ_STRUCT_MEMBER(rms_var, NAME)

#endif /* _CGEN_LIB_RMS_H */
