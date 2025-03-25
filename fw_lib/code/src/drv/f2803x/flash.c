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
#include "Flash2803x_API_Library.h"

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
 * Global variables for Flash API. These registers are EALLOW protected!
 * 
 *************************************************************************************************/

__attribute__((section("FlashScalingVar")))
uint32_t Flash_CPUScaleFactor;

__attribute__((section("FlashCallbackVar")))
void (*Flash_CallbackPtr)(void);

/**************************************************************************************************
 * 
 * flash_new()
 * 
 *************************************************************************************************/
const struct net *
flash_new(const struct net *drv)
{
    static const struct flash_sector map[] = {
        FLASH_SECTOR_INIT(0x3F6000UL, 0x2000UL),    /* Sector A */
        FLASH_SECTOR_INIT(0x3F4000UL, 0x2000UL),    /* Sector B */
        FLASH_SECTOR_INIT(0x3F2000UL, 0x2000UL),    /* Sector C */
        FLASH_SECTOR_INIT(0x3F0000UL, 0x2000UL),    /* Sector D */
        FLASH_SECTOR_INIT(0x3EE000UL, 0x2000UL),    /* Sector E */
        FLASH_SECTOR_INIT(0x3EC000UL, 0x2000UL),    /* Sector F */
        FLASH_SECTOR_INIT(0x3EA000UL, 0x2000UL),    /* Sector G */
        FLASH_SECTOR_INIT(0x3E8000UL, 0x2000UL)     /* Sector H */
    };

    NET_PRIV_NEW(flash, NET_FLASH);

    static struct flash flash = {
        .count = 8U,
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

    EALLOW;

    /**
     * Calculate frequency scale factor in IQ20 format: (2**20)*(200)*(f_MHz/1000). Constants can
     * be grouped together: 209715.2 = (209715+(1/5)). This helps avoid 64-bit integer arithmetic.
     */
    Flash_CPUScaleFactor = ((_clk->cpu.MHz * 209715UL) + (_clk->cpu.MHz / 5U));

    Flash_CallbackPtr = NULL;

    EDIS;

    if (Flash_APIVersionHex() != 0x0100U) {
        return -1;
    }

    priv->open = true;

    return 0;
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

    Uint16 *memory = (Uint16 *) f->address;
    Uint16 *data = (Uint16 *) f->data;

    int ret = 0;
    FLASH_ST st;

    EALLOW;

    ret = Flash_Program(memory, data, f->length, &st);

    EDIS;

    if (ret != STATUS_SUCCESS) {
        return -1;
    }

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
    
    /* Convert sector index to bit mask */
    sector = 1U << sector;

    int ret = 0;
    FLASH_ST st;

    EALLOW;

    ret = Flash_Erase(sector, &st);

    EDIS;

    if (ret != STATUS_SUCCESS) {
        return -1;
    }

    return 0;
}
