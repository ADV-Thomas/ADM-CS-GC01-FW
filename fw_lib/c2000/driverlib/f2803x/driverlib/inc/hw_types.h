/**************************************************************************************************
 * 
 * \file hw_can.h
 * 
 * \brief Useful macros used in driverlib
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef HW_TYPES_H
#define HW_TYPES_H

#ifdef VIRTUAL
#define HWREG(x)        (*((volatile uint32_t *)((uintptr_t)(MEMORY + x))))
#define HWREGH(x)       (*((volatile uint16_t *)((uintptr_t)(MEMORY + x))))

#define HWREG_BP(x)     HWREG(x)
#else
#define HWREG(x)        (*((volatile uint32_t *)((uintptr_t)(x))))
#define HWREGH(x)       (*((volatile uint16_t *)((uintptr_t)(x))))

#define HWREG_BP(x)     __byte_peripheral_32((uint32_t *)(x))
#define HWREGB(x)       __byte((int16_t *)(x), 0)
#endif

#define STATUS_S_SUCCESS    (0)
#define STATUS_E_FAILURE    (-1)

#ifndef C2000_IEEE754_TYPES
#define C2000_IEEE754_TYPES
#ifdef __TI_EABI__
typedef float       float32_t;
typedef double      float64_t;
#else /* TI COFF */
typedef float       float32_t;
typedef long double float64_t;
#endif /* __TI_EABI__ */
#endif /* C2000_IEEE754_TYPES */

/**************************************************************************************************
 * 
 * Support for register manipulation defines. These should be used only within this file.
 * 
 *************************************************************************************************/

#define _HW_BIT(MOD, REG, BIT)      (MOD##_##REG##_##BIT)
#define _HW_MASK(MOD, REG, FLD)     (MOD##_##REG##_##FLD##_M)
#define _HW_SHIFT(MOD, REG, FLD)    (MOD##_##REG##_##FLD##_S)

#define _HW1_REG(REG)   HWREGH(REG)
#define _HW2_REG(REG)   HWREG_BP(REG)

#define ___HWX_REG(SIZE, REG)    _HW##SIZE##_REG(REG)
#define __HWX_REG(SIZE, REG)     ___HWX_REG(SIZE, REG)

