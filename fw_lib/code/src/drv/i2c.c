/**************************************************************************************************
 * 
 * \file i2c.c
 * 
 * \brief I2C driver implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "inc/drv/i2c.h"

/* Must be included before driverlib/i2c.h */
#include <stdbool.h>

#include "driverlib/i2c.h"
#include "driverlib/sysctl.h"

#include "inc/drv/clk.h"
#include "inc/lib/debug.h"
#include "inc/net/i2c.h"

#include <stddef.h>

/**************************************************************************************************
 * 
 * Values that can be passed to \e i2c__wait() as \e cond parameter.
 * 
 *************************************************************************************************/
enum i2c_cond {
    I2C_COND_BUS_BUSY,          /* Wait until bus busy                                          */
    I2C_COND_RRDY,              /* Wait until receive data register becomes available           */
    I2C_COND_XRDY,              /* Wait until transmit data register becomes available          */
    I2C_COND_XSMT,              /* Wait until data byte shifts out on the bus                   */
    I2C_COND_ARDY,              /* Wait until I2C has used its registers                        */
    I2C_COND_STOP_CONDITION,    /* Wait until stop condition has been generated                 */
    I2C_COND_RESET              /* Wait one timeout constant when I2C module is disabled        */
};

/**************************************************************************************************
 * 
 * Defines for the driver
 * 
 *************************************************************************************************/

#define I2C_BITRATE_MAX     400000UL        /* Maximum communication speed (bps)                */

/**************************************************************************************************
 * 
 * Private structure definition
 * 
 *************************************************************************************************/
NET_PRIV_STRUCT(i2c,
    const SysCtl_PeripheralPCLOCKCR periph; /* I2C peripheral clock                             */
    const uint32_t base;        /* I2C module base register address                             */
    uint32_t timeout;           /* CPU delay constant for 20 I2C bit time                       */
    uint32_t bit_us;            /* One I2C bit time duration (us)                               */
);

/**************************************************************************************************
 * 
 * \brief Resets I2C module to default (on-reset) configuration
 * 
 * \param net net object handler
 * 
 * \return None
 * 
 *************************************************************************************************/
static void
i2c__reset(const struct net *net)
{
    struct i2c_priv *priv = net->priv;

    /** TODO: Return I2C module to default-on-reset state */

    SysCtl_disablePeripheral(priv->periph);
}

/**************************************************************************************************
 * 
 * i2c_a_new()
 * 
 *************************************************************************************************/
