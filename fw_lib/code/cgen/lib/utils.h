/**************************************************************************************************
 * 
 * \file utils.h
 * 
 * \brief
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _CGEN_LIB_UTILS_H
#define _CGEN_LIB_UTILS_H

/**
 *      CONSTRUCTORS
 */

#define LIM_INIT(MIN, MAX)                                                                      \
    {   .min = MIN,                                                                             \
        .max = MAX                                                                              \
    }

/**
 *      CODE GENERATION
 */

#define LENGTH(AR)                                                                              \
    (sizeof(AR) / sizeof(AR[0]))

#endif /* _CGEN_LIB_UTILS_H */
