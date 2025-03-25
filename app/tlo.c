/**************************************************************************************************
 * 
 * \file tlo.c
 * 
 * \brief Top-level object implementation
 * 
 * \author Jorge Sola
 * 
 *************************************************************************************************/

#include "app/tlo.h"


#include "app/adc.h"
#include "app/ctl.h"
#include "app/task.h"
#include "app/wcs.h"
#include "app/db.h"
#include "app/wcs.h"
#include "app/dev_ctl.h"
#include "app/superset_ctl.h"



#include "app/display/key.h"
#include "app/display/state_machine.h"


#include "inc/api/db.h"
#include "inc/hal/hal.h"
#include "inc/lib/alert.h"
#include "inc/lib/init.h"
#include "inc/lib/nfo.h"
#include "inc/lib/dlog.h"
#include "inc/lib/logging.h"


#include "adm_cs_fp_db.h"
#include "adm_pc_bp25_db.h"
#include "adm_pc_vg11_fm01_db.h"
#include "adm_pc_vg11_fm02_db.h"


#include <stddef.h>





/** For this application, we want to listen to temperature messages from other modules
 * so we set the CAN mask to 0 so that the hardware does not filter them out */
#define CAN_MASK (0)
/**************************************************************************************************
 * 
 * tlo_new()
 * 
 *************************************************************************************************/
const struct tlo *
tlo_new(void)
{
    static struct tlo tlo = {
        .mod  = NULL,
        .boot = NULL,
        .mal  = NULL,
        .can  = NULL,
        .db   = NULL,
        .db_afe = NULL,
        .db_vg11_fm01 = NULL,
        .db_vg11_fm02 = NULL,
        .adc  = NULL,
        .ctl  = NULL,
        .task = NULL,
        //.wcs = NULL,
        .dlog = NULL,
        .dlog_db = NULL,
        .logging_db = NULL,
        .keys = NULL,
        .state_machine = NULL,
        .dev_ctl = NULL,
        .superset_ctl = NULL,




    };

    tlo.mod = nfo_new(NFO_FAN1);

    #if defined(F28P65X)
        tlo.mod->variant = 1;
        tlo.mod->revision = 1;
    #endif

    tlo.boot = nfo_new(NFO_BOOT);
    static double _coding_wheel_value = 0;
    tlo.coding_wheel_value = &_coding_wheel_value;

    
    static double _blink_speed = 100;
    tlo.blink_speed = &_blink_speed;

    #if !defined(DLOG) && !defined(LOGGING)
        init(tlo.mod, tlo.boot, &tlo.mal, &tlo.can, 0x00000000UL);
    #else
        init(tlo.mod, tlo.boot, &tlo.mal, &tlo.can, 0x00000000UL);
    #endif

    /* Set debounce period to 50 ms for external interlock */
    alert_period(ALERT_EXTERNAL, 50U);

    tlo.adc = adc_new(tlo.mod, tlo.mal);
    //tlo.wcs = wcs_new(tlo.adc,tlo.mod);
    
    tlo.dev_ctl = dev_ctl_new(&tlo);
    tlo.superset_ctl = superset_ctl_new(&tlo);




    

    //tlo.ctl = ctl_new(tlo.adc, tlo.fan_ctl);

    tlo.task = task_new(&tlo);

    tlo.keys = key_new(&tlo);
    tlo.state_machine = state_machine_new(&tlo);



    /* FP  database */
    tlo.db = adm_cs_fp_db_new(&tlo);
    adm_cs_fp_db_init(tlo.db, &tlo);
    db_add_exception_filter(handle_db_exceptions,(const struct db *)tlo.db);
    db_subscribe((const struct db *) tlo.db, tlo.mod->id, tlo.mod->address, DB_ID_DEV_ADR_M);


    /* AFE  database */
    tlo.db_afe = adm_pc_bp25_db_new(&tlo);
    adm_pc_bp25_db_init(tlo.db_afe, &tlo);




    /* VG11 FM01  database */
    tlo.db_vg11_fm01 = adm_pc_vg11_fm01_db_new(&tlo);
    adm_pc_vg11_fm01_db_init(tlo.db_vg11_fm01, &tlo);


  
    
    /*VG11 FM02  database */
    tlo.db_vg11_fm02 = adm_pc_vg11_fm02_db_new(&tlo);
    adm_pc_vg11_fm02_db_init(tlo.db_vg11_fm02, &tlo);


    
    #ifdef DLOG
    tlo.dlog_db = dlog_db_new(&tlo);
    tlo.dlog = dlog_new(&tlo);
    dlog_db_init(tlo.dlog_db, &tlo);
    db_subscribe((const struct db *) tlo.dlog_db, NFO_DLOG, tlo.mod->address, DB_ID_DEV_ADR_M);
    #endif


    #ifdef LOGGING
        tlo.logging_db = logging_db_new(&tlo);
        logging_init( (const struct logging_db *)  tlo.logging_db);
        logging_db_init(tlo.logging_db, &tlo);
        db_subscribe((const struct db *) tlo.logging_db, NFO_LOGGING, tlo.mod->address, DB_ID_DEV_ADR_M);
    #endif
    


  

    alert_set(ALERT_SYSTEM, !(tlo.adc && tlo.task && tlo.dev_ctl &&tlo.db ));
    
    return &tlo;
}