const struct net *
i2c_a_new(void)
{
    NET_PRIV_NEW(i2c, NET_I2C,
        .periph  = SYSCTL_PERIPH_CLK_I2CA,
        .base    = I2CA_BASE,
        .timeout = 0U,
        .bit_us  = 0U
    );

    static struct i2c i2c = {
        .io  = { IOX, IOX },
        .exp = { IO_I2CA_SDA, IO_I2CA_SCL }
    };

    NET_OBJ_NEW(i2c, &i2c);

    i2c__reset(&net);
    
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
static bool
i2c__wait(const struct net *net, enum i2c_cond cond)
{
    struct i2c_priv *priv = net->priv;

    uint32_t base = priv->base;
    uint32_t timeout = priv->timeout;

    while (timeout--) {
        /* Resolve condition type */
        bool condition;
        switch (cond) {
        case I2C_COND_BUS_BUSY:
            condition = !I2C_isBusBusy(base);
            break;
        case I2C_COND_RRDY:
            condition = (I2C_getStatus(base) & I2C_STS_RX_DATA_RDY) != 0U;
            break;
        case I2C_COND_XRDY:
            condition = (I2C_getStatus(base) & I2C_STS_TX_DATA_RDY) != 0U;
            break;
        case I2C_COND_XSMT:
            condition = (I2C_getStatus(base) & I2C_STS_TX_EMPTY) == 0U;
            break;
        case I2C_COND_ARDY:
            condition = (I2C_getStatus(base) & I2C_STS_REG_ACCESS_RDY) != 0U;
            break;
        case I2C_COND_STOP_CONDITION:
            condition = !I2C_getStopConditionStatus(base);
            break;
        case I2C_COND_RESET:
            condition = false;
            break;
        default:
            return true;
        }

        if (condition) {
            return false;
        }

        /* Delay 1 us */
        clk_delay(1U);
    }

    /* Timeout has occurred */
    return true;
}

/**************************************************************************************************
 * 
 * Net layer implementation. Private functions linked to \b net object.
 * 
 *************************************************************************************************/

/**************************************************************************************************
 * 
 * i2c__open()
 * 
 *************************************************************************************************/
static int
i2c__open(const struct net *net, const struct i2c_usr *usr)
{
    struct i2c_priv *priv = net->priv;

    if ((usr->bitrate > I2C_BITRATE_MAX) || (usr->bitcount > 8U)) {
        return -1;
    }

    const struct _clk *_clk = clk_get();
    ASSERT(_clk);
    
    SysCtl_enablePeripheral(priv->periph);

    /* Configuration registers cannot be modified before peripheral clock has been enabled! */
#if defined(F2803X)
    asm(" RPT #2 || NOP");
#elif defined(F28004X)
    asm(" RPT #5 || NOP");
#elif defined(F28P65X)
    asm(" RPT #10 || NOP");
#else
#error "i2c.c - dsp not defined"
#endif

    /* Clear any pending START or STOP condition triggers */
    HWREGH(priv->base + I2C_O_MDR) &= ~(I2C_MDR_STT);
    HWREGH(priv->base + I2C_O_MDR) &= ~(I2C_MDR_STP);

    I2C_disableModule(priv->base);

    /* Time constants */
    priv->timeout = (20U * 1000000UL) / usr->bitrate;
    priv->bit_us = (priv->timeout + (20U-1U)) / 20U; /* round up */

    I2C_initMaster(priv->base, _clk->cpu.Hz, usr->bitrate, I2C_DUTYCYCLE_50);

    I2C_disableFIFO(priv->base);

    /* 0x7U mask will force bitcount from 8U to 0U, which corresponds to I2C_BITCOUNT_8 */
    I2C_setBitCount(priv->base, (I2C_BitCount) (usr->bitcount & 0x7U));

    I2C_setEmulationMode(priv->base, I2C_EMULATION_FREE_RUN);

    I2C_enableModule(priv->base);

    priv->open = true;

    return 0;
}

/**************************************************************************************************
 * 
 * i2c__close()
 * 
 *************************************************************************************************/
static int
i2c__close(const struct net *net)
{
    struct i2c_priv *priv = net->priv;

    priv->open = false;

    i2c__reset(net);
    
    return 0;
}

/**************************************************************************************************
 * 
 * i2c__read()
 * 
 *************************************************************************************************/
static int
i2c__read(const struct net *net, struct i2c_f *f, unsigned length)
{
    /* Read operation in I2C master mode is performed by write! */
    return i2c_write(net, f, length);
}

/**************************************************************************************************
 * 
 * i2c__write()
 * 
 *************************************************************************************************/
static int
i2c__write(const struct net *net, struct i2c_f *f, unsigned length)
{
    struct i2c_priv *priv = net->priv;

    /**
     * Wait for I2C bus to become available. Be careful as I2C module monitors the bus only once
     * the module has been enabled. When the module is taken out of reset the "bus busy" bit will
     * not correctly reflect the bus status until START or STOP condition is detected.
     */
    if (i2c__wait(net, I2C_COND_BUS_BUSY)) {
        return -1;
    }

    /**
     * ARDY flag in status register does not indicate if I2C module is ready or not, but only that
     * previously written value has been used and registers are ready to be used again. ARDY flag
     * is 0 on device reset, although I2C is ready to use registers!
     */
    I2C_clearStatus(priv->base, I2C_STS_NACK_SENT);

    unsigned i;

    /* Check all frames */
    for (i = 0U; i < length; i++) {
        if (!f[i].data || (f[i].length == 0U)) {
            return -1;
        }
    }
    
    for (i = 0U; i < length; i++) {
        /**
         * Start frame must be generated before every frame. However, it can be disabled for two
         * or more consecutive "write" frames assuming slave address does not change.
         */
        if (!i || f[i].rw_ || f[i-1].rw_ || !f[i].start || (f[i].address != f[i-1].address)) {
            
            if (f[i].rw_) {
                I2C_setConfig(priv->base, I2C_MASTER_RECEIVE_MODE | I2C_REPEAT_MODE);
            } else {
                I2C_setConfig(priv->base, I2C_MASTER_SEND_MODE | I2C_REPEAT_MODE);
            }

            I2C_setSlaveAddress(priv->base, f[i].address & 0x007FU);

            I2C_clearStatus(priv->base, I2C_STS_NO_ACK);

            /* NACK bit is generated on 9-th I2C bit of the start frame */
            uint32_t timeout = f[i].start_us / (9U * priv->bit_us);

            do {
                /**
                 * Generate start frame: start bit + 7-bit address + R/W bit. In read operation
                 * (RW = 1), first data byte is being clocked automatically as soon as ACK bit
                 * is received. In write operation (RW = 0), first data byte is clocked once it
                 * is written to the transmit data register.
                 */
                I2C_sendStartCondition(priv->base);

                /**
                 * NOTE: There was small CPU delay previously here to make sure that ARDY flag is
                 * cleared before polling its status to detect when start frame has been generated.
                 */

                /**
                 * First data byte is being clocked automatically as soon as ACK bit is received,
                 * and the ARDY flag will be set once the data byte is received. The NACK bit must
                 * be generated by master receiver before the end of the last data byte that is to
                 * be received. In this special case, waiting for the ARDY flag to set will be too
                 * late for NACK bit to be generated.
                 */
                if (f[i].rw_ && (f[i].length == 1U)) {
                    I2C_sendNACK(priv->base);
                }

                /**
                 * Wait until start frame has been generated and ACK/NACK bit has been received.
                 * In master transmitter mode the ARDY flag will be set once ACK or NACK bit has
                 * been received by the master. In master receiver mode the ARDY flag will be set
                 * once the first data byte has been received by the master, which means at least
                 * two I2C frames. Timeout constant must be long enough to cover both these cases!
                 */
                if (i2c__wait(net, I2C_COND_ARDY)) {
                    goto restart;
                }

                /* Check if NACK bit has been received */
                if (I2C_getStatus(priv->base) & I2C_STS_NO_ACK) {
                    /* Clear NACK bit manually */
                    I2C_clearStatus(priv->base, I2C_STS_NO_ACK);

                    if (timeout-- == 0U) {
                        goto restart;
                    }
                } else {
                    /* Start frame has been generated and ACK has been received */
                    break;
                }
            } while (true);
        }

        /* Do I2C data transfer as ... */
        if (f[i].rw_) { /* ... master receiver */
            unsigned j;
            for (j = 0U; j < f[i].length; j++) {
                /**
                 * Generate NACK for the last data byte. If there was not for this, the I2C module
                 * (master) would clock one extra data byte from the slave. In a special case when
                 * frame length equals 1U, the NACK bit is set just after start frame is triggered.
                 */
                if ((f[i].length > 1U) && (j == f[i].length - 1)) {
                    I2C_sendNACK(priv->base);
                }

                /* Wait for receive data register to become available */
                if (i2c__wait(net, I2C_COND_RRDY)) {
                    goto restart;
                }

                /* Get received data byte */
                f[i].data[j] = (uint8_t) I2C_getData(priv->base);
            }

            /* I2C module must generate NACK for the last data byte */
            if (I2C_getStatus(priv->base) & I2C_STS_NACK_SENT) {
                I2C_clearStatus(priv->base, I2C_STS_NACK_SENT);
            } else {
                goto restart;
            }
        } else { /* ... master transmitter */
            unsigned j;
            for (j = 0U; j < f[i].length; j++) {
                /* Wait for transmit data register to become available */
                if (i2c__wait(net, I2C_COND_XRDY)) {
                    goto restart;
                }

                /* Send data byte */
                I2C_putData(priv->base, (uint16_t) (f[i].data[j] & 0x00FFU));

                /**
                 * Wait for the data byte to shift out. If master operates in the non-repeat mode
                 * (RM = 0) and internal data counter reaches 0, the XSMT flag will always be set.
                 * This means that the XSMT flag does not indicate transmit shift register status
                 * for the last data byte. However, this does not apply here as master operates in
                 * the repeat mode (RM = 1).
                 */
                if (i2c__wait(net, I2C_COND_XSMT)) {
                    goto restart;
                }

                /**
                 * There is no need to introduce 1 bit-time delay to wait for the ACK/NACK bit.
                 * However, if BYTE_SENT flag was polled instead of XSMT flag, we would need to
                 * add the extra 1 bit-time delay. Note that BYTE_SENT flag is not available on
                 * f2803x devices.
                 */

                /* Check if slave has acknowledged the data byte */
                if (I2C_getStatus(priv->base) & I2C_STS_NO_ACK) {
                    goto restart;
                }
            }
        }
    }

    /* Trigger stop condition after last frame */
    I2C_sendStopCondition(priv->base);

    /* Wait for stop condition to be generated */
    if (i2c__wait(net, I2C_COND_STOP_CONDITION)) {
        goto restart;
    }

    return i;
    
restart:
    /* Clear any pending START or STOP condition triggers */
    HWREGH(priv->base + I2C_O_MDR) &= ~(I2C_MDR_STT);
    HWREGH(priv->base + I2C_O_MDR) &= ~(I2C_MDR_STP);

    I2C_disableModule(priv->base);
    i2c__wait(net, I2C_COND_RESET);
    I2C_enableModule(priv->base);

    return i;
}
