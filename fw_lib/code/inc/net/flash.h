/**************************************************************************************************
 * 
 * \file flash.h
 * 
 * \brief Flash communication interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_NET_FLASH_H
#define _INC_NET_FLASH_H

#include "cgen/net/flash.h"
#include "inc/net/net.h"

#include <stdint.h>
#include <stdbool.h>

/**************************************************************************************************
 * 
 * User configuration data definition
 * 
 *************************************************************************************************/

struct flash_usr;

/**************************************************************************************************
 * 
 * Flash sector definition
 * 
 *************************************************************************************************/
struct flash_sector {
    uint32_t address;           /* Sector start address                                         */
    uint32_t length;            /* Sector length as number of 16-bit words                      */
};

/**************************************************************************************************
 * 
 * Transaction frame definition
 * 
 *************************************************************************************************/
struct flash_f {
    uint32_t address;           /* Data start address                                           */
    uint32_t length;            /* Data length                                                  */
    uint16_t *data;             /* Data buffer                                                  */
};

/**************************************************************************************************
 * 
 * Flash object definition
 * 
 *************************************************************************************************/
struct flash {
    uint16_t count;                         /* Number of Flash sectors in the map               */
    const struct flash_sector *map;         /* Flash sector map                                 */
    int (*erase)(const struct net *net, unsigned sector);
};

/**************************************************************************************************
 * 
 * Private function prototypes. Functions are implemented in driver and linked to \b net object.
 * 
 *************************************************************************************************/

static int flash__open(const struct net *net, const struct flash_usr *usr);
static int flash__close(const struct net *net);
static int flash__read(const struct net *net, struct flash_f *f, unsigned length);
static int flash__write(const struct net *net, struct flash_f *f, unsigned length);

/**************************************************************************************************
 * 
 * Private function prototypes. Functions are implemented in driver and linked to \b flash object.
 * 
 *************************************************************************************************/

static int flash__erase(const struct net *net, unsigned sector);

/**************************************************************************************************
 * 
 * \brief Wrapper for \e net_open() function
 * 
 *************************************************************************************************/
extern int
flash_open(const struct net *net);

/**************************************************************************************************
 * 
 * \brief Wrapper for \e net_close() function
 * 
 *************************************************************************************************/
extern int
flash_close(const struct net *net);

/**************************************************************************************************
 * 
 * \brief Wrapper for \e net_read() function
 * 
 *************************************************************************************************/
extern int
flash_read(const struct net *net, struct flash_f *f);

/**************************************************************************************************
 * 
 * \brief Wrapper for \e net_write() function
 * 
 *************************************************************************************************/
extern int
flash_write(const struct net *net, struct flash_f *f);

/**************************************************************************************************
 * 
 * \brief Erases single Flash sector
 * 
 * \param net net object handler
 * \param sector numeric index of Flash sector to erase
 * 
 * \details Wrapper for \e flash->erase() function. The driver must be opened in order to be
 * able to perform erase operation.
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
flash_erase(const struct net *net, unsigned sector);

/**************************************************************************************************
 * 
 * \brief Checks if transaction frame is in Flash address range
 * 
 * \param net net object handler
 * \param address memory block start address
 * \param length memory block length as number of 16-bit words
 * \param rw_ \e true for read and \e false for write operation
 * 
 * \details This function checks if transaction frame is in Flash address range. It is assumed
 * that Flash sectors are defined in either strictly increasing or decreasing order considering
 * sector start address. Range check algorithm allows gaps between Flash sectors.
 * 
 * \return \e true when frame is in Flash address range
 * 
 *************************************************************************************************/
extern bool
flash_check(const struct net *net, uint32_t address, uint32_t length, bool rw_);

#endif /* _INC_NET_FLASH_H */
