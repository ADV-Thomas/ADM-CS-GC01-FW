/**************************************************************************************************
 * 
 * \file flash.c
 * 
 * \brief Flash driver implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "inc/drv/flash.h"

#include "driverlib/cpu.h"
#include "FlashAPI/FlashTech_F28P65x_C28x.h"

#include "inc/drv/clk.h"
#include "inc/lib/debug.h"
#include "inc/net/flash.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**************************************************************************************************
 * 
 * Private structure definition
 * 
 *************************************************************************************************/
NET_PRIV_STRUCT(flash, );

/**************************************************************************************************
 * 
 * flash_new()
 * 
 *************************************************************************************************/
const struct net *
flash_new(const struct net *drv)
{
    static const struct flash_sector map[] = {
        FLASH_SECTOR_INIT(0x080000, 0x20000),
        FLASH_SECTOR_INIT(0x0A0000, 0x20000),
        FLASH_SECTOR_INIT(0x0C0000, 0x20000),
        FLASH_SECTOR_INIT(0x0E0000, 0x20000),
        FLASH_SECTOR_INIT(0x100000, 0x20000)
    };

    NET_PRIV_NEW(flash, NET_FLASH);

    static struct flash flash = {
        .count = 16U,
        .map   = map,
        .erase = flash__erase
    };

    NET_OBJ_NEW(flash, &flash);
    
    /**
     * Although Flash does not connect to I/O pins, the \e connect flag must be set to \e true.
     * Otherwise, the \e flash_open() function will fail.
     */
    priv.connect = true;

    return &net;
}

/**************************************************************************************************
 * 
 * Net layer implementation. Private functions linked to \b net object.
 * 
 *************************************************************************************************/

/**************************************************************************************************
 * 
 * flash__open()
 * 
 *************************************************************************************************/
__attribute__((ramfunc)) static int
flash__open(const struct net *net, const struct flash_usr *usr)
{
    struct flash_priv *priv = net->priv;

    const struct _clk *_clk = clk_get();
    ASSERT(_clk);

    int ret = 0;

    EALLOW;

    Fapi_StatusType st;

    st = Fapi_initializeAPI(FlashTech_CPU0_BASE_ADDRESS, _clk->cpu.MHz);

    if (st == Fapi_Status_Success) {
        /* This allows operations on Bank0 only */
        st = Fapi_setActiveFlashBank(Fapi_FlashBank0);

        if (st == Fapi_Status_Success) {
            priv->open = true;
        } else {
            ret = -1;
        }
    } else {
        ret = -1;
    }

    EDIS;

    return ret;
}

/**************************************************************************************************
 * 
 * flash__close()
 * 
 *************************************************************************************************/
__attribute__((ramfunc)) static int
flash__close(const struct net *net)
{
    struct flash_priv *priv = net->priv;

    priv->open = false;

    return 0;
}

/**************************************************************************************************
 * 
 * flash__read()
 * 
 *************************************************************************************************/
__attribute__((ramfunc)) static int
flash__read(const struct net *net, struct flash_f *f, unsigned length)
{
    if (!flash_check(net, f->address, f->length, true)) {
        return -1;
    }

    while (Fapi_checkFsmForReady() != Fapi_Status_FsmReady);

    uint16_t *memory = (uint16_t *) f->address;
    
    unsigned i;
    for (i = 0U; i < f->length; i++) {
        f->data[i] = memory[i];
    }

    return 1;
}

/**************************************************************************************************
 * 
 * flash__write()
 * 
 *************************************************************************************************/
__attribute__((ramfunc)) static int
flash__write(const struct net *net, struct flash_f *f, unsigned length)
{
    if (!flash_check(net, f->address, f->length, false)) {
        return -1;
    }

    Fapi_StatusType st;

    EALLOW;

    /**
     * Memory address and buffer length must be aligned to 64-bit boundary. The buffer length
     * could actually be aligned to 32-bit boundary, but that is not supported in the driver.
     */
    if ((f->address & 0x3U) || (f->length & 0x3U)) {
        EDIS;
        return -1;
    }

    /**
     * Program Flash in 64-bit blocks. It is also possible to program Flash in 128-bit blocks,
     * but only if memory address is aligned to 128-bit boundary.
     */
    unsigned i;
    for (i = 0U; i < f->length; i += 4U) {
        uint32_t *memory = (uint32_t *) (f->address + i);
        uint16_t *data = (uint16_t *) (f->data + i);

        /* Write data buffer to Flash */
        st = Fapi_issueProgrammingCommand(memory, data, 4U, NULL, 0U, Fapi_AutoEccGeneration);
        
        while (Fapi_checkFsmForReady() == Fapi_Status_FsmBusy);

        if (st != Fapi_Status_Success) {
            goto write_exit;
        }

        /**
         * This might seem unnecessary for the application, but Flash operation is unreliable
         * without it. See "Erase and program flow" in the datasheet (SPNU628A, pp.38-39).
         */
        if (Fapi_getFsmStatus() != 0) {
            goto write_exit;
        }

        /**
         * &stw variable cannot be replaced by \e NULL, although variable is not being used. See
         * e2e.ti.com/support/microcontrollers/c2000/f/171/p/958269/3540900 for more details.
         */
        Fapi_FlashStatusWordType stw;
        st = Fapi_doVerify(memory, 2U, (uint32_t *) data, &stw);

        if (st != Fapi_Status_Success) {
            goto write_exit;
        }
    }

write_exit:
    EDIS;

    return 1;
}

/**************************************************************************************************
 * 
 * Flash layer implementation. Private functions linked to \b flash object.
 * 
 *************************************************************************************************/

/**************************************************************************************************
 * 
 * flash__erase()
 * 
 *************************************************************************************************/
__attribute__((ramfunc)) static int
flash__erase(const struct net *net, unsigned sector)
{
    const struct flash *flash = net->drv;

    if (sector >= flash->count) {
        return -1;
    }

    uint32_t address = flash->map[sector].address;
    uint32_t length = flash->map[sector].length;

    if (length == 0U) {
        return -1;
    }

    int ret = 0;

    EALLOW;

    Fapi_StatusType st;
    
    st = Fapi_issueAsyncCommandWithAddress(Fapi_EraseSector, (uint32_t *) address);

    while (Fapi_checkFsmForReady() != Fapi_Status_FsmReady);

    if (st != Fapi_Status_Success) {
        ret = -1;
        goto erase_exit;
    }

    /**
     * This might seem unnecessary for the application, but Flash operation is unreliable
     * without it. See "Erase and program flow" in the datasheet (SPNU628A, pp.38-39).
     */
    if (Fapi_getFsmStatus() != 0) {
        ret = -1;
        goto erase_exit;
    }

    /**
     * &stw variable cannot be replaced by \e NULL, although variable is not being used. See
     * e2e.ti.com/support/microcontrollers/c2000/f/171/p/958269/3540900 for more details.
     */
    Fapi_FlashStatusWordType stw;
    st = Fapi_doBlankCheck((uint32_t *) address, (length / 2U), &stw);
    
    if (st != Fapi_Status_Success) {
        ret = -1;
        goto erase_exit;
    }

erase_exit:
    EDIS;

    return ret;
}
