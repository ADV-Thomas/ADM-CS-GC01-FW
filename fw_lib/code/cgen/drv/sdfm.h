/**************************************************************************************************
 * 
 * \file sdfm.h
 * 
 * \brief SDFM driver code generator
 * 
 * \author Jorge Sola
 * 
 *************************************************************************************************/

#ifndef _CGEN_DRV_SDFM_H
#define _CGEN_DRV_SDFM_H

#include "cgen/api/obj.h"

/**************************************************************************************************
 * 
 * \brief Generates new static private variables structure
 * 
 * \param None
 * 
 * \details This function generates private variables structure for SDFM driver object:
 * - variable type is 'struct _sdfm_priv'
 * - variable name is 'sdfm_priv'
 * - object structure tpye is 'struct _sdfm'
 * - object type is 'OBJ__SDFM'
 * 
 *************************************************************************************************/
#define _SDFM_PRIV_NEW()                                                                         \
    OBJ_PRIV_NEW(_sdfm_priv, sdfm_priv, _sdfm, OBJ__SDFM,                                           \
        .init = 0U                                                                              \
    )

/**************************************************************************************************
 * 
 * \brief Generates new static SDFM driver object
 * 
 * \param ... optional initializers
 * 
 * \details This function generates SDFM driver object with its private variables structure:
 * - object structure type is 'struct _sdfm'
 * - object name is 'sdfm'
 * 
 *************************************************************************************************/
#define _SDFM_OBJ_NEW(...)                                                                       \
    _SDFM_PRIV_NEW();                                                                            \
    OBJ_NEW(_sdfm, sdfm, &sdfm_priv, __VA_ARGS__)

/**************************************************************************************************
 * 
 * \brief Generates new static SDFM variable with default configuration data
 * 
 * \param NAME SDFM variable name
 * 
 *************************************************************************************************/
#define _SDFM_VAR_NEW(NAME)                                                                      \
    static struct _sdfm_var NAME##_sdfm = {                                                       \
        .enable  = false,                                                                       \
        .channel    = 0U,                                                                          \
        .io    = (enum io)0U,                                                                          \
        .io_clk = (enum io)0U,                                                                     \
        .oversampling  = 0U,                                                      \
        .shift  = 0U,                                                                       \
        .clk_mode = 0U,                                                                          \
    }

/**************************************************************************************************
 * 
 * \brief Initializes SDFM variable with configuration data
 * 
 * \param NAME SDFM variable name
 * \param CHANNEL Channel associated with SDFM variable
 * 
 *************************************************************************************************/
#define _SDFM_VAR_INIT(NAME, IO, IO_CLK, OVERSAMPLING, CLK_MODE)                         \
    *hapi->sdfm->NAME = (struct _sdfm_var) {                                                      \
        .enable  = true,                                                                        \
        .io    = IO,                                                                          \
        .io_clk = IO_CLK,                                                                      \
        .oversampling  = OVERSAMPLING,                                                         \
        .clk_mode   =   CLK_MODE,                                                               \
    }

/**************************************************************************************************
 * 
 * \brief Disables SDFM variable
 * 
 * \param NAME SDFM variable name
 * 
 *************************************************************************************************/
#define _SDFM_VAR_DISABLE(NAME)                                                                  \
    hapi->sdfm->NAME->enable = false

/**************************************************************************************************
 * 
 * \brief Reads single SDFM measurement
 * 
 * \param NAME variable name
 * 
 *************************************************************************************************/
#define _SDFM_READ(NAME)                                                                    \
    adc->NAME->meas.raw = (int16_t)(SDFM_getFilterData(SDFM1_BASE, (SDFM_FilterNumber)hapi->sdfm->NAME->channel) >> 16U)


/**************************************************************************************************
 * 
 * \brief Generates SDFM object structure definition
 * 
 * \param ... optional structure members
 * 
 *************************************************************************************************/
#define _SDFM_OBJ_STRUCT(...)                                                                    \
    OBJ_STRUCT(_sdfm, _sdfm_priv, __VA_ARGS__)

/**************************************************************************************************
 * 
 * \brief Adds variable to SDFM object structure definition
 * 
 * \param NAME variable name
 * 
 *************************************************************************************************/
#define _SDFM_OBJ_STRUCT_MEMBER(NAME)                                                            \
    OBJ_STRUCT_MEMBER(_sdfm_var, NAME)

/**************************************************************************************************
 * 
 * \brief Sets structure member to variable handler
 * 
 * \param NAME variable name
 * 
 *************************************************************************************************/
#define _SDFM_MEMBER_SET(NAME)                                                                   \
    .NAME = &NAME##_sdfm

#endif /* _CGEN_DRV_SDFM_H */
