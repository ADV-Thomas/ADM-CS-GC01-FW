/**************************************************************************************************
 * 
 * \file wcs.h
 * 
 * \brief WCS abstraction code generator
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _CGEN_API_WCS_H
#define _CGEN_API_WCS_H

#include "inc/api/obj.h"

/**************************************************************************************************
 * 
 * \brief Generates new static private variables structure
 * 
 * \param TYPE corresponding object structure type
 * 
 * \details This function generates private variables structure for WCS object:
 * - variable type is 'struct wcs_priv'
 * - variable name is 'priv'
 * - object structure type is 'struct wcs'
 * - object type is 'OBJ_WCS'
 * 
 *************************************************************************************************/
#define WCS_PRIV_NEW()                                                                          \
    OBJ_PRIV_NEW(wcs_priv, priv, wcs, OBJ_WCS,                                                  \
        .init = false                                                                           \
    )

/**************************************************************************************************
 * 
 * \brief Generates new static WCS object
 * 
 * \param ... optional initializers
 * 
 * \details This function generates WCS object with its private variables structure:
 * - object structure type is defined by \e TYPE parameter
 * - object name is 'wcs'
 * 
 *************************************************************************************************/
#define WCS_OBJ_NEW(...)                                                                        \
    WCS_PRIV_NEW();                                                                             \
    OBJ_NEW(wcs, wcs, &priv, __VA_ARGS__)

/**************************************************************************************************
 * 
 * \brief Initializes comparator structure
 * 
 * \param ENABLE enable flag
 * \param INVERT invert flag
 * 
 *************************************************************************************************/
#define WCS_CMP_INIT(ENABLE, INVERT)                                                            \
    {                                                                                           \
        .enable = ENABLE,                                                                       \
        .invert = INVERT,                                                                       \
        .fault = false,                                                                         \
        .sys = {                                                                                \
            .norm = _IQ(0.L),                                                                   \
            .phy = 0.0f                                                                         \
        },                                                                                      \
        .usr = {                                                                                \
            .norm = _IQ(0.L),                                                                   \
            .phy = 0.0f                                                                         \
        },                                                                                      \
        .ref = NULL                                                                             \
    }

/**************************************************************************************************
 * 
 * \brief Generates new WCS variable
 * 
 * \param NAME variable name
 * \param ENABLE_L enable flag for low comparator
 * \param INVERT_L invert flag for low comparator
 * \param ENABLE_H enable flag for high comparator
 * \param INVERT_H invert flag for high comparator
 * 
 *************************************************************************************************/
#define WCS_VAR_NEW(NAME, ENABLE_L, INVERT_L, ENABLE_H, INVERT_H)                               \
    static struct wcs_var NAME = {                                                              \
        .meas = NULL,                                                                           \
        .conv = NULL,                                                                           \
        .cmpl = WCS_CMP_INIT(ENABLE_L, INVERT_L),                                               \
        .cmph = WCS_CMP_INIT(ENABLE_H, INVERT_H),                                               \
        .fault = false                                                                          \
    };                                                                                          \
    NAME.meas = &adc->NAME->meas.flt;                                                           \
    NAME.conv = adc->NAME->conv;

/**************************************************************************************************
 * 
 * \brief Generates new WCS variable for RMS measurements
 * 
 * \param NAME variable name
 * \param ENABLE_L enable flag for low comparator
 * \param INVERT_L invert flag for low comparator
 * \param ENABLE_H enable flag for high comparator
 * \param INVERT_H invert flag for high comparator
 * 
 *************************************************************************************************/
#define WCS_VAR_RMS_NEW(NAME, ENABLE_L, INVERT_L, ENABLE_H, INVERT_H)                               \
    static struct wcs_var NAME##_rms = {                                                              \
        .meas = NULL,                                                                           \
        .conv = NULL,                                                                           \
        .cmpl = WCS_CMP_INIT(ENABLE_L, INVERT_L),                                               \
        .cmph = WCS_CMP_INIT(ENABLE_H, INVERT_H),                                               \
        .fault = false                                                                          \
    };                                                                                          \
    NAME##_rms.meas = &rms->NAME->meas.norm;                                                           \
    NAME##_rms.conv = rms->NAME->adc->conv;


/**************************************************************************************************
 * 
 * \brief Generates WCS object structure definition
 * 
 * \param ... optional structure members
 * 
 *************************************************************************************************/
#define WCS_OBJ_STRUCT(...)                                                                     \
    OBJ_STRUCT(wcs, wcs_priv, __VA_ARGS__)

/**************************************************************************************************
 * 
 * \brief Adds WCS variable to WCS object definition
 * 
 * \param NAME variable name
 * 
 *************************************************************************************************/
#define WCS_OBJ_STRUCT_MEMBER(NAME)                                                             \
    OBJ_STRUCT_MEMBER(wcs_var, NAME)


/**************************************************************************************************
 * 
 * \brief Sets conversion data in WCS variable
 * 
 * \param NAME variable name
 * \param RES resolution (number of bins)
 * \param FS fullscale value
 * \param OFFSET offset
 * 
 *************************************************************************************************/
#define WCS_SET_CONV(NAME, RES, FS, OFFSET)                                                     \
    wcs->NAME->conv->cop = (struct conv_cop) {                                            \
        .res = RES,                                                                             \
        .fs = FS                                                                                \
    };                                                                                          \
    wcs->NAME->conv->hapi = (struct conv_nop) {                                           \
        .offset = OFFSET,                                                                       \
        .gain_c = 0U                                                                            \
    };

/**************************************************************************************************
 * 
 * \brief Sets comparator system limits in WCS variable
 * 
 * \param NAME variable name
 * \param REF_LOW system reference for low comparator
 * \param REF_HIGH system reference for high comparator
 * 
 *************************************************************************************************/
#define WCS_SET_LIMITS(NAME, REF_LOW, REF_HIGH)                                                     \
    wcs->NAME->cmpl.sys = (struct wcs_ref) {                                            \
        .norm = 0,                                                                             \
        .phy = REF_LOW                                                                                \
    };                                                                                          \
    wcs->NAME->cmph.sys = (struct wcs_ref) {                                            \
        .norm = 0,                                                                             \
        .phy = REF_HIGH                                                                                \
    };


/**************************************************************************************************
 * 
 * \brief Sets conversion data in WCS variable
 * 
 * \param NAME variable name
 * \param RES resolution (number of bins)
 * \param FS fullscale value
 * \param OFFSET offset
 * 
 *************************************************************************************************/
#define WCS_SET_NPT(NAME, OBJ, N2P, P2N)                                                        \
    CONV_SET_NPT(NAME##_conv, OBJ, N2P, P2N)

#endif /*_CGEN_API_WCS_H */
