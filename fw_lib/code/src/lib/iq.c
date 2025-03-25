/**************************************************************************************************
 * 
 * \file iq.c
 * 
 * \brief IQmathLib missing functions implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_LIB_IQMATH_H
#define _INC_LIB_IQMATH_H

#include <stdint.h>

/**************************************************************************************************
 * 
 * \brief Multiplies two fixed-point variables
 * 
 * \param A fixed-point variable
 * \param B fixed-point variable
 * \param N point position
 * 
 * \return Multiplication result in fixed-point
 * 
 *************************************************************************************************/
long
__IQmpy(long A, long B, int N)
{
    return (long) (((int64_t) A * B) >> N);
}

/**************************************************************************************************
 * 
 * \brief Converts fixed-point to floating-point variable
 * 
 * \param A fixed-point variable
 * 
 * \return Floating-point variable
 * 
 *************************************************************************************************/
float
_IQ24toF(long A)
{
    return (float) ((double) A / 16777216.0L);
}

#endif /* _INC_LIB_IQMATH_H */
