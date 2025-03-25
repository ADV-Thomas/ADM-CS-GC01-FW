/**************************************************************************************************
 * 
 * \file mem.h
 * 
 * \brief External memory interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_LIB_MEM_H
#define _INC_LIB_MEM_H

#include <stdint.h>
#include <stdbool.h>

/**************************************************************************************************
 * 
 * Default defines
 * 
 *************************************************************************************************/

#define MEM_PAGE        64U     /* Default page size as number of bytes                         */

/* This is the size in words that is reserved for the FLASH module configuration.
It corresponds to 2 words for 32-bit serial number, plus another 2 words for the
MAL_FLASH_KEY (to indicate that the flash is being used to store the module config)
plus 15 pages of 64 words each. This number mut be a multiple of 4, because flash
writes must be aligned to 64 bits (4 words) */
#define MEM_FLASH_SIZE  964     

/**************************************************************************************************
 * 
 * Memory block definition
 * 
 *************************************************************************************************/
struct mem_b {
    uint32_t address;           /* Memory block start address                                   */
    uint32_t length;            /* Memory block length as number of bytes                       */
};

enum mem_type{
    TYPE_EEPROM = 0,
    TYPE_FLASH,
};

/**************************************************************************************************
 * 
 * Memory object definition
 * 
 *************************************************************************************************/
struct mem {
    struct mem_b rd;            /* Memory block with read access                                */
    struct mem_b wr;            /* Memory block with write access                               */
    uint32_t serial;            /* Address of the 32-bit serial number                          */
    uint16_t page;              /* Page size as number of bytes                                 */
    uint16_t wait;              /* Wait time after page write (us)                              */
    uint16_t address;           /* External memory physical address                             */
    enum mem_type type;
    uint16_t sector;            /* Only for FLASH memory type */
#ifndef VIRTUAL
    uint32_t bitrate;           /* Communication speed (bps)                                    */
#else
    const char *fpath;          /* Path to EEPROM emulation file                                */
#endif
};

/**************************************************************************************************
 * 
 * \brief Creates new memory object
 * 
 * \param None
 * 
 * \return Memory object handler
 * 
 *************************************************************************************************/
extern const struct mem *
mem_new(void);

/**************************************************************************************************
 * 
 * \brief Creates new memory object
 * 
 * \param None
 * 
 * \return Memory object handler
 * 
 *************************************************************************************************/
extern const struct mem *
mem_new_flash(void);

/**************************************************************************************************
 * 
 * \brief Checks if memory block is valid
 * 
 * \param mem memory object handler
 * \param address memory block start address
 * \param length memory block length as number of bytes
 * \param rw_ \e true for read and \e false for write operation
 * 
 * \return \e true when memory block is valid
 * 
 *************************************************************************************************/
extern bool
mem_check(const struct mem *mem, uint16_t address, uint16_t length, bool rw_);

#endif /* _INC_LIB_MEM_H */
