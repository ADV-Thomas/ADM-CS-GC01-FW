/**************************************************************************************************
 *
 * \file fsi.c
 *
 * \brief FSI driver implementation
 *
 * \author Jorge Sola
 *
 *************************************************************************************************/

#include "inc/drv/fsi.h"

/* Must be included before driverlib/fsi.h */
#include <stdbool.h>

#include "driverlib/sysctl.h"
#include "driverlib/xbar.h"

#include "inc/drv/clk.h"
#include "inc/drv/dio.h"

#include "inc/lib/debug.h"
#include "inc/net/fsi.h"

#include <stddef.h>

#define HANDSHAKE_TIMEOUT   (1000U) /* Corresponds to 1 second if task is run at 1 KHz*/
#define STATE_TIMEOUT   (200U)     /* Corresponds to 200 millisecond if task is run at 1 KHz*/
#define TIMEOUT_WATCHDOG       (10000000UL) /* Ping watchdog timeout in SYSCLKS ticks. This is 100 ms*/

/**************************************************************************************************
 *
 * Values that can be passed to \e fsi__wait() as \e cond parameter.
 *
 *************************************************************************************************/
enum fsi_cond {
    FSI_COND_TX_RDY,   /* Wait until transmit ready                                    */
    FSI_COND_RX_RDY,   /* Wait until receive data fifo becomes available               */
    FSI_COND_DATA_RDY, /* Wait until there is data in the receive fifo                 */
    FSI_COND_RESET     /* Wait one timeout constant when FSI module is disabled        */
};

/**************************************************************************************************
 *
 * Defines for the driver
 *
 *************************************************************************************************/

#define FSI_CLK_MAX 50000000UL /* Maximum communication speed (Hz)                */

/**************************************************************************************************
 *
 * Private structure definition
 *
 *************************************************************************************************/
NET_PRIV_STRUCT(fsi, 
                const SysCtl_PeripheralPCLOCKCR clk_tx; /* FSI peripheral clock */
                const SysCtl_PeripheralPCLOCKCR clk_rx; /* FSI peripheral clock */
                const uint32_t base_tx;                    /* FSI module base register address */
                const uint32_t base_rx;                     /*                                    */
                uint32_t timeout; /* CPU delay constant for 20 FSI bit time                       */
                uint32_t timeout_local;
                uint32_t tx_timeout;
                uint32_t clk_freq;  /* One FSI bit time duration (us)                             */
                uint16_t prescalar;
                enum fsi_state state;
                enum fsi_type fsi_type;
                uint16_t trg_src;
);

/**************************************************************************************************
 *
 * Forward declarations
 *
 *************************************************************************************************/
static void send_ping(uint16_t tag);
static bool ping_received(uint16_t tag);
static void handshake_slave(const struct net *net);
static void handshake_master(const struct net *net);
static void operation(const struct net *net);
static void fsi__set_ping_mode(const struct net *net);
static void fsi__set_sw_mode(void);
static bool fsi__needs_reset(void);
static bool fsi__wait(const struct net *net, uint16_t cond);

/**************************************************************************************************
 *
 * \brief Resets FSI module to default (on-reset) configuration
 *
 * \param net net object handler
 *
 * \return None
 *
 *************************************************************************************************/
static void fsi__reset(const struct net *net)
{
    FSI_resetRxModule(FSIRXA_BASE, FSI_RX_MASTER_CORE_RESET);
    clk_delay(10U);
    FSI_clearRxModuleReset(FSIRXA_BASE, FSI_RX_MASTER_CORE_RESET);
}

/**************************************************************************************************
 *
 * fsi_a_new()
 *
 *************************************************************************************************/
const struct net *fsi_a_new(void)
{
    NET_PRIV_NEW(
        fsi, NET_FSI,
        .clk_tx = SYSCTL_PERIPH_CLK_FSITXA,
        .clk_rx = SYSCTL_PERIPH_CLK_FSIRXA,
        .base_tx = FSITXA_BASE,
        .base_rx = FSIRXA_BASE,
        .timeout = 0U,
        .clk_freq = 0U,
        .state = FSI_IDLE,);

    static struct fsi_drv fsi = {
        .io = {        IOX,          IOX,          IOX,         IOX},
        .exp = {IO_FSITXA_CLK, IO_FSITXA_D0, IO_FSIRXA_CLK, IO_FSIRXA_D0},
        .run = fsi__run,
    };

    NET_OBJ_NEW(fsi, &fsi);

    return &net;
}