#define _HWX_REG(BASE, OFF, MOD, REG)                                                           \
    __HWX_REG(MOD##_S_##REG, BASE + OFF + MOD##_O_##REG)

#define _HW1_UINT()     uint16_t
#define _HW2_UINT()     uint32_t

#define ___HWX_UINT(SIZE)   _HW##SIZE##_UINT()
#define __HWX_UINT(SIZE)    ___HWX_UINT(SIZE)

#define _HWX_UINT(MOD, REG)                                                                     \
    __HWX_UINT(MOD##_S_##REG)

#define _HWX_VAL(MOD, REG, VAL)                                                                 \
    ((_HWX_UINT(MOD, REG)) (VAL))

/**************************************************************************************************
 * 
 * Defines for register manipulation with base register offset
 * 
 *************************************************************************************************/

/**************************************************************************************************
 * 
 * \brief Clears bit in module register
 * 
 * \param BASE register base address
 * \param OFF offset from base address
 * \param MOD module name
 * \param REG register name
 * \param BIT bit name
 * 
 *************************************************************************************************/
#define HW_BIT_CLRO(BASE, OFF, MOD, REG, BIT)                                                   \
    _HWX_REG(BASE, OFF, MOD, REG) &= ~_HW_BIT(MOD, REG, BIT)

/**************************************************************************************************
 * 
 * \brief Sets bit in module register
 * 
 * \param BASE register base address
 * \param OFF offset from base address
 * \param MOD module name
 * \param REG register name
 * \param BIT bit name
 * 
 *************************************************************************************************/
#define HW_BIT_SETO(BASE, OFF, MOD, REG, BIT)                                                   \
    _HWX_REG(BASE, OFF, MOD, REG) |= _HW_BIT(MOD, REG, BIT)

/**************************************************************************************************
 * 
 * \brief Gets bit from module register
 * 
 * \param BASE register base address
 * \param OFF offset from base address
 * \param MOD module name
 * \param REG register name
 * \param BIT bit name
 * 
 * \return \b true when bit is set, \b false otherwise
 * 
 *************************************************************************************************/
#define HW_BIT_GETO(BASE, OFF, MOD, REG, BIT)                                                   \
    ((_HWX_REG(BASE, OFF, MOD, REG) & _HW_BIT(MOD, REG, BIT)) != 0)

/**************************************************************************************************
 * 
 * \brief Clears field in module register
 * 
 * \param BASE register base address
 * \param OFF offset from base address
 * \param MOD module name
 * \param REG register name
 * \param FLD field name
 * 
 *************************************************************************************************/
#define HW_FLD_CLRO(BASE, OFF, MOD, REG, FLD)                                                   \
    _HWX_REG(BASE, OFF, MOD, REG) &= ~_HW_MASK(MOD, REG, FLD)

/**************************************************************************************************
 * 
 * \brief Sets field in module register
 * 
 * \param BASE register base address
 * \param OFF offset from base address
 * \param MOD module name
 * \param REG register name
 * \param FLD field name
 * \param VAL field value
 * 
 *************************************************************************************************/
#define HW_FLD_SETO(BASE, OFF, MOD, REG, FLD, VAL)                                              \
    HW_FLD_CLRO(BASE, OFF, MOD, REG, FLD);                                                      \
    _HWX_REG(BASE, OFF, MOD, REG) |=                                                            \
        (_HWX_VAL(MOD, REG, VAL) << _HW_SHIFT(MOD, REG, FLD)) & _HW_MASK(MOD, REG, FLD)

/**************************************************************************************************
 * 
 * \brief Gets field from module register
 * 
 * \param BASE register base address
 * \param OFF offset from base address
 * \param MOD module name
 * \param REG register name
 * \param FLD field name
 * 
 * \return Field value
 * 
 *************************************************************************************************/
#define HW_FLD_GETO(BASE, OFF, MOD, REG, FLD)                                                   \
    ((_HWX_REG(BASE, OFF, MOD, REG) & _HW_MASK(MOD, REG, FLD)) >> _HW_SHIFT(MOD, REG, FLD))

/**************************************************************************************************
 * 
 * \brief Sets module register
 * 
 * \param BASE register base address
 * \param OFF offset from base address
 * \param MOD module name
 * \param REG register name
 * \param VAL register value
 * 
 *************************************************************************************************/
#define HW_REG_SETO(BASE, OFF, MOD, REG, VAL)                                                   \
    _HWX_REG(BASE, OFF, MOD, REG) = _HWX_VAL(MOD, REG, VAL)

/**************************************************************************************************
 * 
 * \brief Sets single bit in module register
 * 
 * \param BASE register base address
 * \param OFF offset from base address
 * \param MOD module name
 * \param REG register name
 * \param POS bit position
 * 
 *************************************************************************************************/
#define HW_REG_SET_BITO(BASE, OFF, MOD, REG, POS)                                               \
    _HWX_REG(BASE, OFF, MOD, REG) = (_HWX_VAL(MOD, REG, 1U) << (POS))

/**************************************************************************************************
 * 
 * \brief Gets module register value
 * 
 * \param BASE register base address
 * \param OFF offset from base address
 * \param MOD module name
 * \param REG register name
 * 
 * \return Register value
 * 
 *************************************************************************************************/
#define HW_REG_GETO(BASE, OFF, MOD, REG)                                                        \
    (_HWX_REG(BASE, OFF, MOD, REG))

/**************************************************************************************************
 * 
 * \brief Clears register bit at specified position
 * 
 * \param BASE register base address
 * \param OFF offset from base address
 * \param MOD module name
 * \param REG register name
 * \param POS bit position
 * 
 *************************************************************************************************/
#define HW_REG_BIT_CLRO(BASE, OFF, MOD, REG, POS)                                               \
    _HWX_REG(BASE, OFF, MOD, REG) &= ~(_HWX_VAL(MOD, REG, 1U) << (POS))

/**************************************************************************************************
 * 
 * \brief Sets register bit at specified position
 * 
 * \param BASE register base address
 * \param OFF offset from base address
 * \param MOD module name
 * \param REG register name
 * \param POS bit position
 * 
 *************************************************************************************************/
#define HW_REG_BIT_SETO(BASE, OFF, MOD, REG, POS)                                               \
    _HWX_REG(BASE, OFF, MOD, REG) |= (_HWX_VAL(MOD, REG, 1U) << (POS))

/**************************************************************************************************
 * 
 * \brief Gets register bit at specified position
 * 
 * \param BASE register base address
 * \param OFF offset from base address
 * \param MOD module name
 * \param REG register name
 * \param POS bit position
 * 
 * \return \b true when bit is set; \b false otherwise
 * 
 *************************************************************************************************/
#define HW_REG_BIT_GETO(BASE, OFF, MOD, REG, POS)                                               \
    ((_HWX_REG(BASE, OFF, MOD, REG) & (_HWX_VAL(MOD, REG, 1U) << (POS))) != 0)

/**************************************************************************************************
 * 
 * \brief Clears register bits with respect to specified mask
 * 
 * \param BASE register base address
 * \param OFF offset from base address
 * \param MOD module name
 * \param REG register name
 * \param MASK bit mask
 * 
 *************************************************************************************************/
#define HW_REG_MASK_CLRO(BASE, OFF, MOD, REG, MASK)                                             \
    _HWX_REG(BASE, OFF, MOD, REG) &= ~(MASK)

/**************************************************************************************************
 * 
 * \brief Sets register bits with respect to specified mask
 * 
 * \param BASE register base address
 * \param OFF offset from base address
 * \param MOD module name
 * \param REG register name
 * \param MASK bit mask
 * 
 *************************************************************************************************/
#define HW_REG_MASK_SETO(BASE, OFF, MOD, REG, MASK)                                             \
    _HWX_REG(BASE, OFF, MOD, REG) |= (MASK)

/**************************************************************************************************
 * 
 * Defines for register manipulation without base register offset
 * 
 *************************************************************************************************/

/**************************************************************************************************
 * 
 * \brief See \b HW_BIT_CLRO() for more information
 * 
 *************************************************************************************************/
#define HW_BIT_CLR(BASE, MOD, REG, BIT)         HW_BIT_CLRO(BASE, 0U, MOD, REG, BIT)

/**************************************************************************************************
 * 
 * \brief See \b HW_BIT_SETO() for more information
 * 
 *************************************************************************************************/
#define HW_BIT_SET(BASE, MOD, REG, BIT)         HW_BIT_SETO(BASE, 0U, MOD, REG, BIT)

/**************************************************************************************************
 * 
 * \brief See \b HW_BIT_GETO() for more information
 * 
 *************************************************************************************************/
#define HW_BIT_GET(BASE, MOD, REG, BIT)         HW_BIT_GETO(BASE, 0U, MOD, REG, BIT)

/**************************************************************************************************
 * 
 * \brief See \b HW_FLD_CLRO() for more information
 * 
 *************************************************************************************************/
#define HW_FLD_CLR(BASE, MOD, REG, FLD)         HW_FLD_CLRO(BASE, 0U, MOD, REG, FLD)

/**************************************************************************************************
 * 
 * \brief See \b HW_FLD_SETO() for more information
 * 
 *************************************************************************************************/
#define HW_FLD_SET(BASE, MOD, REG, FLD, VAL)    HW_FLD_SETO(BASE, 0U, MOD, REG, FLD, VAL)

/**************************************************************************************************
 * 
 * \brief See \b HW_FLD_GETO() for more information
 * 
 *************************************************************************************************/
#define HW_FLD_GET(BASE, MOD, REG, FLD)         HW_FLD_GETO(BASE, 0U, MOD, REG, FLD)

/**************************************************************************************************
 * 
 * \brief See \b HW_REG_SETO() for more information
 * 
 *************************************************************************************************/
#define HW_REG_SET(BASE, MOD, REG, VAL)         HW_REG_SETO(BASE, 0U, MOD, REG, VAL)

/**************************************************************************************************
 * 
 * \brief See \b HW_REG_SET_BITO() for more information
 * 
 *************************************************************************************************/
#define HW_REG_SET_BIT(BASE, MOD, REG, POS)     HW_REG_SET_BITO(BASE, 0U, MOD, REG, POS)

/**************************************************************************************************
 * 
 * \brief See \b HW_REG_GETO() for more information
 * 
 *************************************************************************************************/
#define HW_REG_GET(BASE, MOD, REG)              HW_REG_GETO(BASE, 0U, MOD, REG)

/**************************************************************************************************
 * 
 * \brief See \b HW_REG_BIT_CLRO() for more information
 * 
 *************************************************************************************************/
#define HW_REG_BIT_CLR(BASE, MOD, REG, POS)     HW_REG_BIT_CLRO(BASE, 0U, MOD, REG, POS)

/**************************************************************************************************
 * 
 * \brief See \b HW_REG_BIT_SETO() for more information
 * 
 *************************************************************************************************/
#define HW_REG_BIT_SET(BASE, MOD, REG, POS)     HW_REG_BIT_SETO(BASE, 0U, MOD, REG, POS)

/**************************************************************************************************
 * 
 * \brief See \b HW_REG_BIT_GETO() for more information
 * 
 *************************************************************************************************/
#define HW_REG_BIT_GET(BASE, MOD, REG, POS)     HW_REG_BIT_GETO(BASE, 0U, MOD, REG, POS)

/**************************************************************************************************
 * 
 * \brief See \b HW_REG_MASK_CLRO() for more information
 * 
 *************************************************************************************************/
#define HW_REG_MASK_CLR(BASE, MOD, REG, MASK)   HW_REG_MASK_CLRO(BASE, 0U, MOD, REG, MASK)

/**************************************************************************************************
 * 
 * \brief See \b HW_REG_MASK_SETO() for more information
 * 
 *************************************************************************************************/
#define HW_REG_MASK_SET(BASE, MOD, REG, MASK)   HW_REG_MASK_SETO(BASE, 0U, MOD, REG, MASK)

#endif /* HW_TYPES_H */
