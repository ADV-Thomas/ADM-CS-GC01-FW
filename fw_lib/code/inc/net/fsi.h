/**************************************************************************************************
 *
 * \file fsi.h
 *
 * \brief FSI communication interface
 *
 * \author Jorge Sola
 *
 *************************************************************************************************/

#ifndef _INC_NET_FSI_H
#define _INC_NET_FSI_H

#include "inc/net/net.h"
#include "inc/drv/io.h"
#include "driverlib/fsi.h"

#include <stdint.h>
#include <stdbool.h>

/**************************************************************************************************
 *
 * User configuration data definition
 *
 *************************************************************************************************/
enum fsi_type{
    FSI_TYPE_MASTER = 0,
    FSI_TYPE_SLAVE,
};

struct fsi_usr {
    enum fsi_type type;
    uint32_t clk_freq; /* TX Clock frequency in Hz                          */
    uint16_t trg_src; /* Trigger source for Ping frame   */ 
};

/**************************************************************************************************
 *
 * Transaction frame definition
 *
 *************************************************************************************************/
struct fsi_f {
    uint16_t id;        /* ID of message        */
    uint16_t length;    /* Data buffer length   */
    uint16_t data[16];  /* Data buffer          */
};

/**************************************************************************************************
 *
 * FSI object definition
 *
 *************************************************************************************************/
struct fsi_drv {
    enum io io[4];            /* FSI data and lock I/O pin configuration code                 */
    const enum io_fun exp[4]; /* Expected peripheral function code                            */
    int (*run)(const struct net *net);
};

/**************************************************************************************************
 *
 * Private function prototypes. Functions are implemented in driver and linked to \b net object.
 *
 *************************************************************************************************/

static int fsi__open(const struct net *net, const struct fsi_usr *usr);
static int fsi__close(const struct net *net);
static int fsi__read(const struct net *net, struct fsi_f *f, unsigned length);
static int fsi__write(const struct net *net, struct fsi_f *f, unsigned length);

/**************************************************************************************************
 * 
 * Private function prototypes. Functions are implemented in driver and linked to \b can object.
 * 
 *************************************************************************************************/

static int fsi__run(const struct net *net);

/**************************************************************************************************
 *
 * \brief Wrapper for \e net_open() function
 *
 *************************************************************************************************/
extern int fsi_open(const struct net *net, const struct fsi_usr *usr);

/**************************************************************************************************
 *
 * \brief Wrapper for \e net_close() function
 *
 *************************************************************************************************/
extern int fsi_close(const struct net *net);

/**************************************************************************************************
 *
 * \brief Wrapper for \e net_read() function
 *
 *************************************************************************************************/
extern int fsi_read(const struct net *net, struct fsi_f *f);

/**************************************************************************************************
 *
 * \brief Wrapper for \e net_write() function
 *
 *************************************************************************************************/
extern int fsi_write(const struct net *net, struct fsi_f *f);


/**************************************************************************************************
 *
 * \brief Wrapper for \e net_connect() function
 *
 * \param tx_clk clock I/O pin configuration code
 * \param tx_d0 data I/O pin configuration code
 * \param rx_clk clock I/O pin configuration code
 * \param rx_d0 data I/O pin configuration code
 *
 *************************************************************************************************/
extern int fsi_connect(const struct net *net, enum io tx_clk, enum io tx_d0, enum io rx_clk, enum io rx_d0);

/**************************************************************************************************
 *
 * \brief Wrapper for \e net_disconnect() function
 *
 *************************************************************************************************/
extern int fsi_disconnect(const struct net *net);

/**************************************************************************************************
 *
 * \brief Wrapper for \e net_disconnect() function
 *
 *************************************************************************************************/
extern int fsi_run(const struct net *net);

#endif /* _INC_NET_FSI_H */
