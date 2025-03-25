/**************************************************************************************************
 * 
 * \file ecap.h
 * 
 * \brief ECAP driver code generator
 * 
 * \author Jorge Sola
 * 
 *************************************************************************************************/

#ifndef _CGEN_DRV_ECAP_H
#define _CGEN_DRV_ECAP_H

#include "cgen/api/obj.h"

/**************************************************************************************************
 * 
 * \brief Generates new static private variables structure
 * 
 * \param None
 * 
 * \details This function generates private variables structure for ECAP driver object:
 * - variable type is 'struct _ecap_priv'
 * - variable name is 'ecap_priv'
 * - object structure tpye is 'struct _ecap'
 * - object type is 'OBJ__ECAP'
 * 
 *************************************************************************************************/
#define _ECAP_PRIV_NEW()                                                                         \
    OBJ_PRIV_NEW(_ecap_priv, ecap_priv, _ecap, OBJ__ECAP,                                           \
        .init = 0U                                                                              \
    )

/**************************************************************************************************
 * 
 * \brief Generates new static ECAP driver object
 * 
 * \param ... optional initializers
 * 
 * \details This function generates ECAP driver object with its private variables structure:
 * - object structure type is 'struct _ecap'
 * - object name is 'ecap'
 * 
 *************************************************************************************************/
#define _ECAP_OBJ_NEW(...)                                                                       \
    _ECAP_PRIV_NEW();                                                                            \
    OBJ_NEW(_ecap, ecap, &ecap_priv, __VA_ARGS__)

/**************************************************************************************************
 * 
 * \brief Generates new static ECAP variable with default configuration data
 * 
 * \param NAME ECAP variable name
 * 
 *************************************************************************************************/
#define _ECAP_VAR_NEW(NAME)                                                                      \
    static struct _ecap_var NAME##_ecap = {                                                       \
        .enable  = false,                                                                       \
        .core    = 0U,                                                                          \
        .io    = (enum io)0U,                                                                          \
        .base  = 0U,                                                                          \
        .div  = 0U,                                                                    \
    }

/**************************************************************************************************
 * 
 * \brief Initializes ECAP variable with configuration data
 * 
 * \param NAME ECAP variable name
 * \param CORE ECAP core number (1 is ECAPA, 2 is ECAPB, 3 is ECAPC)
 * \param IO GPIO to be used by the ECAP module
 * \param DIV Event timer pre-scaler
 * 
 *************************************************************************************************/
#define _ECAP_VAR_INIT(NAME, CORE, IO, DIV)                         \
    *hapi->ecap->NAME = (struct _ecap_var) {                                                      \
        .enable  = true,                                                                        \
        .core    = CORE,                                                                        \
        .io      = IO,                                                                          \
        .div    = DIV,                                                                          \
    }

/**************************************************************************************************
 * 
 * \brief Disables ECAP variable
 * 
 * \param NAME ECAP variable name
 * 
 *************************************************************************************************/
#define _ECAP_VAR_DISABLE(NAME)                                                                  \
    hapi->ecap->NAME->enable = false

/**************************************************************************************************
 * 
 * \brief Reads signal period (time between rising edge events after prescaling)
 * 
 * \param NAME variable name
 * 
 *************************************************************************************************/
#define _ECAP_READ_SIGNAL_PERIOD(NAME)                                                     \
    ECAP_getEventTimeStamp(hapi->ecap->NAME->base,ECAP_EVENT_1)

/**************************************************************************************************
 * 
 * \brief Reads signal width (time between rising and negative edge after prescaling)
 * 
 * \param NAME variable name
 * 
 *************************************************************************************************/
#define _ECAP_READ_SIGNAL_WIDTH(NAME)                                                     \
    ECAP_getEventTimeStamp(hapi->ecap->NAME->base,ECAP_EVENT_2)

/**************************************************************************************************
 * 
 * \brief Generates ECAP object structure definition
 * 
 * \param ... optional structure members
 * 
 *************************************************************************************************/
#define _ECAP_OBJ_STRUCT(...)                                                                    \
    OBJ_STRUCT(_ecap, _ecap_priv, __VA_ARGS__)

/**************************************************************************************************
 * 
 * \brief Adds variable to ECAP object structure definition
 * 
 * \param NAME variable name
 * 
 *************************************************************************************************/
#define _ECAP_OBJ_STRUCT_MEMBER(NAME)                                                            \
    OBJ_STRUCT_MEMBER(_ecap_var, NAME)

/**************************************************************************************************
 * 
 * \brief Sets structure member to variable handler
 * 
 * \param NAME variable name
 * 
 *************************************************************************************************/
#define _ECAP_MEMBER_SET(NAME)                                                                   \
    .NAME = &NAME##_ecap

#endif /* _CGEN_DRV_ECAP_H */
