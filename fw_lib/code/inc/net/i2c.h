/**************************************************************************************************
 * 
 * \file i2c.h
 * 
 * \brief I2C communication interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_NET_I2C_H
#define _INC_NET_I2C_H

#include "inc/net/net.h"
#include "inc/drv/io.h"

#include <stdint.h>
#include <stdbool.h>

/**************************************************************************************************
 * 
 * User configuration data definition
 * 
 *************************************************************************************************/
struct i2c_usr {
    uint32_t bitrate;           /* Communication speed (bps)                                    */
    uint16_t bitcount;          /* Number of bits per data byte (1..8)                          */
};

/**************************************************************************************************
 * 
 * Transaction frame definition
 * 
 *************************************************************************************************/
struct i2c_f {
    bool rw_;                   /* True for read and false for write operation                  */
    bool start;                 /* True to disable START condition for this frame (if possible) */
    uint32_t start_us;          /* Timeout to generate repeated START condition until ACK (us)  */
    uint16_t address;           /* 7-bit or 10-bit slave (physical) address                     */
    uint16_t length;            /* Data buffer length                                           */
    uint8_t *data;              /* Data buffer                                                  */
};

/**************************************************************************************************
 * 
 * I2C object definition
 * 
 *************************************************************************************************/
struct i2c {
    enum io io[2];              /* I2C data and lock I/O pin configuration code                 */
    const enum io_fun exp[2];   /* Expected peripheral function code                            */
};

/**************************************************************************************************
 * 
 * Private function prototypes. Functions are implemented in driver and linked to \b net object.
 * 
 *************************************************************************************************/

static int i2c__open(const struct net *net, const struct i2c_usr *usr);
static int i2c__close(const struct net *net);
static int i2c__read(const struct net *net, struct i2c_f *f, unsigned length);
static int i2c__write(const struct net *net, struct i2c_f *f, unsigned length);

/**************************************************************************************************
 * 
 * \brief Wrapper for \e net_open() function
 * 
 *************************************************************************************************/
extern int
i2c_open(const struct net *net, const struct i2c_usr *usr);

/**************************************************************************************************
 * 
 * \brief Wrapper for \e net_close() function
 * 
 *************************************************************************************************/
extern int
i2c_close(const struct net *net);

/**************************************************************************************************
 * 
 * \brief Wrapper for \e net_read() function
 * 
 *************************************************************************************************/
extern int
i2c_read(const struct net *net, struct i2c_f *f, unsigned length);

/**************************************************************************************************
 * 
 * \brief Wrapper for \e net_write() function
 * 
 *************************************************************************************************/
extern int
i2c_write(const struct net *net, struct i2c_f *f, unsigned length);

/**************************************************************************************************
 * 
 * \brief Wrapper for \e net_connect() function
 * 
 * \param sda data I/O pin configuration code
 * \param scl clock I/O pin configuration code
 * 
 *************************************************************************************************/
extern int
i2c_connect(const struct net *net, enum io sda, enum io scl);

/**************************************************************************************************
 * 
 * \brief Wrapper for \e net_disconnect() function
 * 
 *************************************************************************************************/
extern int
i2c_disconnect(const struct net *net);

#endif /* _INC_NET_I2C_H */
