/**************************************************************************************************
 * 
 * \file adc.h
 * 
 * \brief ADC driver code generator
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _CGEN_DRV_ADC_H
#define _CGEN_DRV_ADC_H

#include "cgen/api/obj.h"

/**************************************************************************************************
 * 
 * \brief Generates new static private variables structure
 * 
 * \param None
 * 
 * \details This function generates private variables structure for ADC driver object:
 * - variable type is 'struct _adc_priv'
 * - variable name is 'adc_priv'
 * - object structure tpye is 'struct _adc'
 * - object type is 'OBJ__ADC'
 * 
 *************************************************************************************************/
#define _ADC_PRIV_NEW()                                                                         \
    OBJ_PRIV_NEW(_adc_priv, adc_priv, _adc, OBJ__ADC,                                           \
        .init = 0U                                                                              \
    )

/**************************************************************************************************
 * 
 * \brief Generates new static ADC driver object
 * 
 * \param ... optional initializers
 * 
 * \details This function generates ADC driver object with its private variables structure:
 * - object structure type is 'struct _adc'
 * - object name is 'adc'
 * 
 *************************************************************************************************/
#define _ADC_OBJ_NEW(...)                                                                       \
    _ADC_PRIV_NEW();                                                                            \
    OBJ_NEW(_adc, adc, &adc_priv, __VA_ARGS__)

/**************************************************************************************************
 * 
 * \brief Generates new static ADC variable with default configuration data
 * 
 * \param NAME ADC variable name
 * 
 *************************************************************************************************/
#define _ADC_VAR_NEW(NAME)                                                                      \
    static struct _adc_var NAME##_adc = {                                                       \
        .enable  = false,                                                                       \
        .core    = 0U,                                                                          \
        .base    = 0U,                                                                          \
        .result  = 0U,                                                                          \
        .channel = ADC_CH_ADCIN0,                                                               \
        .trigger = ADC_TRIGGER_SW_ONLY,                                                         \
        .sample  = 0U,                                                                          \
        .int_n   = 0U,                                                                          \
        .count   = 0U,                                                                          \
        .soc     = NULL                                                                         \
    }

/**************************************************************************************************
 * 
 * \brief Initializes ADC variable with configuration data
 * 
 * \param NAME ADC variable name
 * \param CORE ADC core number (1 is ADCA, 2 is ADCB, 3 is ADCC)
 * \param CHANNEL Channel associated with ADC variable
 * \param TRIGGER Start of conversion trigger
 * \param SAMPLE Sampling time (ns)
 * \param INT_N Interrupt line number
 * \param ... SOC wrappers associated with ADC channel
 * 
 *************************************************************************************************/
#define _ADC_VAR_INIT(NAME, CORE, CHANNEL, TRIGGER, SAMPLE, INT_N, ...)                         \
    static const uint16_t NAME##_soc[] = { __VA_ARGS__ };                                       \
    *hapi->adc->NAME = (struct _adc_var) {                                                      \
        .enable  = true,                                                                        \
        .core    = CORE,                                                                        \
        .base    = 0U,                                                                          \
        .result  = 0U,                                                                          \
        .channel = CHANNEL,                                                                     \
        .trigger = TRIGGER,                                                                     \
        .sample  = SAMPLE,                                                                      \
        .int_n   = INT_N,                                                                       \
        .count   = sizeof(NAME##_soc) / sizeof(NAME##_soc[0]),                                  \
        .soc     = NAME##_soc                                                                   \
    }

/**************************************************************************************************
 * 
 * \brief Disables ADC variable
 * 
 * \param NAME ADC variable name
 * 
 *************************************************************************************************/
#define _ADC_VAR_DISABLE(NAME)                                                                  \
    hapi->adc->NAME->enable = false

/**************************************************************************************************
 * 
 * \brief Reads single ADC measurement
 * 
 * \param NAME variable name
 * \param SOC index in SOC list
 * 
 *************************************************************************************************/
#define _ADC_READ(NAME, SOC)                                                                    \
    ADC_readResult(hapi->adc->NAME->result, (ADC_SOCNumber)(hapi->adc->NAME->soc[SOC] & 0x00FF))

/**************************************************************************************************
 * 
 * \brief Reads measurement from 1 SOC wrapper
 * 
 * \param NAME variable name
 * 
 *************************************************************************************************/
#define _ADC_READ_1(NAME)                                                                       \
    adc->NAME->meas.raw = _ADC_READ(NAME, 0U)

/**************************************************************************************************
 * 
 * \brief Reads measurement from 2 SOC wrapper
 * 
 * \param NAME variable name
 * 
 *************************************************************************************************/
#define _ADC_READ_2(NAME)                                                                       \
    adc->NAME->meas.raw = ((_ADC_READ(NAME, 0U) + _ADC_READ(NAME, 1U)) >> 1)

/**************************************************************************************************
 * 
 * \brief Reads measurement from 4 SOC wrappers
 * 
 * \param NAME variable name
 * 
 *************************************************************************************************/
#define _ADC_READ_4(NAME)                                                                       \
    adc->NAME->meas.raw = ((_ADC_READ(NAME, 0U) + _ADC_READ(NAME, 1U) +                         \
        _ADC_READ(NAME, 2U) + _ADC_READ(NAME, 3U)) >> 2)

/**************************************************************************************************
 * 
 * \brief Reads measurement from 8 SOC wrappers
 * 
 * \param NAME variable name
 * 
 *************************************************************************************************/
#define _ADC_READ_8(NAME)                                                                       \
    uint16_t NAME##_var_1 = ((_ADC_READ(NAME, 0U) + _ADC_READ(NAME, 1U) +                       \
        _ADC_READ(NAME, 2U) + _ADC_READ(NAME, 3U)) >> 2);                                       \
    uint16_t NAME##_var_2 = ((_ADC_READ(NAME, 4U) + _ADC_READ(NAME, 5U) +                       \
        _ADC_READ(NAME, 6U) + _ADC_READ(NAME, 7U)) >> 2);                                       \
    adc->NAME->meas.raw = ( NAME##_var_1 + NAME##_var_2 ) >> 1

/**************************************************************************************************
 * 
 * \brief Generates ADC object structure definition
 * 
 * \param ... optional structure members
 * 
 *************************************************************************************************/
#define _ADC_OBJ_STRUCT(...)                                                                    \
    OBJ_STRUCT(_adc, _adc_priv, __VA_ARGS__)

/**************************************************************************************************
 * 
 * \brief Adds variable to ADC object structure definition
 * 
 * \param NAME variable name
 * 
 *************************************************************************************************/
#define _ADC_OBJ_STRUCT_MEMBER(NAME)                                                            \
    OBJ_STRUCT_MEMBER(_adc_var, NAME)

/**************************************************************************************************
 * 
 * \brief Sets structure member to variable handler
 * 
 * \param NAME variable name
 * 
 *************************************************************************************************/
#define _ADC_MEMBER_SET(NAME)                                                                   \
    .NAME = &NAME##_adc

#endif /* _CGEN_DRV_ADC_H */
