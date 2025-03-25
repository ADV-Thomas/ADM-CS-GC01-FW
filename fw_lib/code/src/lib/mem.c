/**************************************************************************************************
 * 
 * \file mem.c
 * 
 * \brief External memory implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "inc/lib/mem.h"

#include "inc/lib/debug.h"

/**************************************************************************************************
 * 
 * mem_new()
 * 
 *************************************************************************************************/
extern const struct mem *
mem_new(void)
{
    /** Parameters for Microchip \b 24AA256UID */
    static const struct mem mem = {
        .rd = {
            .address = 0x0000U,
            .length  = 0x8000U
        },
        .wr = {
            .address = 0x0000U,
            .length  = 0x7000U      
        },
        .serial  = 0x7FFCU,
        .page    = 64U,
        .wait    = 6000U,
        .address = 0x00A0U,
        .type = TYPE_EEPROM,
#ifndef VIRTUAL
        .bitrate = 200000UL
#else
        .fpath   = "eeprom.bin"
#endif
    };

    return &mem;
}

/**************************************************************************************************
 * 
 * mem_new()
 * 
 *************************************************************************************************/
extern const struct mem *
mem_new_flash(void)
{
    /** When using FLASH to store module information, the distribution is the following:
     * 0x84000 --> Serial number of module (takes 2 words)
     * 0x84002 --> Flash key (2 words). Used to determine if the sector can be used for config
     * 0x84004 --> Module information starts here
     * 0x84284 --> Last allocated address for storing config in flash. Could be expanded to 0x84FFF
     * 
     * Thus, for the moment we have allocated space for 10 pages of 64 words each.
     * */

    #if defined(F28P65X)
    static const struct mem mem = {
        .rd = {
            .address = 0x0C0004,
            .length  = MEM_FLASH_SIZE*2, /* We multiply by two for the MAL logic to take into
            account 'shadow' pages, but they are not used at all in flash memory type */
        },
        .wr = {
            .address = 0x0C0004,
            .length  = MEM_FLASH_SIZE*2, /* We multiply by two for the MAL logic to take into
            account 'shadow' pages, but they are not used at all in flash memory type */  
        },
        .serial  = 0x0C0000,
        .page    = 64U,
        .wait    = 0,
        .address = 0,
        .type = TYPE_FLASH,
        .sector = 4,
    };


    #else
    static const struct mem mem = {
        .rd = {
            .address = 0x084004,
            .length  = MEM_FLASH_SIZE*2, /* We multiply by two for the MAL logic to take into
            account 'shadow' pages, but they are not used at all in flash memory type */
        },
        .wr = {
            .address = 0x084004,
            .length  = MEM_FLASH_SIZE*2, /* We multiply by two for the MAL logic to take into
            account 'shadow' pages, but they are not used at all in flash memory type */  
        },
        .serial  = 0x084000,
        .page    = 64U,
        .wait    = 0,
        .address = 0,
        .type = TYPE_FLASH,
        .sector = 4,
    };
    #endif

    return &mem;
}

/**************************************************************************************************
 * 
 * mem_check()
 * 
 *************************************************************************************************/
bool
mem_check(const struct mem *mem, uint16_t address, uint16_t length, bool rw_)
{
    ASSERT(mem);

    const struct mem_b *block = rw_ ? &mem->rd : &mem->wr;

    if(!rw_ && (address == mem->serial) && (length == 4)){
        /* If we try to write the Serial number, return TRUE, as this is needed
         for the new type of EEPROM that do not come with a serial number from factory */
        return true;    
    }
    else if ((length == 0U) || (length > block->length)) {
        return false;
    } else if (address < block->address) {
        return false;
    } else if ((address - block->address) > (block->length - length)) {
        return false;
    } else if (!rw_ && (mem->page > 0U)) {
        /* Write operation is limited to single page */
        if ((address / mem->page) != ((address + length - 1U) / mem->page)) {
            return false;
        }
    }

    return true;
}
