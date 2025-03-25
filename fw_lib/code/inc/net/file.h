/**************************************************************************************************
 * 
 * \file file.h
 * 
 * \brief File communication interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_NET_FILE_H
#define _INC_NET_FILE_H

#include "inc/net/net.h"

#include <stdint.h>

/**************************************************************************************************
 * 
 * User configuration data definition
 * 
 *************************************************************************************************/
struct file_usr {
    const char *path;           /* Path to binary file                                          */
};

/**************************************************************************************************
 * 
 * Transaction frame definition
 * 
 *************************************************************************************************/
struct file_f {
    uint16_t address;           /* Data start address                                           */
    uint16_t length;            /* Data length                                                  */
    uint8_t *data;              /* Data buffer                                                  */
};

/**************************************************************************************************
 * 
 * Private function prototypes. Functions are implemented in driver and linked to \b net object.
 * 
 *************************************************************************************************/

static int file__open(const struct net *net, const struct file_usr *usr);
static int file__close(const struct net *net);
static int file__read(const struct net *net, struct file_f *f, unsigned length);
static int file__write(const struct net *net, struct file_f *f, unsigned length);

/**************************************************************************************************
 * 
 * \brief Wrapper for \e net_open() function
 * 
 *************************************************************************************************/
extern int
file_open(const struct net *net, const struct file_usr *usr);

/**************************************************************************************************
 * 
 * \brief Wrapper for \e net_close() function
 * 
 *************************************************************************************************/
extern int
file_close(const struct net *net);

/**************************************************************************************************
 * 
 * \brief Wrapper for \e net_read() function
 * 
 *************************************************************************************************/
extern int
file_read(const struct net *net, struct file_f *f);

/**************************************************************************************************
 * 
 * \brief Wrapper for \e net_write() function
 * 
 *************************************************************************************************/
extern int
file_write(const struct net *net, struct file_f *f);

#endif /* _INC_NET_FILE_H */
