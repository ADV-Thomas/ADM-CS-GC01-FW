/**************************************************************************************************
 * 
 * \file main.c
 * 
 * \brief Main program implementation
 * 
 * \author Jorge Sola
 * 
 *************************************************************************************************/

#include "app/ctl.h"
#include "app/hapi.h"
#include "app/tlo.h"

#include "inc/api/adc.h"
#include "inc/api/task.h"
#include "inc/hal/hal.h"
#include "inc/lib/alert.h"
#include "app/display/key.h"
/**************************************************************************************************
 * 
 * isr()
 * 
 *************************************************************************************************/
__attribute__((ramfunc)) static void
isr(const struct tlo *tlo)
{
    //adc_run(tlo->adc, ADC_OP_UPDATE);


    //read_key_coding(tlo->keys);

    //hapi_pwm_update(tlo->ctl->out.pwm);
}

/**************************************************************************************************
 * 
 * main()
 * 
 *************************************************************************************************/
void
main(void){
    const struct tlo *tlo;
    if (!(tlo = tlo_new())) {
        hal_reset();
    }

    if (!alert_get_group(ALERT_ERROR)) {
        hapi_isr_register(isr, tlo);
        hapi_isr_enable();
    } else {
        hapi_isr_disable();
    }

    task_run(tlo->task);
}
