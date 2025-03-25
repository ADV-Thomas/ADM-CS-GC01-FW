/**************************************************************************************************
 *
 * \file spi.c
 *
 * \brief SPI communication interface implementation
 *
 * \author Jorge Sola
 *
 *************************************************************************************************/

#include "inc/net/spi.h"

/**************************************************************************************************
 *
 * spi_open()
 *
 *************************************************************************************************/
int spi_open(const struct net *net, const struct spi_usr *usr)
{
    if (usr && (usr->bitrate > 0U) && (usr->bitcount > 0U)) {
        return net_open(net, usr, NET_SPI);
    } else {
        return -1;
    }
}

/**************************************************************************************************
 *
 * spi_close()
 *
 *************************************************************************************************/
int spi_close(const struct net *net)
{
    return net_close(net, NET_SPI);
}

/**************************************************************************************************
 *
 * spi_read()
 *
 *************************************************************************************************/
int spi_read(const struct net *net, struct spi_f *f, unsigned length)
{
    return net_read(net, f, length, NET_SPI);
}

/**************************************************************************************************
 *
 * spi_write()
 *
 *************************************************************************************************/
int spi_write(const struct net *net, struct spi_f *f, unsigned length)
{
    return net_write(net, f, length, NET_SPI);
}

/**************************************************************************************************
 *
 * spi_connect()
 *
 *************************************************************************************************/
int spi_connect(const struct net *net, enum io clk, enum io simo, enum io somi, enum io ste)
{
    const struct spi *spi = net->drv;

    /** Break \e const qualifier */
    enum io *io = (enum io *)spi->io;

    io[0] = clk;
    io[1] = simo;
    io[2] = somi;
    io[3] = ste;

    if (spi) {
        return net_connect(net, io, spi->exp, 4U, NET_SPI);
    } else {
        return -1;
    }
}

/**************************************************************************************************
 *
 * spi_disconnect()
 *
 *************************************************************************************************/
int spi_disconnect(const struct net *net)
{
    const struct spi *spi = net->drv;

    /** Break \e const qualifier */
    enum io *io = (enum io *)spi->io;

    if (spi) {
        return net_disconnect(net, io, 4U, NET_SPI);
    } else {
        return -1;
    }
}