/**************************************************************************************************
 *
 * \brief Waits for condition to become \e true with timeout protection
 *
 * \param net net object handler
 * \param cond condition type
 *
 * \return \e true when timeout occurs; \e false when condition has been satisfied
 *
 *************************************************************************************************/
/*static bool fsi__wait(const struct net *net, enum fsi_cond cond)
{
    // Timeout has occurred 
    return true;
}*/

/**************************************************************************************************
 *
 * Net layer implementation. Private functions linked to \b net object.
 *
 *************************************************************************************************/

/**************************************************************************************************
 *
 * fsi__open()
 *
 *************************************************************************************************/
static int fsi__open(const struct net *net, const struct fsi_usr *usr)
{
    struct fsi_priv *priv = net->priv;
    uint16_t prescaler;

    if (usr->clk_freq > FSI_CLK_MAX) {
        return -1;
    }

    const struct _clk *_clk = clk_get();
    ASSERT(_clk);

    SysCtl_enablePeripheral(priv->clk_rx);
    SysCtl_enablePeripheral(priv->clk_tx);

    asm(" RPT #5 || NOP");

    prescaler = _clk->cpu.Hz / usr->clk_freq;

    FSI_disableRxInternalLoopback(priv->base_rx);
    FSI_performTxInitialization(priv->base_tx, prescaler);
    FSI_performRxInitialization(priv->base_rx);
    
    FSI_setTxSoftwareFrameSize(FSITXA_BASE, 16);
    FSI_setRxSoftwareFrameSize(FSIRXA_BASE, 16);
    FSI_setTxDataWidth(FSITXA_BASE, FSI_DATA_WIDTH_1_LANE);
    FSI_setRxDataWidth(FSIRXA_BASE, FSI_DATA_WIDTH_1_LANE);

    /* If Master FSI, set Tx External Ping trigger source*/
    if(usr->type == FSI_TYPE_MASTER){  
        priv->trg_src = usr->trg_src;
    }
    else if(usr->type == FSI_TYPE_SLAVE){
        /* Configure (hardcoded) XBAR Trip 5 to trigger ping transmission
        and CLB tile 1 output 4 to trip it (as in the TI examples). The 
        CLB module still has to be configured so that it trips the EPWM
        XBAR5 when the ping is received */
        XBAR_setEPWMMuxConfig(XBAR_TRIP5, XBAR_EPWM_MUX01_CLB1_OUT4);
        XBAR_enableEPWMMux(XBAR_TRIP5, XBAR_MUX01);
        priv->trg_src = FSI_EXT_TRIGSRC_EPWM_XBARTRIP5; /* This corresponds to EPWM TRIP 5*/
    }

    /* For the moment, transmission only happens if software enabled */
    FSI_setTxStartMode(FSITXA_BASE, FSI_TX_START_FRAME_CTRL);

    priv->open = true;
    priv->fsi_type = usr->type;
    priv->prescalar = prescaler;

    /* For 16 word data frames, 296 bits are transmitted. We use a safety factor of 2 */
    priv->tx_timeout = 2 * (296U * 1000000UL)/usr->clk_freq;

    /* TODO: this was for debugging with the LaunchPad*/
    //io_connect(IO34_DOUT,IO_DOUT);
    //dio_write(IO34_DOUT,false);

    return 0;
}

/**************************************************************************************************
 *
 * fsi__close()
 *
 *************************************************************************************************/
static int fsi__close(const struct net *net)
{
    struct fsi_priv *priv = net->priv;

    priv->open = false;

    fsi__reset(net);

    return 0;
}

/**************************************************************************************************
 *
 * fsi__read()
 *
 *************************************************************************************************/
