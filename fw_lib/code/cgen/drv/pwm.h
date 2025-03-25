/**************************************************************************************************
 * 
 * \file pwm.h
 * 
 * \brief PWM driver code generator
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _CGEN_DRV_PWM_H
#define _CGEN_DRV_PWM_H

#include "cgen/api/obj.h"

/**************************************************************************************************
 * 
 * \brief Generates new static private variables structure
 * 
 * \param None
 * 
 * \details This function generates private variables structure for PWM driver object:
 * - variable type is 'struct _pwm_priv'
 * - variable name is 'pwm_priv'
 * - object structure type is 'struct _pwm'
 * - object type is 'OBJ__PWM'
 * 
 *************************************************************************************************/
#define _PWM_PRIV_NEW()                                                                         \
    OBJ_PRIV_NEW(_pwm_priv, pwm_priv, _pwm, OBJ__PWM,                                           \
        .init = 0U                                                                              \
    )

/**************************************************************************************************
 * 
 * \brief Generates new static PWM driver object
 * 
 * \param ... optional initializers
 * 
 * \details This function generates PWM driver object with its private variables structure:
 * - object structure type is 'struct _pwm'
 * - object name is 'pwm'
 * 
 *************************************************************************************************/
#define _PWM_OBJ_NEW(...)                                                                       \
    _PWM_PRIV_NEW();                                                                            \
    OBJ_NEW(_pwm, pwm, &pwm_priv, __VA_ARGS__)

/**************************************************************************************************
 * 
 * \brief Generates new static PWM variable with default configuration data
 * 
 * \param NAME PWM variable name
 * 
 *************************************************************************************************/
#define _PWM_VAR_NEW(NAME)                                                                      \
    static struct _pwm_var NAME##_pwm = {                                                       \
        .enable  = false,                                                                       \
        .num     = 0U,                                                                          \
        .base    = 0U,                                                                          \
        .io_A    = IOX,                                                                         \
        .io_B    = IOX,                                                                         \
        .out_B_e = false,                                                                       \
        .freq    = 0U,                                                                          \
        .period  = 0U,                                                                          \
        .phase_e = false,                                                                       \
        .phase_0 = 0.0f,                                                                        \
        .dead_0  = 0.0f                                                                         \
    }

/**************************************************************************************************
 * 
 * \brief Initializes PWM variable with configuration data
 * 
 * \param NAME PWM variable name
 * \param NUM ePWM generator number (1..8)
 * \param IO_A ePWMxA output I/O configuration code
 * \param IO_B ePWMxB output I/O configuration code
 * \param OUT_B_E Independent B output control enable
 * \param FREQ PWM carrier frequency
 * \param PHASE_E Enable or disable phase shift
 * \param PHASE_0 Initial phase shift (0..1)
 * \param DEAD_0 Initial dead time
 * 
 *************************************************************************************************/
#define _PWM_VAR_INIT(NAME, NUM, IO_A, IO_B, OUT_B_E, FREQ, PHASE_E, PHASE_0, DEAD_0)           \
    *hapi->pwm->NAME = (struct _pwm_var) {                                                      \
        .enable  = true,                                                                        \
        .num     = NUM,                                                                         \
        .base    = 0U,                                                                          \
        .io_A    = IO_A,                                                                        \
        .io_B    = IO_B,                                                                        \
        .out_B_e = OUT_B_E,                                                                     \
        .freq    = FREQ,                                                                        \
        .period  = 0U,                                                                          \
        .phase_e = PHASE_E,                                                                     \
        .phase_0 = PHASE_0,                                                                     \
        .dead_0  = DEAD_0                                                                       \
    }

/**************************************************************************************************
 * 
 * \brief Disables PWM variable
 * 
 * \param NAME PWM variable name
 * 
 *************************************************************************************************/
#define _PWM_VAR_DISABLE(NAME)                                                                  \
    hapi->pwm->NAME->enable = false

/**************************************************************************************************
 * 
 * \brief Generates PWM object structure definition
 * 
 * \param ... optional structure members
 * 
 *************************************************************************************************/
#define _PWM_OBJ_STRUCT(...)                                                                    \
    OBJ_STRUCT(_pwm, _pwm_priv, __VA_ARGS__)

/**************************************************************************************************
 * 
 * \brief Adds variable to PWM object structure definition
 * 
 * \param NAME variable name
 * 
 *************************************************************************************************/
#define _PWM_OBJ_STRUCT_MEMBER(NAME)                                                            \
    OBJ_STRUCT_MEMBER(_pwm_var, NAME)

/**************************************************************************************************
 * 
 * \brief Sets structure member to variable handler
 * 
 * \param NAME variable name
 * 
 *************************************************************************************************/
#define _PWM_MEMBER_SET(NAME)                                                                   \
    .NAME = &NAME##_pwm

#endif /* _CGEN_DRV_PWM_H */
