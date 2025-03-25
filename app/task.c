/**************************************************************************************************
 * 
 * \file task.c
 * 
 * \brief Task scheduler implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "app/task.h"

#include "app/tlo.h"
#include "app/wcs.h"
#include "app/adc.h"
#include "app/ctl.h"
#include "app/dev_ctl.h"
#include "app/user.h"

#include "inc/api/db.h"
#include "inc/hal/hal.h"

#include "app/hapi.h"

#include "app/display/state_machine.h"
#include "app/display/key.h"

#include <stddef.h>


#include "inc/net/can.h"


/**************************************************************************************************
 * 
 * \brief Callback function for CAN communication
 * 
 * \param tlo top-level object handler
 * 
 * \return None
 * 
 *************************************************************************************************/
static void
callback_can(const struct tlo *tlo)
{
    
    /*const struct db * db_a[4];
    db_a[0] =  (const struct db *) tlo->db;
    db_a[1] =  (const struct db *) tlo->db_afe;*/


    const struct db * db[]={
        (const struct db *) tlo->db,
        (const struct db *) tlo->db_afe,
        (const struct db *) tlo->db_vg11_fm01,
        (const struct db *) tlo->db_vg11_fm02,
        #ifdef DLOG
        (const struct db *) tlo->dlog_db,
        #endif
        #ifdef LOGGING
        (const struct db *) tlo->logging_d,
        #endif

    };


    
    //tlo->ctl->can_lock = true;
    uint16_t can_size =  sizeof(db)/sizeof(db[0]);

    db_run(tlo->can, db, can_size);



    //we subscribe only to send a message then unsubscribe to keep receiving all message
    db_unsubscribe((const struct db *) tlo->db_afe);
    db_unsubscribe((const struct db *) tlo->db_vg11_fm01);
    db_unsubscribe((const struct db *) tlo->db_vg11_fm02);


    //tlo->ctl->can_lock = false;
    dev_ctl_update_timestamp(tlo->dev_ctl);
}


static void
callback_screen(const struct tlo *tlo)
{
    state_machine_run(tlo->state_machine);
}



/**************************************************************************************************
 * 
 * \brief Callback function for blink LED
 * 
 * \param tlo top-level object handler
 * 
 * \return None
 * 
 *************************************************************************************************/

static void
callback_blink(const struct tlo *tlo)
{
    
    static unsigned counter = 0U;
    int max_counter = 5;   
    if (++counter >=max_counter) {
        counter = 0U;
        hapi_toggle_led_1();
    }
}


/**************************************************************************************************
 * 
 * \brief Callback function for measurements
 * 
 * \param tlo top-level object handler
 * 
 * \return None
 * 
 *************************************************************************************************/
static void
callback_meas(const struct tlo *tlo)
{
   
    //adc_run(tlo->adc, ADC_OP_FILTER);
    read_key_button(tlo->keys);
    read_key_coding(tlo->keys);
  
}

/**************************************************************************************************
 * 
 * \brief Callback function for physical measurements
 * 
 * \param tlo top-level object handler
 * 
 * \return None
 * 
 *************************************************************************************************/
static void
callback_phy(const struct tlo *tlo)
{
    //adc_run(tlo->adc, ADC_OP_PHYSICAL);
    //read_key_coding(tlo->keys);
}

/**************************************************************************************************
 * 
 * \brief Callback function for control
 * 
 * \param tlo top-level object handler
 * 
 * \return None
 * 
 *************************************************************************************************/
static void
callback_ctl(const struct tlo *tlo)
{
    //check if dev are alive
    dev_ctl_update_timestamp(tlo->dev_ctl);

    static unsigned counter = 0U;
    int max_counter = 1000;   
    if (++counter >=max_counter) { //check avery sec
        dev_ctl_check_alive(tlo->dev_ctl);

    }

   //ctl_background(tlo->ctl);
}
/**************************************************************************************************
 * 
 * task_new()
 * 
 *************************************************************************************************/
const struct task *
task_new(const struct tlo *tlo)
{
    ASSERT(tlo);

    volatile  int ret;

    TASK_JOB_NEW(can,   C_TASK_FREQ_CAN, callback_can);
    TASK_JOB_NEW(blink,  10U, callback_blink);
    TASK_JOB_NEW(screen,   10U, callback_screen);
    TASK_JOB_NEW(meas,  C_TASK_FREQ_MEAS  , callback_meas);
    TASK_JOB_NEW(phy,   C_TASK_FREQ_MEAS , callback_phy);
    TASK_JOB_NEW(ctl,   1000, callback_ctl);

    TASK_OBJ_NEW(
        OBJ_MEMBER_SET(can),
        OBJ_MEMBER_SET(blink),
        OBJ_MEMBER_SET(meas),
        OBJ_MEMBER_SET(phy),
        OBJ_MEMBER_SET(ctl),
        OBJ_MEMBER_SET(screen),

   
    );

    priv.tlo = tlo;

    ret = task_init(&task);
    if (ret < 0) {
        return NULL;
    }

    return &task;
}
