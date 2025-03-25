/**************************************************************************************************
 * 
 * \file utils.h
 * 
 * \brief Collection of commonly used constants and small functions
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_LIB_UTILS_H
#define _INC_LIB_UTILS_H

#include <stdint.h>

#include "IQmathLib.h"

/**************************************************************************************************
 * 
 * Defines for commonly used mathematical constants
 * 
 *************************************************************************************************/

#define C_1BY2          (0.50000000000000000000000000000000f)
#define C_1BY3          (0.33333333333333333333333333333333f)
#define C_2BY3          (0.66666666666666666666666666666667f)

#define C_SQRT2         (1.41421356237309504880168872420970f)
#define C_SQRT2BY2      (0.70710678118654752440084436210485f)
#define C_SQRT3         (1.73205080756887729352744634150590f)
#define C_SQRT3BY2      (0.86602540378443864676372317075294f)
#define C_SQRT3BY3      (0.57735026918962576450914878050196f)
#define C_SQRT6BY6      (0.40824829046386301636621401245098f)
#define C_SQRT2BYSQRT3  (0.81649658092772603273242802490196f)

#define C_PI            (3.14159265358979323846264338327950f)
#define C_PIBY4         (0.78539816339744830961566084581988f)
#define C_PIBY2         (1.57079632679489661923132169163980f)
#define C_3PIBY2        (4.71238898038468985769396507491930f)
#define C_2PI           (6.28318530717958647692528676655900f)

/**************************************************************************************************
 * 
 * \brief Returns number of elements in array
 * 
 * \param AR array name
 * 
 * \return number of elements in array
 * 
 *************************************************************************************************/
#define LENGTH(AR)                                                                              \
    (sizeof(AR) / sizeof(AR[0]))


/**************************************************************************************************
 * 
 * \brief Saturates float number between min and max
 * 
 *************************************************************************************************/
#define CLAMP_MACRO(x,min,max)  \
         x = x > max ? max : x;     \
         x = x < min ? min : x;

/**************************************************************************************************
 * 
 * \brief Unsigned 16-bit division with round to nearest integer
 * 
 * \param num numerator
 * \param den denominator
 * 
 * \return 16-bit unsigned result rounded to nearest integer; largest unsigned 16-bit integer
 * if denominator is 0
 * 
 *************************************************************************************************/
static inline uint16_t
div_round_u16(uint16_t num, uint16_t den)
{
    return ((den != 0U) ? ((num + (den >> 1)) / den) : 0xFFFFU);
}

/**************************************************************************************************
 * 
 * \brief Unsigned 32-bit division with round to nearest integer
 * 
 * \param num numerator
 * \param den denominator
 * 
 * \return 32-bit unsigned result rounded to nearest integer; largest unsigned 32-bit integer
 * if denominator is 0
 * 
 *************************************************************************************************/
static inline uint32_t
div_round_u32(uint32_t num, uint32_t den)
{
    return ((den != 0U) ? ((num + (den >> 1)) / den) : 0xFFFFFFFFU);
}

/**************************************************************************************************
 * 
 * \brief Signed 32-bit floating-point division with round to nearest integer
 * 
 * \param num numerator
 * \param den denominator
 * 
 * \return 32-bit signed result rounded to nearest integer
 * 
 *************************************************************************************************/
static inline int32_t
div_round_fp32(float num, float den)
{
    return (uint32_t) ((num / den) + 0.5f);
}

/**************************************************************************************************
 * 
 * \brief Unsigned 32-bit division with round-up
 * 
 * \param num numerator
 * \param den denominator
 * 
 * \return 32-bit unsigned result rounded up (ceil) to larger integer; largest unsigned 32-bit
 * integer if denominator is 0
 * 
 *************************************************************************************************/
static inline uint32_t
div_ceil_u32(uint32_t num, uint32_t den)
{
    return ((den != 0U) ? ((num + (den - 1)) / den) : 0xFFFFFFFFU);
}

/**************************************************************************************************
 * 
 * \brief Unsigned 32-bit division with round-down
 * 
 * \param num numerator
 * \param den denominator
 * 
 * \return 32-bit unsigned result rounded down (floor) to smaller integer; largest unsigned 32-bit
 * integer if denominator is 0
 * 
 *************************************************************************************************/
static inline uint32_t
div_floor_u32(uint32_t num, uint32_t den)
{
    return ((den != 0U) ? (num / den) : 0xFFFFFFFFU);
}

/**************************************************************************************************
 * 
 * \brief Compares fixed-point variable to its floating-point source
 * 
 * \param iq fixed-point variable
 * \param fp floating-point variable
 * \param pp allowed error as percentage in range 0..1
 * 
 * \return 0 if overflow did not occur; -1 otherwise
 * 
 *************************************************************************************************/
static int
iq_verify(_iq iq, float fp, float pp)
{
    float e = _IQtoF(iq) - fp;
    if (e < 0.0f) {
        e = -e;
    }

    return (e <= (pp * fp)) ? 0 : -1;
}

__attribute__((ramfunc)) static inline _iq iq_sat(_iq value, _iq min, _iq max)
{
    _iq t = value < min ? min : value;
    return t > max ? max : t;
}

__attribute__((ramfunc)) static inline _iq sign(_iq value)
{
    _iq sgn = value > _IQ(0.0) ? _IQ(1.0) : value < _IQ(0.0) ? _IQ(-1.0) : _IQ(0.0);
    return sgn;
}

#endif /* _INC_LIB_UTILS_H */
