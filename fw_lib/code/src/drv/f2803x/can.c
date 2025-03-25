/**************************************************************************************************
 * 
 * \file can.c
 * 
 * \brief CAN driver implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "inc/drv/can.h"

#include "driverlib/can.h"
#include "driverlib/sysctl.h"

#include "inc/drv/clk.h"
#include "inc/net/can.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**************************************************************************************************
 * 
 * Defines for the driver
 * 
 *************************************************************************************************/

#define CAN_BITRATE_MAX     1000000UL       /* Maximum communication speed (bps)                */

#define CAN_MAILBOX_MAX     32U             /* Total number of mailboxes                        */

#define CAN_MAILBOX_RX1     1U              /* First regular receive mailbox                    */
#define CAN_MAILBOX_RXN     20U             /* Last regular receive mailbox                     */

#define CAN_MAILBOX_TX1     21U             /* First regular transmit mailbox                   */
#define CAN_MAILBOX_TXN     30U             /* Last regular transmit mailbox                    */

#define CAN_MAILBOX_RXL     31U             /* Low-latency receive mailbox                      */
#define CAN_MAILBOX_TXL     32U             /* Low-latency transmit mailbox                     */

/**************************************************************************************************
 * 
 * Private structure definition
 * 
 *************************************************************************************************/
NET_PRIV_STRUCT(can,
    const SysCtl_PeripheralPCLOCKCR periph; /* CAN peripheral clock                             */
    const uint32_t base;        /* CAN module base register address                             */
    bool trig;                  /* True when low-latency message send has been triggered        */
);

/**************************************************************************************************
 * 
 * Bit timing configuration parameters limit values
 * 
 *************************************************************************************************/
static const struct can_btl can_btl = {
    .nq    = { 8U,  25U },      /* Number of quants                                             */
    .brp   = { 1U, 256U },      /* Baud rate prescaler                                          */
    .tSeg1 = { 2U,  16U },      /* Time segment before the sample point                         */
    .tSeg2 = { 2U,   8U },      /* Time segment after the sample point                          */
    .sjw   = { 1U,   4U }       /* Synchronization jump width                                   */
};

/**************************************************************************************************
 * 
 * \brief Resets CAN module to default (on-reset) configuration
 * 
 * \param net net object handler
 * 
 * \return None
 * 
 *************************************************************************************************/
static void
can__reset(const struct net *net)
{
    struct can_priv *priv = net->priv;

    /** TODO: Return CAN module to default-on-reset state */

    SysCtl_disablePeripheral(priv->periph);
}

/**************************************************************************************************
 * 
 * can_a_new()
 * 
 *************************************************************************************************/
const struct net *
can_a_new(void)
{
    NET_PRIV_NEW(can, NET_CAN,
        .periph = SYSCTL_PERIPH_CLK_ECANA,
        .base   = CANA_BASE,
        .trig   = false
    );

    static struct can can = {
        .io     = { IOX, IOX },
        .exp    = { IO_CANA_TX, IO_CANA_RX },
        .setup  = can__setup,
        .get    = can__get,
        .send   = can__send,
        .status = can__status
    };

    NET_OBJ_NEW(can, &can);

    can__reset(&net);

    return &net;
}

/**************************************************************************************************
 * 
 * \brief Reads data from mailbox
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * \param f transaction frame
 * 
 * \return \e true when data has been read from the mailbox
 * 
 *************************************************************************************************/
static bool
can__mailbox_read(uint32_t base, uint16_t mbx, struct can_f *f)
{
    if (!CAN_getReceivedMessagePending(base, mbx)) {
        return false;
    }

    CAN_IdentifierMode mode = CAN_getIdentifierMode(base, mbx);
    bool rtrFlag = CAN_getRemoteTransmissionRequest(base, mbx);

    if ((mode != CAN_ID_MODE_EXTENDED) || rtrFlag) {
        CAN_clearReceivedMessagePending(base, mbx);
        return false;
    }

    if (f) {
        f->id = CAN_getMessageIdentifier(base, mbx);

        f->length = CAN_getMessageDataLength(base, mbx);

        uint16_t data[8];

        CAN_readData(base, mbx, data, f->length);

        unsigned i;
        for (i = 0U; i < f->length; i++) {
            f->data[i] = (uint8_t) data[i];
        }
    }

    CAN_clearReceivedMessagePending(base, mbx);

    return true;
}

