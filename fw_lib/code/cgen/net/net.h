/**************************************************************************************************
 * 
 * \file net.h
 * 
 * \brief Interface code generator for different communication protocols
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _CGEN_NET_NET_H
#define _CGEN_NET_NET_H

/**************************************************************************************************
 * 
 * \brief Provides definition for private structure
 * 
 * \param TYPE driver type
 * \param MEMBER_LIST zero or more structure members separated by semicolon
 * 
 *************************************************************************************************/
#define NET_PRIV_STRUCT(TYPE, MEMBER_LIST)                                                      \
    struct TYPE##_priv {                                                                        \
        const enum net_type type;                                                               \
        bool open;                                                                              \
        bool connect;                                                                           \
        MEMBER_LIST                                                                             \
    }

/**************************************************************************************************
 * 
 * \brief Creates and initializes private variables structure for \e net object
 * 
 * \param TYPE driver type
 * \param NET_TYPE communication protocol from 'enum net_type'
 * \param ... one or more initializers separated by comma
 * 
 *************************************************************************************************/
#define NET_PRIV_NEW(TYPE, NET_TYPE, ...)                                                       \
    static struct TYPE##_priv priv = {                                                          \
        .type    = NET_TYPE,                                                                    \
        .open    = false,                                                                       \
        .connect = false,                                                                       \
        __VA_ARGS__                                                                             \
    }

/**************************************************************************************************
 * 
 * \brief Creates and initializes net object
 * 
 * \param TYPE driver type
 * \param DRV pointer to driver object
 * 
 *************************************************************************************************/
#define NET_OBJ_NEW(TYPE, DRV)                                                                  \
    static const struct net net = {                                                             \
        .priv  = &priv,                                                                         \
        .drv   = DRV,                                                                           \
        .open  = (int (*)(const struct net *, const void *)) TYPE##__open,                      \
        .close = (int (*)(const struct net *)) TYPE##__close,                                   \
        .read  = (int (*)(const struct net *, void *, unsigned)) TYPE##__read,                  \
        .write = (int (*)(const struct net *, void *, unsigned)) TYPE##__write                  \
    }

#endif /* _CGEN_NET_NET_H */
