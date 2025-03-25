/**************************************************************************************************
 * 
 * \file flash.c
 * 
 * \brief Flash communication interface implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "inc/net/flash.h"

#include "inc/lib/debug.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**************************************************************************************************
 * 
 * flash_open()
 * 
 *************************************************************************************************/
int
flash_open(const struct net *net)
{
    return net_open(net, NULL, NET_FLASH);
}

/**************************************************************************************************
 * 
 * flash_close()
 * 
 *************************************************************************************************/
int
flash_close(const struct net *net)
{
    return net_close(net, NET_FLASH);
}

/**************************************************************************************************
 * 
 * flash_read()
 * 
 *************************************************************************************************/
int
flash_read(const struct net *net, struct flash_f *f)
{
    return net_read(net, f, 1U, NET_FLASH);
}

/**************************************************************************************************
 * 
 * flash_write()
 * 
 *************************************************************************************************/
int
flash_write(const struct net *net, struct flash_f *f)
{
    return net_write(net, f, 1U, NET_FLASH);
}

/**************************************************************************************************
 * 
 * flash_erase()
 * 
 *************************************************************************************************/
int
flash_erase(const struct net *net, unsigned sector)
{
    if (!net_check(net, NET_FLASH)) {
        return -1;
    }

    NET_PRIV_STRUCT(net, );

    struct net_priv *priv = (struct net_priv *) net->priv;
    const struct flash *flash = net->drv;

    if (priv->open && flash && flash->erase) {
        return flash->erase(net, sector);
    } else {
        return -1;
    }
}

/**************************************************************************************************
 * 
 * flash_range()
 * 
 *************************************************************************************************/
__attribute__((ramfunc)) bool
flash_check(const struct net *net, uint32_t address, uint32_t length, bool rw_)
{
    ASSERT(net);

    const struct flash *flash = net->drv;
    ASSERT((flash->count > 0U) && flash->map);

    uint16_t count = flash->count;
    const struct flash_sector *map = flash->map;

    /* True when Flash sectors are specified in increasing order considering start address */
    bool increasing = map[0].address <= map[count-1].address;

    uint32_t end_address = 0U;
    
    bool found = false;

    unsigned i;
    for (i = 0U; i < count; i++) {
        /* Get Flash sector from the map in increasing memory order */
        const struct flash_sector *sector = &map[i];
        if (!increasing) {
            sector = &map[count-1-i];
        }

        /* Skip empty sectors */
        if (sector->length == 0U) {
            continue;
        }

        /* Sectors must not overlap */
        if ((i > 0U) && (sector->address <= end_address)) {
            return false;
        }
        
        /* Get sector end address */
        end_address = sector->address + sector->length - 1U;

        if ((address >= sector->address) && (address <= end_address)) {
            /* Found at least one matching sector */
            found = true;

            /* Number of words that could be placed in this sector */
            uint16_t residual = end_address - address + 1U;

            if (length <= residual) {
                /* The whole data block fits in this sector */
                return true;
            } else {
                address += residual;
                length -= residual;
            }
        } else if (found) {
            /* Trying to access a gap between two sectors */
            return false;
        }
    }
    
    return false;
}
