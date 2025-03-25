/**************************************************************************************************
 * 
 * \file wch.h
 * 
 * \brief WCH abstraction code generator
 * 
 * \author Jorge Sola
 * 
 *************************************************************************************************/

#ifndef _CGEN_API_WCH_H
#define _CGEN_API_WCH_H

#include "inc/api/obj.h"

/**************************************************************************************************
 * 
 * \brief Generates new static private variables structure
 * 
 * \param TYPE corresponding object structure type
 * 
 * \details This function generates private variables structure for WCH object:
 * - variable type is 'struct wch_priv'
 * - variable name is 'priv'
 * - object structure type is 'struct wch'
 * - object type is 'OBJ_WCH'
 * 
 *************************************************************************************************/
#define WCH_PRIV_NEW()                                                                          \
    OBJ_PRIV_NEW(wch_priv, priv, wch, OBJ_WCH,                                                  \
        .init = false,                                                                           \
    )

/**************************************************************************************************
 * 
 * \brief Generates new static WCH object
 * 
 * \param ... optional initializers
 * 
 * \details This function generates WCH object with its private variables structure:
 * - object structure type is defined by \e TYPE parameter
 * - object name is 'wch'
 * 
 *************************************************************************************************/
#define WCH_OBJ_NEW(...)                                                                        \
    WCH_PRIV_NEW();                                                                             \
    OBJ_NEW(wch, wch, &priv, __VA_ARGS__)

/**************************************************************************************************
 * 
 * \brief Generates new WCH variable with low-pass filter and conversion object
 * 
 * \param NAME variable name
 * 
 *************************************************************************************************/
#define WCH_VAR_NEW(NAME)                                                                \
    CONV_OBJ_NEW(NAME##_h_conv);                                                                  \
    CONV_OBJ_NEW(NAME##_l_conv);                                                                  \
    static struct wch_var NAME = {                                                              \
        .fault = false,                                                                         \
        .cmpl = {                                                                        \
            .enable = true,                                                                     \
            .fault = false,                                                                     \
            .conv = &NAME##_l_conv                                                              \
        },                                                                                       \
        .cmph = {                                                                        \
            .enable = true,                                                                     \
            .fault = false,                                                                     \
            .conv = &NAME##_h_conv                                                              \
        },                                                                                       \
    }


/**************************************************************************************************
 * 
 * \brief Sets conversion data in WCH variable
 * 
 * \param NAME variable name
 * \param RES resolution (number of bins)
 * \param FS fullscale value
 * \param OFFSET offset
 * 
 *************************************************************************************************/
#define WCH_SET_CONV(NAME, RES, FS, OFFSET)                                                     \
    wch->NAME->cmpl.conv->cop = (struct conv_cop) {                                            \
        .res = RES,                                                                             \
        .fs = FS                                                                                \
    };                                                                                          \
    wch->NAME->cmpl.conv->hapi = (struct conv_nop) {                                           \
        .offset = OFFSET,                                                                       \
        .gain_c = 0U                                                                            \
    };                                                                                           \
    wch->NAME->cmph.conv->cop = (struct conv_cop) {                                            \
        .res = RES,                                                                             \
        .fs = FS                                                                                \
    };                                                                                          \
    wch->NAME->cmph.conv->hapi = (struct conv_nop) {                                           \
        .offset = OFFSET,                                                                       \
        .gain_c = 0U                                                                            \
    };


/**************************************************************************************************
 * 
 * \brief Generates WCH object structure definition
 * 
 * \param ... optional structure members
 * 
 *************************************************************************************************/
#define WCH_OBJ_STRUCT(...)                                                                     \
    OBJ_STRUCT(wch, wch_priv, __VA_ARGS__)

/**************************************************************************************************
 * 
 * \brief Adds WCH variable to WCH object definition
 * 
 * \param NAME variable name
 * 
 *************************************************************************************************/
#define WCH_OBJ_STRUCT_MEMBER(NAME)                                                             \
    OBJ_STRUCT_MEMBER(wch_var, NAME)

#endif /* _CGEN_API_WCH_H */
