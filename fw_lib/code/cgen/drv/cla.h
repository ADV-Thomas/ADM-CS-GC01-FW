/**************************************************************************************************
 * 
 * \file cla.h
 * 
 * \brief CLA driver code generator
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _CGEN_DRV_CLA_H
#define _CGEN_DRV_CLA_H

#include "cgen/api/obj.h"

/**************************************************************************************************
 * 
 * \brief Generates new static private variables structure
 * 
 * \param None
 * 
 * \details This function generates private variables structure for CLA driver object:
 * - variable type is 'struct _cla_priv'
 * - variable name is 'cla_priv'
 * - object structure tpye is 'struct _cla'
 * - object type is 'OBJ__CLA'
 * 
 *************************************************************************************************/
#define _CLA_PRIV_NEW()                                                                         \
    OBJ_PRIV_NEW(_cla_priv, cla_priv, _cla, OBJ__CLA,                                           \
        .init = 0U                                                                              \
    )

/**************************************************************************************************
 * 
 * \brief Generates new static CLA driver object
 * 
 * \param ... optional initializers
 * 
 * \details This function generates CLA driver object with its private variables structure:
 * - object structure type is 'struct _cla'
 * - object name is 'cla'
 * 
 *************************************************************************************************/
#define _CLA_OBJ_NEW(...)                                                                       \
    _CLA_PRIV_NEW();                                                                            \
    OBJ_NEW(_cla, cla, &cla_priv, __VA_ARGS__)

/**************************************************************************************************
 * 
 * \brief Generates new static CLA variable with default configuration data
 * 
 * \param NAME CLA variable name
 * 
 *************************************************************************************************/
#define _CLA_VAR_NEW(NAME)                                                                      \
    static struct _cla_var NAME##_cla = {                                                       \
        .enable  = false,                                                                       \
        .task_num= 0,                                                                           \
        .trigger = 0,                                                                           \
        .task_fun= NULL,                                                                        \
    }

/**************************************************************************************************
 * 
 * \brief Initializes CLA variable with configuration data
 * 
 * \param NAME CLA variable name
 * \param TASK_N CLA task number
 * \param TRIGGER CLA task trigger
 * \param FUNC Function callback attached to that task
 * 
 *************************************************************************************************/
#define _CLA_VAR_INIT(NAME, TASK_N, TRIGGER, FUNC)                                              \
    *hapi->cla->NAME = (struct _cla_var) {                                                      \
        .enable  = true,                                                                        \
        .task_num= TASK_N,                                                                      \
        .trigger = TRIGGER,                                                                     \
        .task_fun= FUNC,                                                                        \
    }

/**************************************************************************************************
 * 
 * \brief Disables CLA variable
 * 
 * \param NAME CLA variable name
 * 
 *************************************************************************************************/
#define _CLA_VAR_DISABLE(NAME)                                                                  \
    hapi->cla->NAME->enable = false

/**************************************************************************************************
 * 
 * \brief Generates CLA object structure definition
 * 
 * \param ... optional structure members
 * 
 *************************************************************************************************/
#define _CLA_OBJ_STRUCT(...)                                                                    \
    OBJ_STRUCT(_cla, _cla_priv, __VA_ARGS__)

/**************************************************************************************************
 * 
 * \brief Adds variable to CLA object structure definition
 * 
 * \param NAME variable name
 * 
 *************************************************************************************************/
#define _CLA_OBJ_STRUCT_MEMBER(NAME)                                                            \
    OBJ_STRUCT_MEMBER(_cla_var, NAME)

/**************************************************************************************************
 * 
 * \brief Sets structure member to variable handler
 * 
 * \param NAME variable name
 * 
 *************************************************************************************************/
#define _CLA_MEMBER_SET(NAME)                                                                   \
    .NAME = &NAME##_cla

#endif /* _CGEN_DRV_CLA_H */
