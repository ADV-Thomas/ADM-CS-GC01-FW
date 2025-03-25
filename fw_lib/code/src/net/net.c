/**************************************************************************************************
 * 
 * \file net.c
 * 
 * \brief Interface implementation for different communication protocols
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "inc/net/net.h"

#include <stdint.h>

/**************************************************************************************************
 * 
 * Generic private structure definition
 * 
 *************************************************************************************************/
NET_PRIV_STRUCT(net, );

/**************************************************************************************************
 * 
 * net_check()
 * 
 *************************************************************************************************/
bool
net_check(const struct net *net, enum net_type type)
{
    const struct net_priv *priv = net->priv;

    return (net && priv && (priv->type == type));
}

/**************************************************************************************************
 * 
 * net_get_type()
 * 
 *************************************************************************************************/
enum net_type
net_get_type(const struct net *net)
{
    const struct net_priv *priv = net->priv;

    return (net && priv) ? priv->type : NET_NONE;
}

/**************************************************************************************************
 * 
 * net_open()
 * 
 *************************************************************************************************/
int
net_open(const struct net *net, const void *usr, enum net_type type)
{
    if (!net_check(net, type)) {
        return -1;
    }

    const struct net_priv *priv = net->priv;

    if (priv->connect && net->open) {
        return net->open(net, usr);
    } else {
        return -1;
    }
}

/**************************************************************************************************
 * 
 * net_close()
 * 
 *************************************************************************************************/
int
net_close(const struct net *net, enum net_type type)
{
    if (!net_check(net, type)) {
        return -1;
    }

    if (net->close) {
        return net->close(net);
    } else {
        return -1;
    }
}

/**************************************************************************************************
 * 
 * net_read()
 * 
 *************************************************************************************************/
int
net_read(const struct net *net, void *f, unsigned length, enum net_type type)
{
    if (!net_check(net, type)) {
        return -1;
    }

    const struct net_priv *priv = net->priv;

    if (priv->open && net->read && f && (length > 0U)) {
        return net->read(net, f, length);
    } else {
        return -1;
    }
}

/**************************************************************************************************
 * 
 * net_write()
 * 
 *************************************************************************************************/
int
net_write(const struct net *net, void *f, unsigned length, enum net_type type)
{
    if (!net_check(net, type)) {
        return -1;
    }

    const struct net_priv *priv = net->priv;

    if (priv->open && net->write && f && (length > 0U)) {
        return net->write(net, f, length);
    } else {
        return -1;
    }
}

/**************************************************************************************************
 * 
 * net_connect()
 * 
 *************************************************************************************************/
int
net_connect(const struct net *net, enum io *io, const enum io_fun *exp, unsigned length,
    enum net_type type)
{
    if (!net_check(net, type)) {
        return -1;
    }

    struct net_priv *priv = net->priv;

    if (priv->connect || !io || (length == 0U)) {
        return -1;
    }

    priv->connect = true;

    int ret = 0;

    unsigned i;
    for (i = 0U; i < length; i++) {
        ret |= io_connect(io[i], (exp ? exp[i] : IO_NONE));
        if (ret < 0) {
            net_disconnect(net, io, length, type);
            break;
        }
    }

    return (ret < 0) ? -1 : 0;
}

/**************************************************************************************************
 * 
 * net_disconnect()
 * 
 *************************************************************************************************/
int
net_disconnect(const struct net *net, enum io *io, unsigned length, enum net_type type)
{
    if (!net_check(net, type)) {
        return -1;
    }

    struct net_priv *priv = net->priv;

    if (!priv->connect || !io || (length == 0U)) {
        return -1;
    }

    priv->connect = false;

    int ret = 0;

    unsigned i;
    for (i = 0U; i < length; i++) {
        ret |= io_disconnect(io[i]);
        io[i] = IOX;
    }

    return (ret < 0) ? -1 : 0;
}
