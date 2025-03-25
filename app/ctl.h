/**************************************************************************************************
 * 
 * \file ctl.h
 * 
 * \brief Control interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _APP_CTL_H
#define _APP_CTL_H

#include <stdint.h>
#include <stdbool.h>

/**************************************************************************************************
 * 
 * Forward declarations
 * 
 *************************************************************************************************/

struct adc;
struct fan_ctl;

/**************************************************************************************************
 * 
 * Output PWM variables
 * 
 *************************************************************************************************/
struct pwm_out {
    bool update;                /* Boolean flag to trigger ePWM generator update                */
    bool enable;
    float supply_duty;
    float speed_duty;
};
/**************************************************************************************************
 * 
 * Output PWM variables
 * 
 *************************************************************************************************/
struct pwm {
    struct pwm_out fan1;          /* Duty cycle for three phases                              */
    struct pwm_out fan2;          /* Duty cycle for three phases                              */
    struct pwm_out fan3;          /* Duty cycle for three phases                              */
};

/**************************************************************************************************
 * 
 * User-defined variables
 * 
 *************************************************************************************************/
struct ctl_usr {
    float fan1_supply_ref;
    float fan2_supply_ref;
    float fan3_supply_ref;
    float fan1_speed_ref;
    float fan2_speed_ref;
    float fan3_speed_ref;
    bool fan1_enable;
    bool fan2_enable;
    bool fan3_enable;
    bool external_interface;
};

/**************************************************************************************************
 * 
 * Output variables
 * 
 *************************************************************************************************/
struct ctl_out {                     
    const uint16_t * const error;           /* Filter error state                               */
    const struct pwm *pwm;
};

/**************************************************************************************************
 * 
 * Control object definition
 * 
 *************************************************************************************************/
struct ctl {
    struct ctl_priv * const priv;   /* Private variables                        *do-not-access* */
    struct ctl_usr usr;             /* User-defined variables                                   */
    struct ctl_out out;             /* Output variables                                         */
    bool can_lock;
};

/**************************************************************************************************
 * 
 * \brief Creates new control object
 * 
 * \param adc ADC object handler
 * \param rms RMS object handler
 * 
 * \return Control object handler
 * 
 *************************************************************************************************/
extern struct ctl *
ctl_new(const struct adc *adc, const struct fan_ctl *fan_ctl);

/**************************************************************************************************
 * 
 * \brief Runs control background task
 * 
 * \param ctl control object handler
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
ctl_background(struct ctl *ctl);

/**************************************************************************************************
 * 
 * \brief Runs control routine
 * 
 * \param ctl control object handler
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
ctl_run(struct ctl *ctl);

#endif /* _APP_CTL_H */