static int fsi__read(const struct net *net, struct fsi_f *f, unsigned length)
{
    struct fsi_priv *priv = net->priv;

    /* Run state machine */
    fsi__run(net);

    uint16_t rxFlags;
    uint16_t receivedWords;

    /* If handshake was successful, then read */
    if(priv->state == FSI_OPERATION){
        /* Read RX Event flags */
        rxFlags = FSI_getRxEventStatus(FSIRXA_BASE);

        /* Return in case of CRC error */
        if(rxFlags & (FSI_RX_EVT_CRC_ERR)){
            FSI_clearRxEvents(FSITXA_BASE, FSI_RX_EVT_CRC_ERR);
            return 0;
        }

        /* If received data frame, and there are 16 words, read its content */
        if(rxFlags & (FSI_RX_EVT_DATA_FRAME | FSI_RX_EVT_FRAME_DONE) ){

            receivedWords = FSI_getRxWordCount(FSIRXA_BASE);
            if(receivedWords == 16){
                FSI_clearRxEvents(FSITXA_BASE, (FSI_RX_EVT_DATA_FRAME | FSI_RX_EVT_FRAME_DONE));
                f->id = FSI_getRxUserDefinedData(FSIRXA_BASE);
                FSI_readRxBuffer(FSIRXA_BASE,f->data,16U,0U);
                return 1;
            } else {
                return 0;
            }
        } else {
            return 0;
        }
    }
    return -1;
}

/**************************************************************************************************
 *
 * fsi__write()
 *
 *************************************************************************************************/
static int fsi__write(const struct net *net, struct fsi_f *f, unsigned length)
{
    struct fsi_priv *priv = net->priv;

    //uint16_t txFlags = 0;

    /* If handshake was successful, then write */
    if(priv->state == FSI_OPERATION){

        fsi__set_sw_mode();
        FSI_setTxFrameType(FSITXA_BASE, FSI_FRAME_TYPE_NWORD_DATA);

        FSI_setTxUserDefinedData(FSITXA_BASE,f->id);
        FSI_writeTxBuffer(FSITXA_BASE,f->data,16U,0U);
        FSI_clearTxEvents(FSITXA_BASE, FSI_TX_EVT_FRAME_DONE); // Clear previous FRAME_DONE flags
        FSI_startTxTransmit(FSITXA_BASE);

        /* Wait (with timeout protection) until the data frame is sent */
        fsi__wait(net, FSI_TX_EVT_FRAME_DONE);

        fsi__set_ping_mode(net);

        return 1;
    }
    else{
        return 0;
    }
}

/**************************************************************************************************
 * 
 * \brief Waits for condition to become \e true with timeout protection
 * 
 * \param net net object handler
 * \param cond condition type
 * 
 * \return \e true when timeout occurs; \e false when condition has been satisfied
 * 
 *************************************************************************************************/
static bool
fsi__wait(const struct net *net, uint16_t cond)
{
    struct fsi_priv *priv = net->priv;
    //uint16_t txFlags;
    uint32_t timeout = priv->tx_timeout;

    while (timeout--) {
        /* Resolve condition type */
        bool condition;

        condition = (FSI_getTxEventStatus(FSITXA_BASE) & cond) != 0U;

        if (condition) {
            FSI_clearTxEvents(FSITXA_BASE, cond);
            return false;
        }

        /* Delay 1 us */
        clk_delay(1U);
    }

    /* Timeout has occurred */
    return true;
}

static int fsi__run(const struct net *net)
{
    struct fsi_priv *priv = net->priv;

    /** MASTER state machine **/
    if(priv->fsi_type == FSI_TYPE_MASTER){
        switch(priv->state){
        case FSI_IDLE:
            fsi__set_sw_mode();

            FSI_executeTxFlushSequence(FSITXA_BASE, priv->prescalar);
            send_ping(FSI_FRAME_TAG0);
            priv->state = FSI_HANDSHAKE_WAIT_TAG_0;
            priv->timeout = 0;
            priv->timeout_local = 0;
        break;  
        case FSI_HANDSHAKE_WAIT_TAG_0:
        case FSI_HANDSHAKE_WAIT_TAG_1:
            handshake_master(net);
        break;
        case FSI_OPERATION:
            operation(net);
        break;
        default:
        break;
        }
    }


    /** SLAVE state machine **/
    else if(priv->fsi_type == FSI_TYPE_SLAVE){
        switch(priv->state){
        case FSI_IDLE:
            fsi__set_sw_mode();

            priv->state = FSI_HANDSHAKE_WAIT_TAG_0;
            priv->timeout = 0;
            priv->timeout_local = 0;
        break;  
        case FSI_HANDSHAKE_WAIT_TAG_0:
        case FSI_HANDSHAKE_WAIT_TAG_1:
        case FSI_HANDSHAKE_EXTRA_WAIT:
            handshake_slave(net);
        break;
        case FSI_OPERATION:
            operation(net);
        break;
        default:
        break;
        }
    }

    return 0;
}

