/**************************************************************************************************
 * 
 * \file can.c
 * 
 * \brief CAN communication interface implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "inc/net/can.h"

#include "inc/lib/debug.h"
#include "inc/lib/utils.h"

#include <stdint.h>
#include <stdbool.h>

/**************************************************************************************************
 * 
 * can_open()
 * 
 *************************************************************************************************/
int
can_open(const struct net *net, const struct can_usr *usr)
{
    if (usr && (usr->bitrate > 0U)) {
        return net_open(net, usr, NET_CAN);
    } else {
        return -1;
    }
}

/**************************************************************************************************
 * 
 * can_close()
 * 
 *************************************************************************************************/
int
can_close(const struct net *net)
{
    return net_close(net, NET_CAN);
}

/**************************************************************************************************
 * 
 * can_read()
 * 
 *************************************************************************************************/
int
can_read(const struct net *net, struct can_f *f)
{
    return net_read(net, f, 1U, NET_CAN);
}

/**************************************************************************************************
 * 
 * can_write()
 * 
 *************************************************************************************************/
int
can_write(const struct net *net, struct can_f *f)
{
    return net_write(net, f, 1U, NET_CAN);
}

/**************************************************************************************************
 * 
 * can_connect()
 * 
 *************************************************************************************************/
int
can_connect(const struct net *net, enum io tx, enum io rx)
{
    const struct can *can = net->drv;

    /** Break \e const qualifier */
    enum io *io = (enum io *) can->io;

    io[0] = tx;
    io[1] = rx;

    if (can) {
        return net_connect(net, io, can->exp, 2U, NET_CAN);
    } else {
        return -1;
    }
}

/**************************************************************************************************
 * 
 * can_disconnect()
 * 
 *************************************************************************************************/
int
can_disconnect(const struct net *net)
{
    const struct can *can = net->drv;

    /** Break \e const qualifier */
    enum io *io = (enum io *) can->io;

    if (can) {
        return net_disconnect(net, io, 2U, NET_CAN);
    } else {
        return -1;
    }
}

/**************************************************************************************************
 * 
 * can_setup()
 * 
 *************************************************************************************************/
int
can_setup(const struct net *net, const struct can_cfg *cfg)
{
    if (!net_check(net, NET_CAN)) {
        return -1;
    }

    NET_PRIV_STRUCT(net, );

    struct net_priv *priv = (struct net_priv *) net->priv;
    const struct can *can = net->drv;

    if (priv->open && can && can->setup && cfg) {
        return can->setup(net, cfg);
    } else {
        return -1;
    }
}

/**************************************************************************************************
 * 
 * can_get()
 * 
 *************************************************************************************************/
int
can_get(const struct net *net, struct can_f *f)
{
    if (!net_check(net, NET_CAN)) {
        return -1;
    }

    NET_PRIV_STRUCT(net, );

    struct net_priv *priv = (struct net_priv *) net->priv;
    const struct can *can = net->drv;

    if (priv->open && can && can->get) {
        return can->get(net, f);
    } else {
        return -1;
    }
}

/**************************************************************************************************
 * 
 * can_send()
 * 
 *************************************************************************************************/
__attribute__((ramfunc)) int
can_send(const struct net *net, struct can_f *f)
{
    if (!net_check(net, NET_CAN)) {
        return -1;
    }

    NET_PRIV_STRUCT(net, );

    struct net_priv *priv = (struct net_priv *) net->priv;
    const struct can *can = net->drv;

    if (priv->open && can && can->send) {
        return can->send(net, f);
    } else {
        return -1;
    }
}

/**************************************************************************************************
 * 
 * can_status()
 * 
 *************************************************************************************************/
int
can_status(const struct net *net)
{
    if (!net_check(net, NET_CAN)) {
        return -1;
    }

    NET_PRIV_STRUCT(net, );

    struct net_priv *priv = (struct net_priv *) net->priv;
    const struct can *can = net->drv;

    if (priv->open && can && can->status) {
        return can->status(net);
    } else {
        return -1;
    }
}

/**************************************************************************************************
 * 
 * can_bit_timing()
 * 
 *************************************************************************************************/
int
can_bit_timing(const struct can_btl *btl, struct can_btp *btp, uint32_t clk, uint32_t bitrate)
{
    ASSERT(btl && btp);
    ASSERT(clk > 0U);
    ASSERT(bitrate > 0U);

    const uint16_t tSync = 1U;
    const uint16_t tProp = 2U; /** TODO: Parametrize via cable length! */

    bool initialized = false;
    uint32_t bitrate_error_max;

    /* Find best performing number of quants considering baud rate error */
    uint16_t nq;
    for (nq = btl->nq[0]; nq <= btl->nq[1]; nq++) {
        /* Baud rate prescaler rounded to nearest integer */
        uint32_t brp = div_round_u32(clk, (nq * bitrate));

        if ((brp < btl->brp[0]) || (brp > btl->brp[1])) {
            continue;
        }

        /* Absolute bitrate error */
        uint32_t bitrate_actual = div_round_u32(clk, (nq * brp));
        uint32_t bitrate_error = (bitrate >= bitrate_actual) ?
            (bitrate - bitrate_actual) : (bitrate_actual - bitrate);

        if (initialized && (bitrate_error >= bitrate_error_max)) {
            continue;
        }
        
        uint16_t tSeg = nq - (tSync + tProp);

        uint16_t tSeg2 = tSeg / 2U;
        if (tSeg2 < btl->tSeg2[0]) {
            tSeg2 = btl->tSeg2[0];
        } else if (tSeg2 > btl->tSeg2[1]) {
            tSeg2 = btl->tSeg2[1];
        }
        
        uint16_t tSeg1 = (tSeg - tSeg2) + tProp;
        if ((tSeg1 < btl->tSeg1[0]) || (tSeg1 > btl->tSeg1[1])) {
            continue;
        }

        uint16_t sjw = tSeg2;
        if (sjw < btl->sjw[0]) {
            sjw = btl->sjw[0];
        } else if (sjw > btl->sjw[1]) {
            sjw = btl->sjw[1];
        }

        btp->nq = nq;
        btp->brp = brp;
        btp->tProp = tProp;
        btp->tSeg1 = tSeg1;
        btp->tSeg2 = tSeg2;
        btp->sjw = sjw;

        initialized = true;
        bitrate_error_max = bitrate_error;

        btp->sp = div_round_u32((1U + tSeg1) * 100U, nq);
        btp->err = div_round_u32(bitrate_error_max * 100U, bitrate);

        if (bitrate_error == 0U) {
            break;
        }
    }

    if (initialized) {
        return (bitrate_error_max == 0U) ? 0 : 1;
    } else { /* !initialized */
        return -1;
    }
}
