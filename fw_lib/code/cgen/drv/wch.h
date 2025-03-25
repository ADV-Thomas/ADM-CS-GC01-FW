/**************************************************************************************************
 * 
 * \file wch.h
 * 
 * \brief Window Hardware Comparator driver code generator
 * 
 * \author Jorge Sola
 * 
 *************************************************************************************************/

#ifndef _CGEN_DRV_WCH_H
#define _CGEN_DRV_WCH_H

#include "cgen/api/obj.h"

/**************************************************************************************************
 * 
 * \brief Generates new static private variables structure
 * 
 * \param None
 * 
 * \details This function generates private variables structure for WCH driver object:
 * - variable type is 'struct _wch_priv'
 * - variable name is 'wch_priv'
 * - object structure tpye is 'struct _wch'
 * - object type is 'OBJ__WCH'
 * 
 *************************************************************************************************/
#define _WCH_PRIV_NEW()                                                                         \
    OBJ_PRIV_NEW(_wch_priv, wch_priv, _wch, OBJ__WCH,                                           \
        .init = 0U                                                                              \
    )

/**************************************************************************************************
 * 
 * \brief Generates new static WCH driver object
 * 
 * \param ... optional initializers
 * 
 * \details This function generates WCH driver object with its private variables structure:
 * - object structure type is 'struct _wch'
 * - object name is 'wch'
 * 
 *************************************************************************************************/
#define _WCH_OBJ_NEW(...)                                                                       \
    _WCH_PRIV_NEW();                                                                            \
    OBJ_NEW(_wch, wch, &wch_priv, __VA_ARGS__)

/**************************************************************************************************
 * 
 * \brief Generates new static WCH variable with default configuration data
 * 
 * \param NAME WCH variable name
 * 
 *************************************************************************************************/
#define _WCH_VAR_NEW(NAME)                                                                      \
    static struct _wch_var NAME##_wch = {                                                       \
        .enable  = false,                                                                       \
        .core    = 0U,                                                                          \
        .base    = 0U,                                                                          \
        .adc_core = 0U,                                                                         \
        .adc_channel = 0U,                                                                    \
        .xbar_trip_h = 0U,                                                                       \
        .xbar_trip_l = 0U,                                                                       \
        .pwm_sync = 0U,                                                                         \
    }

/**************************************************************************************************
 * 
 * \brief Initializes WCH variable with configuration data
 * 
 * \param NAME WCH variable name
 * \param CORE ADC core to which the comparator is connected (1 is ADCA, 2 is ADCB, 3 is ADCC)
 * \param CHANNEL ADC channel to which the comparator is connected
 * \param PWM_SYNC PWM number connected to the comparator module (to reset its current ramp)
 * \param XBAR_TRIP_HIGH XBAR_TRIP to which the comparator HIGH output is connected
 * \param XBAR_TRIP_LOW XBAR_TRIP to which the comparator LOW output is connected
 * 
 *************************************************************************************************/
#define _WCH_VAR_INIT(NAME, CORE, CHANNEL, PWM_SYNC, XBAR_TRIP_HIGH, XBAR_TRIP_LOW)             \
    *hapi->wch->NAME = (struct _wch_var) {                                                      \
        .enable  = true,                                                                        \
        .core    = 0U,                                                                          \
        .base    = 0U,                                                                          \
        .adc_core = CORE,                                                                       \
        .adc_channel = CHANNEL,                                                                 \
        .pwm_sync = PWM_SYNC,                                                                   \
        .xbar_trip_h = XBAR_TRIP_HIGH,                                                          \
        .xbar_trip_l = XBAR_TRIP_LOW,                                                           \
    }

/**************************************************************************************************
 * 
 * \brief Disables WCH variable
 * 
 * \param NAME WCH variable name
 * 
 *************************************************************************************************/
#define _WCH_VAR_DISABLE(NAME)                                                                  \
    hapi->wch->NAME->enable = false


/**************************************************************************************************
 * 
 * \brief Generates WCH object structure definition
 * 
 * \param ... optional structure members
 * 
 *************************************************************************************************/
#define _WCH_OBJ_STRUCT(...)                                                                    \
    OBJ_STRUCT(_wch, _wch_priv, __VA_ARGS__)

/**************************************************************************************************
 * 
 * \brief Adds variable to WCH object structure definition
 * 
 * \param NAME variable name
 * 
 *************************************************************************************************/
#define _WCH_OBJ_STRUCT_MEMBER(NAME)                                                            \
    OBJ_STRUCT_MEMBER(_wch_var, NAME)

/**************************************************************************************************
 * 
 * \brief Sets structure member to variable handler
 * 
 * \param NAME variable name
 * 
 *************************************************************************************************/
#define _WCH_MEMBER_SET(NAME)                                                                   \
    .NAME = &NAME##_wch

/**************************************************************************************************
 * 
 * \brief Reads WCH status
 * 
 * \param NAME variable name
 * 
 *************************************************************************************************/
#define _WCH_GET_STATUS(NAME)                                                                    \
    wch->NAME->cmpl.fault = (bool) ((CMPSS_getStatus(hapi->wch->NAME->base) & 0x0200) != 0);\
    wch->NAME->cmph.fault = (bool) ((CMPSS_getStatus(hapi->wch->NAME->base) & 0x0002) != 0); \
    wch->NAME->fault = (bool) (wch->NAME->cmpl.fault || wch->NAME->cmph.fault)


/**************************************************************************************************
 * 
 * \brief Sets WCH thresholds
 * 
 * \param NAME variable name
 * 
 *************************************************************************************************/
#define _WCH_SET(NAME)                                                                    \
    CMPSS_setDACValueLow(hapi->wch->NAME->base, wch->NAME->cmpl.ref.raw);                 \
    CMPSS_setDACValueHigh(hapi->wch->NAME->base, wch->NAME->cmph.ref.raw);                \
    CMPSS_setMaxRampValue(hapi->wch->NAME->base, (wch->NAME->cmph.ref.raw << 4))       
    


/**************************************************************************************************
 * 
 * \brief Clears WCH flag
 * 
 * \param NAME variable name
 * 
 *************************************************************************************************/
#define _WCH_CLEAR(NAME)                                                                    \
    CMPSS_clearFilterLatchLow(hapi->wch->NAME->base);                                       \
    CMPSS_clearFilterLatchHigh(hapi->wch->NAME->base);                                      \
    wch->NAME->cmpl.fault = false;                                                          \
    wch->NAME->cmph.fault = false;                                                          \
    wch->NAME->fault = false;                                                                             

#endif /* _CGEN_DRV_WCH_H */
