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

#define CAN_MAILBOX_TXL     1U              /* Low-latency transmit mailbox                     */
#define CAN_MAILBOX_RXL     2U              /* Low-latency receive mailbox                      */

#define CAN_MAILBOX_TX1     3U              /* First regular transmit mailbox                   */
#define CAN_MAILBOX_TXN     12U             /* Last regular transmit mailbox                    */

#define CAN_MAILBOX_RX1     13U             /* First regular receive mailbox                    */
#define CAN_MAILBOX_RXN     32U             /* Last regular receive mailbox                     */

/**************************************************************************************************
 * 
 * Private structure definition
 * 
 *************************************************************************************************/
NET_PRIV_STRUCT(can,
    const SysCtl_PeripheralPCLOCKCR periph; /* CAN peripheral clock                             */
    const uint32_t base;        /* CAN module base register address                             */
    bool trig;                  /* True when low-latency message send has been triggered        */
    bool semaphore;             /* Semaphore for concurrence when can_get() is called in ISR, as 
    it shares same IF registers as can_read    */
);

/**************************************************************************************************
 * 
 * Bit timing configuration parameters limit values
 * 
 *************************************************************************************************/
static const struct can_btl can_btl = {
    .nq    = { 8U,   25U },     /* Number of quants                                             */
    .brp   = { 1U, 1024U },     /* Baud rate prescaler                                          */
    .tSeg1 = { 2U,   16U },     /* Time segment before the sample point                         */
    .tSeg2 = { 2U,    8U },     /* Time segment after the sample point                          */
    .sjw   = { 1U,    4U }      /* Synchronization jump width                                   */
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
    
    CAN_disableController(priv->base);

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
        .periph = SYSCTL_PERIPH_CLK_CANA,
        .base   = CANA_BASE,
        .trig   = false,
        .semaphore = false,
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
    if ((CAN_getNewDataFlags(base) & (1UL << (mbx-1U))) == 0) {
        return false;
    }

    /* This also acknowledges received message */
    CAN_transferMessage(base, 2U, mbx, false, false);

    uint32_t mctl = HWREG_BP(base + CAN_O_IF2MCTL);
    uint32_t arb = HWREG_BP(base + CAN_O_IF2ARB);

    bool mode = (arb & CAN_IF2ARB_XTD) != 0;
    bool rtrFlag = (arb & CAN_IF2ARB_DIR) != 0;

    if (!mode || rtrFlag) {
        return false;
    }

    if (f) {
        f->id = (arb & CAN_IF2ARB_ID_M) >> CAN_IF2ARB_ID_S;

        f->length = (mctl & CAN_IF2MCTL_DLC_M) >> CAN_IF2MCTL_DLC_S;

        uint16_t data[8];

        CAN_readDataReg(data, base + CAN_O_IF2DATA, f->length);

        unsigned i;
        for (i = 0U; i < f->length; i++) {
            f->data[i] = (uint8_t) data[i];
        }
    }

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
 * \return \e true when mailbox has been written to the mailbox
 * 
 *************************************************************************************************/
static bool
can__mailbox_write(uint32_t base, uint16_t mbx, const struct can_f *f)
{
    if ((CAN_getTxRequests(base) & (1UL << (mbx-1U))) != 0) {
        return false;
    }

    if (f) {
        CAN_setupMessageObject(base, mbx, f->id, CAN_MSG_FRAME_EXT,
            CAN_MSG_OBJ_TYPE_TX, 0UL, 0UL, f->length);
        
        uint16_t data[8];

        unsigned i;
        for (i = 0U; i < f->length; i++) {
            data[i] = (uint16_t) f->data[i];
        }
        
        CAN_writeDataReg(data, base + CAN_O_IF1DATA, f->length);

        CAN_transferMessage(base, 1U, mbx, true, false);
    } else {
        uint32_t cmd = 0UL;
        cmd |= (uint32_t) CAN_IF1CMD_TXRQST;
        cmd |= (uint32_t) CAN_IF1CMD_DIR;
        cmd |= (mbx & CAN_IF1CMD_MSG_NUM_M) >> CAN_IF1CMD_MSG_NUM_S;

        while (HWREGH(base + CAN_O_IF1CMD) & CAN_IF1CMD_BUSY);

        /* Trigger message transmit */
        HWREG_BP(base + CAN_O_IF1CMD) = cmd;

        while (HWREGH(base + CAN_O_IF1CMD) & CAN_IF1CMD_BUSY);
    }

    return true;
}

/**************************************************************************************************
 * 
 * Net layer implementation. Static (private) functions linked to \b net object.
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
    asm(" RPT #5 || NOP");

    CAN_initModule(priv->base);

    CAN_setBitTiming(priv->base, ((btp.brp - 1U) % 64U), ((btp.brp - 1U) / 64U),
        (btp.tSeg1 - 1U), (btp.tSeg2 - 1U), (btp.sjw - 1U));


    //CAN_setBitRate(priv->base, uint32_t clockFreq, 500000,8)
    CAN_enableAutoBusOn(priv->base);

    CAN_enableRetry(priv->base);

    unsigned mbx;
    for (mbx = 1U; mbx <= CAN_MAILBOX_MAX; mbx++) {
        CAN_clearMessage(priv->base, mbx);
    }

    CAN_enableController(priv->base);

    /* CAN driver must be opened before configuring mailboxes */
    priv->open = true;

    struct can_cfg cfg = {
        .mbx    = 0x7FFFFFFFUL,// was set to 0x3FFFFFFFUL by Marko,
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

    priv->semaphore = true;
    unsigned mbx;
    for (mbx = CAN_MAILBOX_RX1; mbx <= CAN_MAILBOX_RXN; mbx++) {
        if (can__mailbox_read(priv->base, mbx, f)) {
            priv->semaphore = false;
            return 1;
        }
    }
    priv->semaphore = false;
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
    for (mbx = CAN_MAILBOX_TX1; mbx <= CAN_MAILBOX_TXN; mbx++) {
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
        uint32_t mbx_mask = 1UL << (CAN_MAILBOX_MAX - mbx);

        if ((cfg->mbx & mbx_mask) == 0) {
            continue;
        }

        CAN_clearMessage(priv->base, mbx);

        if (!cfg->enable) {
            continue;
        }

        /** TODO: Think about having standard identifier for low-latency mailboxes! */

        if (((mbx >= CAN_MAILBOX_RX1) && (mbx <= CAN_MAILBOX_RXN)) || (mbx == CAN_MAILBOX_RXL)) {
            uint32_t msgFlags = CAN_MSG_OBJ_NO_FLAGS;
            if ((mbx != CAN_MAILBOX_RXN) && (mbx != CAN_MAILBOX_RXL)) {
                msgFlags = CAN_MSG_OBJ_FIFO;
                CAN_setupMessageObject(priv->base, mbx, cfg->id, CAN_MSG_FRAME_EXT,
                    CAN_MSG_OBJ_TYPE_RX, cfg->mask, msgFlags | CAN_MSG_OBJ_USE_ID_FILTER, 0U);
            }
            else if(mbx == CAN_MAILBOX_RXL){
                 msgFlags |= (CAN_MSG_OBJ_USE_ID_FILTER | CAN_MSG_OBJ_USE_EXT_FILTER);
                /** TODO: all types of modules will receive sync. messages from AFE...
                 * but they will not do anything. Anyway, to keep in mind.
                 */
                // 0x80000100 is the CAN_SYNC ID used for inverter sync mode in AFE
                // 0x1FFFFFFF is the mask used for this message
                CAN_setupMessageObject(priv->base, mbx, 0x80000100, CAN_MSG_FRAME_EXT,
                    CAN_MSG_OBJ_TYPE_RX, 0x1FFFFFFF, msgFlags, 0U);
            }else{
                /* CAN_MAILBOX_RXN case */
                CAN_setupMessageObject(priv->base, mbx, cfg->id, CAN_MSG_FRAME_EXT,
                    CAN_MSG_OBJ_TYPE_RX, cfg->mask, msgFlags | CAN_MSG_OBJ_USE_ID_FILTER, 0U);
            }


        } else {
            if (cfg->f && (mbx == CAN_MAILBOX_TXL)) {
                CAN_setupMessageObject(priv->base, mbx, cfg->f->id, CAN_MSG_FRAME_EXT,
                    CAN_MSG_OBJ_TYPE_TX, 0UL, CAN_MSG_OBJ_NO_FLAGS, cfg->f->length);
                
                uint16_t data[8];

                unsigned i;
                for (i = 0U; i < cfg->f->length; i++) {
                    data[i] = (uint16_t) cfg->f->data[i];
                }
                
                CAN_writeDataReg(data, priv->base + CAN_O_IF1DATA, cfg->f->length);

                uint32_t cmd = 0UL;
                cmd |= (uint32_t) CAN_IF1CMD_DATA_A;
                cmd |= (uint32_t) CAN_IF1CMD_DATA_B;
                cmd |= (uint32_t) CAN_IF1CMD_DIR;
                cmd |= (mbx & CAN_IF1CMD_MSG_NUM_M) >> CAN_IF1CMD_MSG_NUM_S;
                
                while(HWREGH(priv->base + CAN_O_IF1CMD) & CAN_IF1CMD_BUSY);

                /* Transfer data to mailbox RAM without triggering message transmit */
                HWREG_BP(priv->base + CAN_O_IF1CMD) = cmd;

                while(HWREGH(priv->base + CAN_O_IF1CMD) & CAN_IF1CMD_BUSY);
            } else {
                CAN_setupMessageObject(priv->base, mbx, 0UL, CAN_MSG_FRAME_EXT,
                    CAN_MSG_OBJ_TYPE_TX, 0UL, CAN_MSG_OBJ_NO_FLAGS, 0U);
            }
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

    /* We only try to read it if semaphore is free, meaning that the main application
    is not trying to read anything when this function is called in the ISR. */
    if(!priv->semaphore){
        if (can__mailbox_read(priv->base, CAN_MAILBOX_RXL, f)) {
            return 1;
        }
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

    bool trs = (CAN_getTxRequests(priv->base) & (1UL << (CAN_MAILBOX_TXL-1U))) != 0;

    if (priv->trig) {
        priv->trig = trs;
        return trs ? 0 : 1;
    } else { /* !priv->is_trig */
        return trs ? -1 : 2;
    }
}
