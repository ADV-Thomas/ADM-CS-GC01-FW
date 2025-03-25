/**************************************************************************************************
 *
 * \file spi.h
 *
 * \brief SPI communication interface
 *
 * \author Jorge Sola
 *
 *************************************************************************************************/

#ifndef _INC_NET_SPI_H
#define _INC_NET_SPI_H

#include "inc/net/net.h"
#include "inc/drv/io.h"
#include "driverlib/spi.h"

#include <stdint.h>
#include <stdbool.h>

/**************************************************************************************************
 *
 * User configuration data definition
 *
 *************************************************************************************************/
struct spi_usr {
    uint32_t bitrate;  /* Communication speed (bps)                                    */
    uint16_t bitcount; /* Number of bits per data byte (1..8)                          */
};

/**************************************************************************************************
 *
 * Transaction frame definition
 *
 *************************************************************************************************/
struct spi_f {
    uint16_t length; /* Data buffer length                                           */
    uint8_t *data;   /* Data buffer                                                  */
};

/**************************************************************************************************
 *
 * SPI object definition
 *
 *************************************************************************************************/
struct spi {
    enum io io[4];            /* SPI data and lock I/O pin configuration code                 */
    const enum io_fun exp[4]; /* Expected peripheral function code                            */
};

/**************************************************************************************************
 *
 * Private function prototypes. Functions are implemented in driver and linked to \b net object.
 *
 *************************************************************************************************/

static int spi__open(const struct net *net, const struct spi_usr *usr);
static int spi__close(const struct net *net);
static int spi__read(const struct net *net, struct spi_f *f, unsigned length);
static int spi__write(const struct net *net, struct spi_f *f, unsigned length);

/**************************************************************************************************
 *
 * \brief Wrapper for \e net_open() function
 *
 *************************************************************************************************/
extern int spi_open(const struct net *net, const struct spi_usr *usr);

/**************************************************************************************************
 *
 * \brief Wrapper for \e net_close() function
 *
 *************************************************************************************************/
extern int spi_close(const struct net *net);

/**************************************************************************************************
 *
 * \brief Wrapper for \e net_read() function
 *
 *************************************************************************************************/
extern int spi_read(const struct net *net, struct spi_f *f, unsigned length);

/**************************************************************************************************
 *
 * \brief Wrapper for \e net_write() function
 *
 *************************************************************************************************/
extern int spi_write(const struct net *net, struct spi_f *f, unsigned length);

/**************************************************************************************************
 *
 * \brief Wrapper for \e net_connect() function
 *
 * \param sda data I/O pin configuration code
 * \param scl clock I/O pin configuration code
 *
 *************************************************************************************************/
extern int spi_connect(const struct net *net, enum io clk, enum io simo, enum io somi, enum io ste);

/**************************************************************************************************
 *
 * \brief Wrapper for \e net_disconnect() function
 *
 *************************************************************************************************/
extern int spi_disconnect(const struct net *net);

#endif /* _INC_NET_SPI_H */
