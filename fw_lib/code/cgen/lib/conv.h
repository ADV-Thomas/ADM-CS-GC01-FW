/**************************************************************************************************
 * 
 * \file conv.h
 * 
 * \brief Measurements conversion code generator
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _CGEN_LIB_CONV_H
#define _CGEN_LIB_CONV_H

#include <stddef.h>

#include "IQmathLib.h"

/**************************************************************************************************
 * 
 * \brief Generates new conversion object
 * 
 * \param NAME conversion object name
 * 
 *************************************************************************************************/
#define CONV_OBJ_NEW(NAME)                                                                      \
    static struct conv NAME = {                                                                 \
        .cop = {                                                                                \
            .res = 0U,                                                                          \
            .fs = 0.0f                                                                          \
        },                                                                                      \
        .hapi = {                                                                               \
            .offset = 0U,                                                                       \
            .gain_c = 0U                                                                        \
        },                                                                                      \
        .mem = {                                                                                \
            .offset = 0U,                                                                       \
            .gain_c = 0U                                                                        \
        },                                                                                      \
        .usr = {                                                                                \
            .offset = 0U,                                                                       \
            .gain_c = 0U                                                                        \
        },                                                                                      \
        .rnt = {                                                                                \
            .src = CONV_NONE,                                                                   \
            .offset = 0U,                                                                       \
            .gain = 0U                                                                          \
        },                                                                                      \
        .npt = {                                                                                \
            .obj = NULL,                                                                        \
            .n2p = NULL,                                                                        \
            .p2n = NULL                                                                         \
        },                                                                                      \
        .calib = {                                                                              \
            .offset = 0U,                                                                       \
            .gain_c = 0U                                                                         \
        },                                                                                      \
        .invert = false                                                                         \
    }

/**************************************************************************************************
 * 
 * \brief Sets custom normalized-physical transformation in conversion object
 * 
 * \param NAME conversion object name
 * \param OBJ transformation object
 * \param N2P normalized to physical function handler
 * \param P2N physical to normalized function handler
 * 
 *************************************************************************************************/
#define CONV_SET_NPT(NAME, OBJ, N2P, P2N)                                                       \
    NAME.npt = (struct conv_npt) {                                                              \
        .obj = (const void *) OBJ,                                                              \
        .n2p = (float (*)(const void *, _iq)) N2P,                                              \
        .p2n = (_iq (*)(const void *, float)) P2N                                               \
    }

#endif /* _CGEN_LIB_CONV_H */
