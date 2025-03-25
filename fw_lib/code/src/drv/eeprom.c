/**************************************************************************************************
 * 
 * \file eeprom.c
 * 
 * \brief EEPROM driver implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "inc/drv/eeprom.h"

#include "inc/lib/debug.h"
#include "inc/lib/mem.h"
#include "inc/net/eeprom.h"

#ifndef VIRTUAL
#include "inc/net/i2c.h"
#else
#include "inc/net/file.h"
#endif

#include <stddef.h>

/**************************************************************************************************
 * 
 * Private structure definition
 * 
 *************************************************************************************************/
NET_PRIV_STRUCT(eeprom,
    const struct net *drv;          /* Communication driver handler                             */
    enum net_type drv_t;            /* Communication driver type                                */
    const struct mem *mem;          /* Memory object handler                                    */
);

/**************************************************************************************************
 * 
 * eeprom_new()
 * 
 *************************************************************************************************/
const struct net *
eeprom_new(const struct net *drv)
{
    ASSERT(drv);
    
    NET_PRIV_NEW(eeprom, NET_EEPROM,
        .drv   = NULL,
        .drv_t = NET_NONE,
        .mem   = NULL
    );

    priv.drv = drv;
    priv.drv_t = net_get_type(drv);

    NET_OBJ_NEW(eeprom, NULL);

    /**
     * Although EEPROM does not connect to I/O pins, the \e connect flag must be set to \e true.
     * Otherwise, the \e eeprom_open() function will fail.
     */
    priv.connect = true;

    return &net;
}

/**************************************************************************************************
 * 
 * \brief Reads or writes to external EEPROM
 * 
 * \param net net object handler
 * \param f transaction frame
 * \param rw_ \e true for read and \e false for write operation
 * 
 * \return 1 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
static int
eeprom__rw(const struct net *net, const struct eeprom_f *f, bool rw_)
{
    struct eeprom_priv *priv = net->priv;
    const struct mem *mem = priv->mem;
    
    if (!mem_check(mem, f->address, f->length, rw_)) {
        return -1;
    }

    int ret;

    switch (priv->drv_t) {
#ifndef VIRTUAL
    case NET_I2C:
    {
        uint8_t memory_address[2];
        struct i2c_f i2c_f[2];

        uint16_t slave_address = mem->address >> 1;

        /* Address MSB is transmitted first */
        memory_address[0] = (uint8_t) ((f->address >> 8) & 0x00FFU);
        memory_address[1] = (uint8_t) ((f->address >> 0) & 0x00FFU);

        /* First write start address to EEPROM */
        i2c_f[0] = (struct i2c_f) {
            .rw_      = false, /* Write operation */
            .start    = false,
            .start_us = 0U,
            .address  = slave_address,
            .length   = 2U,
            .data     = memory_address
        };

        /* Disable repeated start condition for write operation */
        i2c_f[1] = (struct i2c_f) {
            .rw_      = rw_, /* Read or write operation */
            .start    = !rw_,
            .start_us = 0U,
            .address  = slave_address,
            .length   = f->length,
            .data     = f->data
        };

        ret = i2c_write(priv->drv, &i2c_f[0], 2U);
        if (ret != 2) {
            return -1;
        }
        
        /* Acknowledge polling after write operation */
        if (!rw_) {
            i2c_f[0].start_us = mem->wait;

            ret = i2c_write(priv->drv, &i2c_f[0], 1U);
            if (ret != 1) {
                return -1;
            }
        }

        break;
    }
#else
    case NET_FILE:
    {
        struct file_f file_f = {
            .address = f->address,
            .length  = f->length,
            .data    = f->data
        };

        if (rw_) {
            ret = file_read(priv->drv, &file_f);
        } else { /* !rw_ */
            ret = file_write(priv->drv, &file_f);
        }

        if (ret != 1) {
            return -1;
        }

        break;
    }
#endif
    default:
        return -1;
    }

    return 1;
}

/**************************************************************************************************
 * 
 * Net layer implementation. Private functions linked to \b net object.
 * 
 *************************************************************************************************/

/**************************************************************************************************
 * 
 * eeprom__open()
 * 
 *************************************************************************************************/
static int
eeprom__open(const struct net *net, const struct eeprom_usr *usr)
{
    struct eeprom_priv *priv = net->priv;
    const struct mem *mem = usr->mem;

    priv->mem = mem;
    
    int ret;

    switch (priv->drv_t) {
#ifndef VIRTUAL
    case NET_I2C:
    {
        struct i2c_usr i2c_usr = {
            .bitrate  = mem->bitrate,
            .bitcount = 8U
        };

        ret = i2c_open(priv->drv, &i2c_usr);
        break;
    }
#else
    case NET_FILE:
    {
        struct file_usr file_usr = {
            .path = mem->fpath
        };

        ret = file_open(priv->drv, &file_usr);
        break;
    }
#endif
    default:
        ret = -1;
        break;
    }

    priv->open = (ret >= 0);
    
    return (ret < 0) ? -1 : 0;
}

/**************************************************************************************************
 * 
 * eeprom__close()
 * 
 *************************************************************************************************/
static int
eeprom__close(const struct net *net)
{
    struct eeprom_priv *priv = net->priv;

    priv->open = false;

    int ret;

    switch (priv->drv_t) {
#ifndef VIRTUAL
    case NET_I2C:
        ret = i2c_close(priv->drv);
        break;
#else
    case NET_FILE:
        ret = file_close(priv->drv);
        break;
#endif
    default:
        ret = -1;
        break;
    }

    return (ret < 0) ? -1 : 0;
}

/**************************************************************************************************
 * 
 * eeprom__read()
 * 
 *************************************************************************************************/
static int
eeprom__read(const struct net *net, struct eeprom_f *f, unsigned length)
{
    return eeprom__rw(net, f, true);
}

/**************************************************************************************************
 * 
 * eeprom__write()
 * 
 *************************************************************************************************/
static int
eeprom__write(const struct net *net, struct eeprom_f *f, unsigned length)
{
    return eeprom__rw(net, f, false);
}
