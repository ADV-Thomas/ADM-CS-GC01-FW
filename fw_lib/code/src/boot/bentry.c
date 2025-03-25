/**************************************************************************************************
 * 
 * \file bentry.c
 * 
 * \brief Boot entry module implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "inc/boot/bentry.h"

#ifndef BOOTLOADER

/**************************************************************************************************
 * 
 * \brief Program entry point that initializes microcontroller and calls main()
 * 
 * \param None
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
c_int00(void);

/**************************************************************************************************
 * 
 * Boot entry object. Our convention is to place it at the start of a Flash sector, so that
 * \e bentry_jump() could find it.
 * 
 *************************************************************************************************/
__attribute__((section("bentry")))
const struct bentry bentry = {
    .key         = BENTRY_KEY,
    .boot        = BENTRY_BOOT,
    .checksum    = 0UL,
    .entry_point = (uint32_t) c_int00
};

#else

#include "inc/net/flash.h"

/**************************************************************************************************
 * 
 * Defines for the boot entry module
 * 
 *************************************************************************************************/

#define BENTRY_SIZE     (sizeof(struct bentry) / sizeof(uint16_t))

/**************************************************************************************************
 * 
 * \brief Safely jumps to application program
 * 
 * \param entry_point application program address
 * 
 * \note This function is defined in boot.asm file as _boot_exit symbol.
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
boot_exit(uint32_t entry_point);

/**************************************************************************************************
 * 
 * \brief Searches and jumps to application program
 * 
 * \param net flash driver handler
 * \param f Flash transaction frame
 * 
 * \return None
 * 
 *************************************************************************************************/
static void
bentry_search_and_jump(const struct net *net, struct flash_f *f)
{
    if (flash_read(net, f) != 1) {
        return;
    }

    const struct bentry *b = (struct bentry *) f->data;
    
    if (b->key == BENTRY_KEY && b->boot == BENTRY_BOOT && b->checksum == 0UL) {
        /** TODO: Cleanup HAL and HAPI before jumping? */
        boot_exit(b->entry_point);
    }
}

/**************************************************************************************************
 * 
 * bentry_jump()
 * 
 *************************************************************************************************/
void
bentry_jump(const struct net *net)
{
    const struct flash *flash = net->drv;

    const struct flash_sector *map = flash->map;

    uint16_t buffer[BENTRY_SIZE];
    
    struct flash_f f = {
        .address = 0UL,
        .length  = BENTRY_SIZE,
        .data    = buffer
    };

    unsigned i;
    for (i = 0U; i < flash->count; i++) {
        /**
         * By our convention, the boot entry structure is placed at the end of the last sector: at
         * \e 0x3F3FF8 (sector C) on f2803x and at \e 0x08FFF8 (sector 15) on f28004x devices. Note
         * that sectors A..H translate to 0..7 indexes on f2803x. Bootloader sectors should not be
         * searched for boot entry structure: A..B on f2803x and 0..2 on f28004x devices. Also,
         * shadow bootloader sectors should not be checked either (sectors H..G on f2803x and
         * 3..5 on f28004x)
         */
#if defined(F2803X)
        unsigned s = i;
        if (s <= 1U || s >= 6) {
            continue;
        }
#elif defined(F28004X) || defined(F28P65X)
        /* First search in sector 15 as boot entry structure is most likely there */
        unsigned s = flash->count - i - 1U;
        if (s <= 5U) {
            continue;
        }
#else
#error "bentry.c - dsp not defined"
#endif

        if (map[s].length < BENTRY_SIZE) {
            continue;
        }
        
        /* Search at the end of the Flash sector */
        f.address = map[s].address + map[s].length - BENTRY_SIZE;
        bentry_search_and_jump(net, &f);

        /* Search at the start of the Flash sector */
        f.address = map[s].address;
        bentry_search_and_jump(net, &f);
    }
}

#endif
