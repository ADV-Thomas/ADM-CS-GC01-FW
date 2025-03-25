/**************************************************************************************************
 *
 * \file fsi.c
 *
 * \brief FSI communication interface implementation
 *
 * \author Jorge Sola
 *
 *************************************************************************************************/

#include "inc/net/fsi.h"

/**************************************************************************************************
 *
 * fsi_open()
 *
 *************************************************************************************************/
int fsi_open(const struct net *net, const struct fsi_usr *usr)
{
    if (usr && (usr->clk_freq > 0U)) {
        return net_open(net, usr, NET_FSI);
    } else {
        return -1;
    }
}

/**************************************************************************************************
 *
 * fsi_close()
 *
 *************************************************************************************************/
int fsi_close(const struct net *net)
{
    return net_close(net, NET_FSI);
}

/**************************************************************************************************
 *
 * fsi_read()
 *
 *************************************************************************************************/
int fsi_read(const struct net *net, struct fsi_f *f)
{
    return net_read(net, f, 1U, NET_FSI);
}

/**************************************************************************************************
 *
 * fsi_write()
 *
 *************************************************************************************************/
int fsi_write(const struct net *net, struct fsi_f *f)
{
    return net_write(net, f, 1U, NET_FSI);
}

/**************************************************************************************************
 *
 * fsi_connect()
 *
 *************************************************************************************************/
int fsi_connect(const struct net *net, enum io tx_clk, enum io tx_d0, enum io rx_clk, enum io rx_d0)
{
    const struct fsi_drv *fsi = net->drv;

    /** Break \e const qualifier */
    enum io *io = (enum io *)fsi->io;

    io[0] = tx_clk;
    io[1] = tx_d0;
    io[2] = rx_clk;
    io[3] = rx_d0;

    if (fsi) {
        return net_connect(net, io, fsi->exp, 4U, NET_FSI);
    } else {
        return -1;
    }
}

/**************************************************************************************************
 *
 * fsi_disconnect()
 *
 *************************************************************************************************/
int fsi_disconnect(const struct net *net)
{
    const struct fsi_drv *fsi = net->drv;

    /** Break \e const qualifier */
    enum io *io = (enum io *)fsi->io;

    if (fsi) {
        return net_disconnect(net, io, 4U, NET_FSI);
    } else {
        return -1;
    }
}

/**************************************************************************************************
 * 
 * can_send()
 * 
 *************************************************************************************************/
int
fsi_run(const struct net *net)
{
    if (!net_check(net, NET_FSI)) {
        return -1;
    }

    NET_PRIV_STRUCT(net, );

    struct net_priv *priv = (struct net_priv *) net->priv;
    const struct fsi_drv *fsi = net->drv;

    if (priv->open && fsi && fsi->run) {
        return fsi->run(net);
    } else {
        return -1;
    }
}
