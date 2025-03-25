/**************************************************************************************************
 * 
 * \file adm_cs_fp_db.c
 * 
 * \brief ADM-PC-BP25 CAN database callback implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "adm_cs_fp_db.h"

#include "app/adc.h"
#include "app/ctl.h"
#include "app/tlo.h"
#include "app/user.h"
#include "app/wcs.h"


#include "inc/hal/hal.h"
#include "inc/hal/hapi.h"
#include "inc/lib/alert.h"
#include "inc/lib/nfo.h"
#include "inc/lib/data.h"
#include "inc/drv/dio.h"

#include "app/hapi.h"

/**************************************************************************************************
 * 
 * adm_cs_fp_db_init()
 * 
 *************************************************************************************************/
DB_PROTOTYPE_INIT(adm_cs_fp_db)
{
    /* NOT Periodic callbacks (called just once now) */
    DB_SET_APERIODIC_CALLBACK(adm_cs_fp_fp_identification);
    DB_SET_APERIODIC_CALLBACK(adm_cs_fp_fp_fw_info);
    DB_SET_APERIODIC_CALLBACK(adm_cs_fp__fp_boot_fw_info);

    DB_MSG_CALLBACK(adm_cs_fp_fp_identification);
    DB_MSG_CALLBACK(adm_cs_fp_fp_fw_info);
    DB_MSG_CALLBACK(adm_cs_fp__fp_boot_fw_info);

        
    DB_MSG_ENABLE(adm_cs_fp_fp_identification);
    DB_MSG_ENABLE(adm_cs_fp_fp_fw_info);
    DB_MSG_ENABLE(adm_cs_fp__fp_boot_fw_info);


    return 0;
}




/**************************************************************************************************
 * 
 * adm_cs_fp_afe_voltage_setpoint_control_callback()
 * 
 *************************************************************************************************/
DB_PROTOTYPE_CALLBACK(adm_cs_fp_fp_stack_control)
{
    /* Pointers for shorter notation */
    struct nfo *mod = tlo->mod;

    int ret;

    /** TODO: Etka swaps serial number bytes. Stop doing this in Etka! */
    uint32_t serial = 0U;
    serial |= (mod->serial >> 24) & 0x000000FFUL;
    serial |= (mod->serial >>  8) & 0x0000FF00UL;
    serial |= (mod->serial <<  8) & 0x00FF0000UL;
    serial |= (mod->serial << 24) & 0xFF000000UL;

    if (msg->sn_number != serial){
        return;
    }

    uint16_t stack_size = msg->stack_size;
    if (stack_size == 0U) {
        stack_size = 1U;
    }

    ret = nfo_set_address(mod, msg->stack_position, stack_size);
    if (ret <= 0) {
        return;
    }

    data_nfo(tlo->mal, mod, false);

    hal_reset();    
}

/**************************************************************************************************
 * 
 * adm_cs_fp_afe_voltage_setpoint_control_callback()
 * 
 *************************************************************************************************/
DB_PROTOTYPE_CALLBACK(adm_cs_fp_fp_identification)
{
    const struct nfo *mod = tlo->mod;

    msg->device_type = (uint16_t) mod->id;
    msg->hw_revision = mod->revision;
    msg->hw_variant = mod->variant;
    msg->stack_position = mod->address;

    /** TODO: Etka swaps serial number bytes. Stop doing this in Etka! */
    uint32_t serial = 0U;
    serial |= (mod->serial >> 24) & 0x000000FFUL;
    serial |= (mod->serial >>  8) & 0x0000FF00UL;
    serial |= (mod->serial <<  8) & 0x00FF0000UL;
    serial |= (mod->serial << 24) & 0xFF000000UL;

    msg->sn_number = serial;    
}


/**************************************************************************************************
 * 
 * adm_cs_fp_afe_voltage_setpoint_control_callback()
 * 
 *************************************************************************************************/
DB_PROTOTYPE_CALLBACK(adm_cs_fp_fp_status)
{
    msg->ready = false;
    msg->running = false;
}


/**************************************************************************************************
 * 
 * adm_cs_fp_afe_voltage_setpoint_control_callback()
 * 
 *************************************************************************************************/
DB_PROTOTYPE_CALLBACK(adm_cs_fp_fp_fw_info)
{
    const struct nfo *mod = tlo->mod;

    if (msg == tlo->db->adm_cs_fp__fp_boot_fw_info->msg) {
        mod = tlo->boot;
    }

    char version[14], timestamp[14];

    nfo_firmware_unpack16(mod->version, version);
    nfo_firmware_unpack16(mod->timestamp, timestamp);

    msg->fw_revision_0  = version[0];
    msg->fw_revision_1  = version[1];
    msg->fw_revision_2  = version[2];
    msg->fw_revision_3  = version[3];
    msg->fw_revision_4  = version[4];
    msg->fw_revision_5  = version[5];
    msg->fw_revision_6  = version[6];
    msg->fw_revision_7  = version[7];
    msg->fw_revision_8  = version[8];
    msg->fw_revision_9  = version[9];
    msg->fw_revision_10 = version[10];
    msg->fw_revision_11 = version[11];
    msg->fw_revision_12 = version[12];
    msg->fw_revision_13 = version[13];

    msg->fw_datecode_0  = timestamp[0];
    msg->fw_datecode_1  = timestamp[1];
    msg->fw_datecode_2  = timestamp[2];
    msg->fw_datecode_3  = timestamp[3];
    msg->fw_datecode_4  = timestamp[4];
    msg->fw_datecode_5  = timestamp[5];
    msg->fw_datecode_6  = timestamp[6];
    msg->fw_datecode_7  = timestamp[7];
    msg->fw_datecode_8  = timestamp[8];
    msg->fw_datecode_9  = timestamp[9];
    msg->fw_datecode_10 = timestamp[10];
    msg->fw_datecode_11 = timestamp[11];
    msg->fw_datecode_12 = timestamp[12];
    msg->fw_datecode_13 = timestamp[13];    
}
/**************************************************************************************************
 * 
 * adm_cs_fp_afe_voltage_setpoint_control_callback()
 * 
 *************************************************************************************************/
DB_PROTOTYPE_CALLBACK(adm_cs_fp__fp_boot_fw_info)
{
    /* This assumes that boot_fw_info and fw_info messages have the same layout! */
    adm_cs_fp_fp_fw_info_callback(tlo, (struct adm_cs_fp_fp_fw_info_t *) msg);    
}






