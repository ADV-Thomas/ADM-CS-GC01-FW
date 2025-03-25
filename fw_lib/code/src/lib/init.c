/**************************************************************************************************
 * 
 * \file init.c
 * 
 * \brief Common initializer implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "inc/lib/init.h"

#include "inc/api/db.h"
#include "inc/hal/hal.h"
#include "inc/lib/data.h"
#include "inc/lib/debug.h"
#include "inc/lib/mal.h"
#include "inc/lib/mem.h"
#include "inc/lib/nfo.h"
#include "inc/net/can.h"
#include "inc/net/i2c.h"

#ifdef BOOTLOADER
#include "inc/boot/bentry.h"
#include "inc/net/flash.h"
#else
#include "inc/lib/alert.h"
#endif

/**************************************************************************************************
 * 
 * init()
 * 
 *************************************************************************************************/
void
init(struct nfo *mod, struct nfo *boot, struct mal **mal, const struct net **can, unsigned long can_mask)
{
    ASSERT(mod && boot);
    
    volatile int ret;
    
    hal_init();

    const struct net *eeprom = hal_get_eeprom();
    const struct mem *mem_eeprom = mem_new();

    #if defined(F28004X) ||  defined(F28P65X) 
    /* For F28004X devices, we might prefer to use the internal FLASH
     * to store module info and calibration data instead of the external
     * EEPROM. For retrocompatibility, we have the following decision tree:
     *
     * 1. We try to read from FLASH sector 4:
     *      -> If SUCCESSFUL, then initialize everything based on FLASH
     *      -> If NOT SUCCESSFUL then move to point 2.
     * 2. We try to read from external EEPROM:
     *      -> If SUCCESSFUL, then initialize everything from EEPROM
     *      -> If NOT SUCCESSFUL then move to point 3.
     * 3. Read internal DSP unique serial number (in ROM), and copy
     * it inside FLASH sector 4. Initialize everything from FLASH.
     *
     * Because the serial number should always be available in either
     * FLASH or EEPROM, we consider that the memory is not usable when
     * the serial number is not available to read.
     */

    const struct net *flash = hal_get_flash(); //i  arrived here 
    const struct mem *mem_flash = mem_new_flash();
    struct mal *mal_eeprom = mal_new(eeprom);
    struct mal *mal_flash = mal_new_flash(flash);
    ASSERT(*mal);

    /* First, we try reading the FLASH memory */
    struct mal_usr mal_usr = {
        .mem = mem_flash
    };
    ret = mal_open(mal_flash, &mal_usr);
    ASSERT(ret >= 0);
    
    ret = data_nfo(mal_flash, boot, true);
    *mal = mal_flash;

    if(ret == SERIAL_NOT_READABLE){
        /* If FLASH is empty, try with external EEPROM */
        mal_usr.mem = mem_eeprom;
        ret = mal_open(mal_eeprom, &mal_usr);
        ret = data_nfo(mal_eeprom, boot, true);

        if(ret == SERIAL_NOT_READABLE){
            /* If EEPROM does not work, close it and free the GPIOs
             * and use FLASH */
            mal_close(mal_eeprom);
            i2c_disconnect(hal_get_i2c());
            mal_write_serial_number(mal_flash,hal_get_SN());
            data_nfo(mal_flash, boot, true);
        }
        else{
            /* If EEPROM works, keep on using it */
            *mal = mal_eeprom;
        }
    }else{
        /* Flash works, so disconnect I2C pins */
        i2c_disconnect(hal_get_i2c());
    }

    data_nfo(*mal, mod, true);
    #endif

    #ifdef F2803X
    *mal = mal_new(eeprom);
    ASSERT(*mal);

    struct mal_usr mal_usr = {
        .mem = mem_eeprom,
    };

    ret = mal_open(*mal, &mal_usr);
    ASSERT(ret >= 0);
    
    data_nfo(*mal, boot, true);
    data_nfo(*mal, mod, true);

    #ifdef BOOTLOADER
    const struct net *flash = hal_get_flash();
    ret = flash_open(flash);
    ASSERT(ret >= 0);
    #endif

    #endif
    
    
#ifndef BOOTLOADER
    alert_set(ALERT_MEMORY, mod->serial == NFO_SN_DEFAULT);
    
    alert_set(ALERT_NFO, mod->revision == NFO_HW_DEFAULT);
    alert_set(ALERT_NFO, mod->variant == NFO_HW_DEFAULT);
#endif



    ret = hal_setup(mod);
#ifndef BOOTLOADER
    if (ret < 0) {
        alert_set(ALERT_SYSTEM, true);
    }
#endif
    
    enum nfo_id mod_id = mod->id;
    if (mod_id == NFO_NONE) {
        mod_id = NFO_BOOT;
    }

    *can = hal_get_can();

    struct db_id db_id; 
    db_id_set(&db_id, mod_id, mod->address, can_mask);
    
    struct can_usr can_usr = {
        .bitrate = 500000UL,
        .id      = db_id.id,
        .mask    = db_id.mask,
    };

    ret = can_open(*can, &can_usr);
    ASSERT(ret >= 0);
}
