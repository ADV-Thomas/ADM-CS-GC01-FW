/**************************************************************************************************
 * 
 * \file eeprom.h
 * 
 * \brief EEPROM communication interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_NET_EEPROM_H
#define _INC_NET_EEPROM_H

#include "inc/net/net.h"

#include <stdint.h>
#include <stdbool.h>

/**************************************************************************************************
 * 
 * Forward declarations
 * 
 *************************************************************************************************/

struct mem;

/**************************************************************************************************
 * 
 * User configuration data definition
 * 
 *************************************************************************************************/
struct eeprom_usr {
    const struct mem *mem;      /* Memory object handler                                        */
};

/**************************************************************************************************
 * 
 * Transaction frame definition
 * 
 *************************************************************************************************/
struct eeprom_f {
    uint16_t address;           /* Data start address                                           */
    uint16_t length;            /* Data length                                                  */
    uint8_t *data;              /* Data buffer                                                  */
};

/**************************************************************************************************
 * 
 * Private function prototypes. Functions are implemented in driver and linked to \b net object.
 * 
 *************************************************************************************************/

static int eeprom__open(const struct net *net, const struct eeprom_usr *usr);
static int eeprom__close(const struct net *net);
static int eeprom__read(const struct net *net, struct eeprom_f *f, unsigned length);
static int eeprom__write(const struct net *net, struct eeprom_f *f, unsigned length);

/**************************************************************************************************
 * 
 * \brief Wrapper for \e net_open() function
 * 
 *************************************************************************************************/
extern int
eeprom_open(const struct net *net, const struct eeprom_usr *usr);

/**************************************************************************************************
 * 
 * \brief Wrapper for \e net_close() function
 * 
 *************************************************************************************************/
extern int
eeprom_close(const struct net *net);

/**************************************************************************************************
 * 
 * \brief Wrapper for \e net_read() function
 * 
 *************************************************************************************************/
extern int
eeprom_read(const struct net *net, struct eeprom_f *f);

/**************************************************************************************************
 * 
 * \brief Wrapper for \e net_write() function
 * 
 *************************************************************************************************/
extern int
eeprom_write(const struct net *net, struct eeprom_f *f);

#endif /* _INC_NET_EEPROM_H */
