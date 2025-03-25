/**************************************************************************************************
 * 
 * \file bentry.h
 * 
 * \brief Boot entry module interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_BOOT_BENTRY_H
#define _INC_BOOT_BENTRY_H

#include <stdint.h>

/**************************************************************************************************
 * 
 * Defines for the boot entry module
 * 
 *************************************************************************************************/

#define BENTRY_KEY      (0x0000AA55UL)

#define BENTRY_BOOT     (1UL)

/**************************************************************************************************
 * 
 * Boot entry object definition
 * 
 *************************************************************************************************/
struct bentry {
    uint32_t key;               /* Key to identifiy boot entry block                            */
    uint32_t boot;              /* Enable or disable boot entry (not in use)                    */
    uint32_t checksum;          /* Checksum of the flash data (not in use)                      */
    uint32_t entry_point;       /* Program entry point (absolute address)                       */
};

#ifdef BOOTLOADER

/**************************************************************************************************
 * 
 * Forward declarations
 * 
 *************************************************************************************************/

struct net;

/**************************************************************************************************
 * 
 * \brief Jumps to application program
 * 
 * \param net flash driver handler
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
bentry_jump(const struct net *net);

#endif

#endif /* _INC_BOOT_BENTRY_H */