static void operation(const struct net *net)
{
    struct fsi_priv *priv = net->priv;
    uint16_t rxFlags;

    /* Check and reset if ping was not received in time */
    rxFlags = FSI_getRxEventStatus(FSIRXA_BASE);
    if(rxFlags & FSI_RX_EVT_PING_WD_TIMEOUT){
        priv->state = FSI_IDLE;
        FSI_clearRxEvents(FSIRXA_BASE, FSI_RX_EVT_PING_WD_TIMEOUT);
        //dio_write(IO34_DOUT,false);
    }
    else if(fsi__needs_reset()){
        FSI_clearRxEvents(FSIRXA_BASE, rxFlags);
        fsi__reset(net);
        priv->state = FSI_IDLE;
        //dio_write(IO34_DOUT,false);
    }
    else{
        //dio_write(IO34_DOUT,true);
    }
}

static void handshake_master(const struct net *net)
{
    struct fsi_priv *priv = net->priv;

    priv->timeout++;
    priv->timeout_local++;

    if(priv->state == FSI_HANDSHAKE_WAIT_TAG_0){
        if(priv->timeout > HANDSHAKE_TIMEOUT || priv->timeout_local > STATE_TIMEOUT){
            priv->state = FSI_IDLE;
        }
        else if(ping_received(FSI_FRAME_TAG0)){
            /* Ping Tag 0 was received. Now, send Ping Tag 1 */
            send_ping(FSI_FRAME_TAG1);
            priv->state = FSI_HANDSHAKE_WAIT_TAG_1;
            priv->timeout_local = 0;
        }
    }

    else if(priv->state == FSI_HANDSHAKE_WAIT_TAG_1){
        if(priv->timeout > HANDSHAKE_TIMEOUT){
            priv->state = FSI_IDLE;
        }
        else if(priv->timeout_local > STATE_TIMEOUT){
            send_ping(FSI_FRAME_TAG1);
            priv->timeout_local = 0;
        }
        else if(ping_received(FSI_FRAME_TAG1)){
            /* Ping Tag 1 was received*/

            /* Enable automatic transmission of ping mode */
            fsi__set_ping_mode(net);

            priv->state = FSI_OPERATION;
        }
    }
}

static void handshake_slave(const struct net *net)
{
    struct fsi_priv *priv = net->priv;

    priv->timeout++;
    priv->timeout_local++;

    if(priv->state == FSI_HANDSHAKE_WAIT_TAG_0){
        if(priv->timeout > HANDSHAKE_TIMEOUT || priv->timeout_local > STATE_TIMEOUT){
            priv->state = FSI_IDLE;
        }
        else if(ping_received(FSI_FRAME_TAG0)){
            /* Ping Tag 0 was received. Now, send Ping Tag 1 */
            FSI_executeTxFlushSequence(FSITXA_BASE, priv->prescalar);
            send_ping(FSI_FRAME_TAG0);
            priv->state = FSI_HANDSHAKE_WAIT_TAG_1;
            priv->timeout_local = 0;
        }
    }
    
    else if(priv->state == FSI_HANDSHAKE_WAIT_TAG_1){
        if(priv->timeout > HANDSHAKE_TIMEOUT){
            priv->state = FSI_IDLE;
        }
        else if(priv->timeout_local > STATE_TIMEOUT){
            FSI_executeTxFlushSequence(FSITXA_BASE, priv->prescalar);
            send_ping(FSI_FRAME_TAG0);
            priv->timeout_local = 0;
        }
        else if(ping_received(FSI_FRAME_TAG1)){
            /* Ping Tag 1 was received. Send back Ping Tag 1 */
            send_ping(FSI_FRAME_TAG1);

            /* Enable automatic transmission of ping mode */
            fsi__set_ping_mode(net);

            /* Wait 1 more millisecond before enabling standard operation,
             * so that the application does not try to write data right away, because
             * it would interfere with the handshake process */
            priv->state = FSI_HANDSHAKE_EXTRA_WAIT;
        }
    }
    else if(priv->state == FSI_HANDSHAKE_EXTRA_WAIT){
        /* Enable standard operation */
        priv->state = FSI_OPERATION;

    }
}

