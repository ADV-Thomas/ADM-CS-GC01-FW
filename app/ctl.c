/**************************************************************************************************
 * 
 * \file ctl.c
 * 
 * \brief Control implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "app/ctl.h"

#include "app/adc.h"
#include "app/hapi.h"
#include "inc/lib/fan_ctl.h"
#include "app/user.h"

#include "inc/lib/alert.h"
#include "inc/lib/debug.h"
#include "inc/lib/ghm.h"

#include <stddef.h>

#include "IQmathLib.h"

/**************************************************************************************************
 * 
 * Private variables definition
 * 
 *************************************************************************************************/
struct ctl_priv {
    uint16_t error;                 /* Filter error state                                       */
    const struct adc *adc;          /* ADC object handler                                       */
    const struct fan_ctl *fan_ctl;
    struct pwm pwm;                 /* PWM struct                                               */
    uint32_t timer;                 /* Software timer                                           */
};

/**************************************************************************************************
 * 
 * ctl_new()
 * 
 *************************************************************************************************/
struct ctl *
ctl_new(const struct adc *adc, const struct fan_ctl *fan_ctl)
{
    ASSERT(adc && fan_ctl);

    if (!(adc) || !(fan_ctl) ) {
        return NULL;
    }

    static struct ctl_priv priv = {
        .error = 0U,
        .adc   = NULL,
        .timer = 0U,
        .fan_ctl = NULL,
    };

    priv.adc = adc;
    priv.fan_ctl = fan_ctl;

    static struct ctl ctl = {
        .priv = &priv,
        .usr = {
            .fan1_speed_ref = 0.0,
            .fan2_speed_ref = 0.0,
            .fan3_speed_ref = 0.0,
            .fan1_enable = false,
            .fan2_enable = false,
            .fan3_enable = false,
            .fan1_supply_ref = 0.0,
            .fan2_supply_ref = 0.0,
            .fan3_supply_ref = 0.0,
        },
        .out = {
            .error = &priv.error,
        }
    };

    /** TODO: this node was just hardcoded for the AFE tester
    struct node node = (struct node){
        .active = true,
        .max_speed = 100.0,
        .min_temp = 30.0,
        .max_temp = 60.0,
        .suscribed = true,
        .id = 0x1078005,
        .offset = 0,
        
    };
    fan_ctl_add_node(fan_ctl, &node);
    */
    ctl.out.pwm = &priv.pwm;

    return &ctl;
}


/**************************************************************************************************
 * 
 * ctl_background()
 * 
 *************************************************************************************************/
void
ctl_background(struct ctl *ctl)
{
    ASSERT(ctl);

}

float fsat(float input,float min,float max)
{
    return input > max ? max : input < min ? min : input;
}
/**************************************************************************************************
 * 
 * ctl_run()
 * 
 *************************************************************************************************/
__attribute__((ramfunc)) void
ctl_run(struct ctl *ctl)
{

    
    ASSERT(ctl);
   
}
