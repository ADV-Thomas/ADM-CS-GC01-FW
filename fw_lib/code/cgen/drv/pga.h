/**************************************************************************************************
 * 
 * \file pga.h
 * 
 * \brief PGA driver code generator
 * 
 * \author Jorge Sola
 * 
 *************************************************************************************************/

#ifndef _CGEN_DRV_PGA_H
#define _CGEN_DRV_PGA_H

#include "cgen/api/obj.h"

/**************************************************************************************************
 * 
 * \brief Generates new static private variables structure
 * 
 * \param None
 * 
 * \details This function generates private variables structure for PGA driver object:
 * - variable type is 'struct _pga_priv'
 * - variable name is 'pga_priv'
 * - object structure tpye is 'struct _pga'
 * - object type is 'OBJ__PGA'
 * 
 *************************************************************************************************/
#define _PGA_PRIV_NEW()                                                                         \
    OBJ_PRIV_NEW(_pga_priv, pga_priv, _pga, OBJ__PGA,                                           \
        .init = 0U                                                                              \
    )

/**************************************************************************************************
 * 
 * \brief Generates new static PGA driver object
 * 
 * \param ... optional initializers
 * 
 * \details This function generates PGA driver object with its private variables structure:
 * - object structure type is 'struct _pga'
 * - object name is 'pga'
 * 
 *************************************************************************************************/
#define _PGA_OBJ_NEW(...)                                                                       \
    _PGA_PRIV_NEW();                                                                            \
    OBJ_NEW(_pga, pga, &pga_priv, __VA_ARGS__)

/**************************************************************************************************
 * 
 * \brief Generates new static PGA variable with default configuration data
 * 
 * \param NAME PGA variable name
 * 
 *************************************************************************************************/
#define _PGA_VAR_NEW(NAME)                                                                      \
    static struct _pga_var NAME##_pga = {                                                       \
        .enable  = false,                                                                       \
        .io    = (enum io)0U,                                                                          \
        .gain  = 0U,                                                              \
    }

/**************************************************************************************************
 * 
 * \brief Initializes PGA variable with configuration data
 * 
 * \param NAME PGA variable name
 * \param CHANNEL Channel associated with PGA variable
 * 
 *************************************************************************************************/
#define _PGA_VAR_INIT(NAME, IO, GAIN)                         \
    *hapi->pga->NAME = (struct _pga_var) {                                                      \
        .enable  = true,                                                                        \
        .io    = IO,                                                                          \
        .gain  = GAIN,                                                                    \
    }

/**************************************************************************************************
 * 
 * \brief Disables PGA variable
 * 
 * \param NAME PGA variable name
 * 
 *************************************************************************************************/
#define _PGA_VAR_DISABLE(NAME)                                                                  \
    hapi->pga->NAME->enable = false


/**************************************************************************************************
 * 
 * \brief Generates PGA object structure definition
 * 
 * \param ... optional structure members
 * 
 *************************************************************************************************/
#define _PGA_OBJ_STRUCT(...)                                                                    \
    OBJ_STRUCT(_pga, _pga_priv, __VA_ARGS__)

/**************************************************************************************************
 * 
 * \brief Adds variable to PGA object structure definition
 * 
 * \param NAME variable name
 * 
 *************************************************************************************************/
#define _PGA_OBJ_STRUCT_MEMBER(NAME)                                                            \
    OBJ_STRUCT_MEMBER(_pga_var, NAME)

/**************************************************************************************************
 * 
 * \brief Sets structure member to variable handler
 * 
 * \param NAME variable name
 * 
 *************************************************************************************************/
#define _PGA_MEMBER_SET(NAME)                                                                   \
    .NAME = &NAME##_pga

#endif /* _CGEN_DRV_PGA_H */