/**************************************************************************************************
 * 
 * \brief Writes data to mailbox
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * \param f transaction frame
 * 
 * \return \e true when data has been written to the mailbox
 * 
 *************************************************************************************************/
static bool
can__mailbox_write(uint32_t base, uint16_t mbx, const struct can_f *f)
{
    if (CAN_getTransmissionRequest(base, mbx)) {
        return false;
    }

    if (f) {
        CAN_disableMailbox(base, mbx);
        CAN_setMessageIdentifier(base, mbx, f->id);

        if (f->length > 0U) {
            CAN_setMessageDataLength(base, mbx, f->length);

            uint16_t data[8];

            unsigned i;
            for (i = 0U; i < f->length; i++) {
                data[i] = (uint16_t) f->data[i];
            }

            CAN_writeData(base, mbx, data, f->length);
        } else {
            CAN_setMessageDataLength(base, mbx, 0U);
        }
    }

    CAN_enableMailbox(base, mbx);

    CAN_setTransmissionRequest(base, mbx);

    return true;
}

/**************************************************************************************************
 * 
 * Net layer implementation. Private functions linked to \b net object.
 * 
 *************************************************************************************************/

/**************************************************************************************************
 * 
 * can__open()
 * 
 *************************************************************************************************/
static int
can__open(const struct net *net, const struct can_usr *usr)
{
    if (usr->bitrate > CAN_BITRATE_MAX) {
        return -1;
    }

    struct can_priv *priv = net->priv;

    const struct _clk *_clk = clk_get();
    ASSERT(_clk);

    int ret;
    struct can_btp btp;

    ret = can_bit_timing(&can_btl, &btp, _clk->can.Hz, usr->bitrate);
    if (ret < 0) {
        return -1;
    }

    SysCtl_enablePeripheral(priv->periph);

    /* Configuration registers cannot be modified before peripheral clock has been enabled! */
    asm(" RPT #2 || NOP");

    CAN_resetModule(priv->base);

    CAN_setControllerMode(priv->base, CAN_MODE_ENHANCED);

    CAN_setBitTiming(priv->base, (btp.brp - 1U), (btp.tSeg1 - 1U),
        (btp.tSeg2 - 1U), (btp.sjw - 1U));
    
    CAN_enableAutoBusOn(priv->base);

    CAN_enableTxPin(priv->base);
    CAN_enableRxPin(priv->base);

    unsigned mbx;
    for (mbx = 1U; mbx <= CAN_MAILBOX_MAX; mbx++) {
        CAN_disableMailbox(priv->base, mbx);
    }

    /* CAN driver must be opened before configuring mailboxes */
    priv->open = true;

    struct can_cfg cfg = {
        .mbx    = 0x3FFFFFFFUL,
        .enable = true,
        .id     = usr->id,
        .mask   = usr->mask,
        .f      = NULL
    };

    /* Setup regular receive and transmit mailboxes */
    can_setup(net, &cfg);

    return 0;
}

/**************************************************************************************************
 * 
 * can__close()
 * 
 *************************************************************************************************/
static int
can__close(const struct net *net)
{
    struct can_priv *priv = net->priv;

    priv->open = false;
    
    can__reset(net);

    return 0;
}

/**************************************************************************************************
 * 
 * can__read()
 * 
 *************************************************************************************************/
static int
can__read(const struct net *net, struct can_f *f, unsigned length)
{
    struct can_priv *priv = net->priv;

    unsigned mbx;
    for (mbx = CAN_MAILBOX_RXN; mbx >= CAN_MAILBOX_RX1; mbx--) {
        if (can__mailbox_read(priv->base, mbx, f)) {
            return 1;
        }
    }

    return 0;
}

/**************************************************************************************************
 * 
 * can__write()
 * 
 *************************************************************************************************/
static int
can__write(const struct net *net, struct can_f *f, unsigned length)
{
    struct can_priv *priv = net->priv;

    unsigned mbx;
    for (mbx = CAN_MAILBOX_TXN; mbx >= CAN_MAILBOX_TX1; mbx--) {
        if (can__mailbox_write(priv->base, mbx, f)) {
            return 1;
        }
    }
    
    return 0;
}

