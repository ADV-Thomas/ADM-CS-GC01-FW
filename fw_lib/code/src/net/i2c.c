/**************************************************************************************************
 * 
 * \file i2c.c
 * 
 * \brief I2C communication interface implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "inc/net/i2c.h"

/**************************************************************************************************
 * 
 * i2c_open()
 * 
 *************************************************************************************************/
int
i2c_open(const struct net *net, const struct i2c_usr *usr)
{
    if (usr && (usr->bitrate > 0U) && (usr->bitcount > 0U)) {
        return net_open(net, usr, NET_I2C);
    } else {
        return -1;
    }
}

/**************************************************************************************************
 * 
 * i2c_close()
 * 
 *************************************************************************************************/
int
i2c_close(const struct net *net)
{
    return net_close(net, NET_I2C);
}

/**************************************************************************************************
 * 
 * i2c_read()
 * 
 *************************************************************************************************/
int
i2c_read(const struct net *net, struct i2c_f *f, unsigned length)
{
    return net_read(net, f, length, NET_I2C);
}

/**************************************************************************************************
 * 
 * i2c_write()
 * 
 *************************************************************************************************/
int
i2c_write(const struct net *net, struct i2c_f *f, unsigned length)
{
    return net_write(net, f, length, NET_I2C);
}

/**************************************************************************************************
 * 
 * i2c_connect()
 * 
 *************************************************************************************************/
int
i2c_connect(const struct net *net, enum io sda, enum io scl)
{
    const struct i2c *i2c = net->drv;

    /** Break \e const qualifier */
    enum io *io = (enum io *) i2c->io;

    io[0] = sda;
    io[1] = scl;

    if (i2c) {
        return net_connect(net, io, i2c->exp, 2U, NET_I2C);
    } else {
        return -1;
    }
}

/**************************************************************************************************
 * 
 * i2c_disconnect()
 * 
 *************************************************************************************************/
int
i2c_disconnect(const struct net *net)
{
    const struct i2c *i2c = net->drv;

    /** Break \e const qualifier */
    enum io *io = (enum io *) i2c->io;

    if (i2c) {
        return net_disconnect(net, io, 2U, NET_I2C);
    } else {
        return -1;
    }
}
