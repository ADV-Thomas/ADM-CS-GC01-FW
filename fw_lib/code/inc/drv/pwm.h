/**************************************************************************************************
 * 
 * \file pwm.h
 * 
 * \brief PWM driver interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_DRV_PWM_H
#define _INC_DRV_PWM_H

#include "driverlib/epwm.h"

#include "cgen/drv/pwm.h"
#include "inc/api/obj.h"
#include "inc/drv/io.h"

#include <stdint.h>
#include <stdbool.h>

/**************************************************************************************************
 * 
 * Forward declarations
 * 
 *************************************************************************************************/

struct _pwm;

/**************************************************************************************************
 * 
 * Private variables definition
 * 
 *************************************************************************************************/
OBJ_PRIV_STRUCT(_pwm_priv,
    uint16_t init;                      /* Bit mask which indicates if ePWM generator is in use */
);

/**************************************************************************************************
 * 
 * PWM variable definition
 * 
 *************************************************************************************************/
struct _pwm_var {
    bool enable;                        /* Enables PWM variable if true                         */
    uint16_t num;                       /* ePWM generator number (1..8)                         */
    uint32_t base;                      /* ePWM generator base register address                 */
    enum io io_A;                       /* I/O pin configuration code for ePWMxA output         */
    enum io io_B;                       /* I/O pin configuration code for ePWMxB output         */
    bool out_B_e;                       /* Enables independent B output control when true       */
    uint32_t freq;                      /* PWM carrier frequency (Hz)                           */
    uint16_t period;                    /* Time-base counter period for given frequency         */
    bool phase_e;                       /* Enables time-base counter phase shift when true      */
    float phase_0;                      /* Initial time-base counter phase shift value (0..1)   */
    float dead_0;                       /* Initial rising- and falling-edge delay (us)          */
};

/**************************************************************************************************
 * 
 * \brief Configures all PWM variables in the PWM object
 * 
 * \param pwm PWM object handler
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
pwm_setup(const struct _pwm *pwm);

/**************************************************************************************************
 * 
 * \brief Setups start-of-conversion trigger for ADC
 * 
 * \param base base address of the EPWM module
 * \param type ADC SOC type (SOC_A or SOC_B)
 * \param source SOC trigger source
 * \param prescale number of events needed to generate SOC trigger
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
pwm_trigger(uint32_t base, EPWM_ADCStartOfConversionType type,
    EPWM_ADCStartOfConversionSource source, uint16_t prescale);

#endif /* _INC_DRV_PWM_H */