/**************************************************************************************************
 * 
 * CAN layer implementation. Private functions linked to \b can object.
 * 
 *************************************************************************************************/

/**************************************************************************************************
 * 
 * can__setup()
 * 
 *************************************************************************************************/
static int
can__setup(const struct net *net, const struct can_cfg *cfg)
{
    struct can_priv *priv = net->priv;

    unsigned mbx;
    for (mbx = 1U; mbx <= CAN_MAILBOX_MAX; mbx++) {
        uint32_t mbx_mask = 1UL << (mbx - 1U);

        if ((cfg->mbx & mbx_mask) == 0) {
            continue;
        }
        
        CAN_disableMailbox(priv->base, mbx);

        if (!cfg->enable) {
            continue;
        }

        /** TODO: Think about having standard identifier for low-latency mailboxes! */

        if (((mbx >= CAN_MAILBOX_RX1) && (mbx <= CAN_MAILBOX_RXN)) || (mbx == CAN_MAILBOX_RXL)) {
            CAN_setMailboxDirection(priv->base, mbx, CAN_DIR_RECEIVE);

            CAN_setIdentifierMode(priv->base, mbx, CAN_ID_MODE_EXTENDED);
            CAN_setMessageIdentifier(priv->base, mbx, cfg->id);
            CAN_setAnswerMode(priv->base, mbx, CAN_ANSWER_MODE_NORMAL);

            CAN_enableAcceptanceMask(priv->base, mbx);
            CAN_setLocalAcceptance(priv->base, mbx, ~cfg->mask, false);

            CAN_enableOverwriteProtection(priv->base, mbx);
            
            CAN_clearReceivedMessagePending(priv->base, mbx);

            CAN_enableMailbox(priv->base, mbx);
        } else {
            CAN_setMailboxDirection(priv->base, mbx, CAN_DIR_TRANSMIT);

            CAN_setIdentifierMode(priv->base, mbx, CAN_ID_MODE_EXTENDED);
            CAN_setMessageIdentifier(priv->base, mbx, 0UL);

            if (cfg->f && (mbx == CAN_MAILBOX_TXL)) {
                CAN_setMessageIdentifier(priv->base, mbx, cfg->f->id);
                CAN_setMessageDataLength(priv->base, mbx, cfg->f->length);

                uint16_t data[8];

                unsigned i;
                for (i = 0U; i < cfg->f->length; i++) {
                    data[i] = cfg->f->data[i];
                }

                CAN_writeData(priv->base, mbx, data, cfg->f->length);
            }

            CAN_setAnswerMode(priv->base, mbx, CAN_ANSWER_MODE_NORMAL);

            CAN_disableAcceptanceMask(priv->base, mbx);

            CAN_setTransmitPriorityLevel(priv->base, mbx, 0U);

            CAN_clearRemoteTransmissionRequest(priv->base, mbx);
        }
    }

    return 0;
}

/**************************************************************************************************
 * 
 * can__get()
 * 
 *************************************************************************************************/
__attribute__((ramfunc)) static int
can__get(const struct net *net, struct can_f *f)
{
    struct can_priv *priv = net->priv;

    if (can__mailbox_read(priv->base, CAN_MAILBOX_RXL, f)) {
        return 1;
    }

    return 0;
}

/**************************************************************************************************
 * 
 * can__send()
 * 
 *************************************************************************************************/
__attribute__((ramfunc)) static int
can__send(const struct net *net, struct can_f *f)
{
    struct can_priv *priv = net->priv;

    if (can__mailbox_write(priv->base, CAN_MAILBOX_TXL, f)) {
        priv->trig = true;
        return 1;
    }

    return 0;
}

/**************************************************************************************************
 * 
 * can__status()
 * 
 *************************************************************************************************/
__attribute__((ramfunc)) static int
can__status(const struct net *net)
{
    struct can_priv *priv = net->priv;

    bool trs = CAN_getTransmissionRequest(priv->base, CAN_MAILBOX_TXL);

    if (priv->trig) {
        priv->trig = trs;
        return trs ? 0 : 1;
    } else { /* !priv->trig */
        return trs ? -1 : 2;
    }
}