/**************************************************************************************************
 *
 * ping_received()
 *
 *************************************************************************************************/
static bool ping_received(uint16_t tag)
{
    uint16_t rx_flags;
    //uint16_t rx_frameType;
    uint16_t rx_pingTag;

    rx_flags = FSI_getRxEventStatus(FSIRXA_BASE);
    //rx_frameType = FSI_getRxFrameType(FSIRXA_BASE);
    rx_pingTag = FSI_getRxPingTag(FSIRXA_BASE);
    FSI_clearRxEvents(FSIRXA_BASE, rx_flags);

    if(rx_flags == (FSI_RX_EVT_PING_FRAME | FSI_RX_EVT_FRAME_DONE)
        //&& rx_frameType == FSI_FRAME_TYPE_PING
        && rx_pingTag == tag){
        return true;
    }
    return false;
}

/**************************************************************************************************
 *
 * send_ping()
 *
 *************************************************************************************************/
static void send_ping(uint16_t tag)
{
    /** NOTE: Here we use Frame Tag instead of Ping tag, because for software transmitted frames,
     * we need to use frame tag, as explained in the technical reference manual. */
    FSI_setTxFrameTag(FSITXA_BASE, (FSI_FrameTag)tag);
    FSI_setTxFrameType(FSITXA_BASE, FSI_FRAME_TYPE_PING);
    FSI_startTxTransmit(FSITXA_BASE);
}

/**************************************************************************************************
 *
 * fsi_set_sw_mode()
 * 
 * \brief It sets the FSI configuration for manual software trigger of frames
 *
 *************************************************************************************************/
static void fsi__set_sw_mode(void)
{
    FSI_setTxStartMode(FSITXA_BASE, FSI_TX_START_FRAME_CTRL);
    FSI_disableRxPingWatchdog(FSIRXA_BASE);
    FSI_disableTxExtPingTrigger(FSITXA_BASE);
}


/**************************************************************************************************
 *
 * fsi_set_sw_mode()
 * 
 * \brief It sets the FSI configuration for automatic trigger of ping frames
 * 
 *************************************************************************************************/
static void fsi__set_ping_mode(const struct net *net)
{
    struct fsi_priv *priv = net->priv;

    /* Enable Ping Watchdog with 1.5 seconds of timeout, assuming 100 MHz SYSCLK*/
    FSI_setRxPingTimeoutMode(FSIRXA_BASE,FSI_PINGTIMEOUT_ON_HWINIT_PING_FRAME);
    FSI_enableRxPingWatchdog(FSIRXA_BASE,TIMEOUT_WATCHDOG); 

    /* Enable ping trigger*/
    FSI_setTxPingTag(FSITXA_BASE, FSI_FRAME_TAG1);
    FSI_setTxFrameType(FSITXA_BASE, FSI_FRAME_TYPE_PING);
    FSI_enableTxExtPingTrigger(FSITXA_BASE, priv->trg_src);
    FSI_setTxStartMode(FSITXA_BASE, FSI_TX_START_EXT_TRIG);
}


/**************************************************************************************************
 *
 * fsi__needs_reset()
 * 
 * \brief Returns true if module needs a reset
 * 
 *************************************************************************************************/
static bool fsi__needs_reset(void)
{
    return(HWREGH(FSIRXA_BASE + FSI_O_RX_VIS_1) & FSI_RX_VIS_1_RX_CORE_STS);
}
