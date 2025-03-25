/**************************************************************************************************
 *
 * \file spi.c
 *
 * \brief SPI driver implementation
 *
 * \author Jorge Sola
 *
 *************************************************************************************************/

#include "inc/drv/spi.h"

/* Must be included before driverlib/spi.h */
#include <stdbool.h>

#include "driverlib/sysctl.h"

#include "inc/drv/clk.h"
#include "inc/lib/debug.h"
#include "inc/net/spi.h"

#include <stddef.h>

/**************************************************************************************************
 *
 * Values that can be passed to \e spi__wait() as \e cond parameter.
 *
 *************************************************************************************************/
enum spi_cond {
    SPI_COND_TX_RDY,   /* Wait until transmit ready                                    */
    SPI_COND_RX_RDY,   /* Wait until receive data fifo becomes available               */
    SPI_COND_DATA_RDY, /* Wait until there is data in the receive fifo                 */
    SPI_COND_RESET     /* Wait one timeout constant when SPI module is disabled        */
};

/**************************************************************************************************
 *
 * Defines for the driver
 *
 *************************************************************************************************/

#define SPI_BITRATE_MAX 75000000UL /* Maximum communication speed (bps)                */

/**************************************************************************************************
 *
 * Private structure definition
 *
 *************************************************************************************************/
NET_PRIV_STRUCT(spi, const SysCtl_PeripheralPCLOCKCR periph; /* SPI peripheral clock */
                const uint32_t base;                         /* SPI module base register address                         */
                uint32_t timeout; /* CPU delay constant for 20 SPI bit time                       */
                uint32_t bit_us;  /* One SPI bit time duration (us)                               */
);

/**************************************************************************************************
 *
 * \brief Resets SPI module to default (on-reset) configuration
 *
 * \param net net object handler
 *
 * \return None
 *
 *************************************************************************************************/
static void spi__reset(const struct net *net)
{
    struct spi_priv *priv = net->priv;

    /** TODO: Return SPI module to default-on-reset state */

    SysCtl_disablePeripheral(priv->periph);
}

/**************************************************************************************************
 *
 * spi_a_new()
 *
 *************************************************************************************************/
const struct net *spi_a_new(void)
{
    NET_PRIV_NEW(
        spi, NET_SPI, .periph = SYSCTL_PERIPH_CLK_SPIA, .base = SPIA_BASE, .timeout = 0U,
        .bit_us = 0U);

    static struct spi spi = {
        .io = {        IOX,          IOX,          IOX,         IOX},
          .exp = {IO_SPIA_CLK, IO_SPIA_SIMO, IO_SPIA_SOMI, IO_SPIA_STE}
    };

    NET_OBJ_NEW(spi, &spi);

    spi__reset(&net);

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
static bool spi__wait(const struct net *net, enum spi_cond cond)
{
    struct spi_priv *priv = net->priv;

    uint32_t base    = priv->base;
    uint32_t timeout = priv->timeout;

    while (timeout--) {
        /* Resolve condition type */
        bool condition;
        switch (cond) {
        case SPI_COND_TX_RDY:
            condition = !SPI_isBusy(base);
            break;
        case SPI_COND_RX_RDY:
            condition = (SPI_getRxFIFOStatus(base) < SPI_FIFO_RXFULL);
            break;
        case SPI_COND_DATA_RDY:
            condition = (SPI_getRxFIFOStatus(base) > 0);
            break;
        case SPI_COND_RESET:
            condition = true;
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
 * spi__open()
 *
 *************************************************************************************************/
static int spi__open(const struct net *net, const struct spi_usr *usr)
{
    struct spi_priv *priv = net->priv;

    if ((usr->bitrate > SPI_BITRATE_MAX) || (usr->bitcount > 16U)) {
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
    asm(" RPT #5 || NOP");
#else
#error "spi.c - dsp not defined"
#endif

    SPI_disableModule(priv->base);

    /* Time constants */
    priv->timeout = (20U * 1000000UL) / usr->bitrate;
    priv->bit_us  = (priv->timeout + (20U - 1U)) / 20U; /* round up */

    SPI_setConfig(
        priv->base, _clk->cpu.Hz / 2, SPI_PROT_POL1PHA0, SPI_MODE_MASTER, usr->bitrate,
        usr->bitcount);

    SPI_enableFIFO(priv->base);

    SPI_setEmulationMode(priv->base, SPI_EMULATION_FREE_RUN);

    SPI_enableModule(priv->base);

    priv->open = true;

    return 0;
}

/**************************************************************************************************
 *
 * spi__close()
 *
 *************************************************************************************************/
static int spi__close(const struct net *net)
{
    struct spi_priv *priv = net->priv;

    priv->open = false;

    spi__reset(net);

    return 0;
}

/**************************************************************************************************
 *
 * spi__read()
 *
 *************************************************************************************************/
static int spi__read(const struct net *net, struct spi_f *f, unsigned length)
{
    struct spi_priv *priv = net->priv;
    int i;


    for (i = 0; i < length; i++) {
        if (spi__wait(net, SPI_COND_DATA_RDY)) {
            goto restart;
        }

        f[i].data[0] = SPI_readDataNonBlocking(priv->base);
        f[i].length  = 1;
    }
    return i;

restart:
    SPI_disableModule(priv->base);
    spi__wait(net, SPI_COND_RESET);
    SPI_enableModule(priv->base);
    return i;
}

/**************************************************************************************************
 *
 * spi__write()
 *
 *************************************************************************************************/
static int spi__write(const struct net *net, struct spi_f *f, unsigned length)
{
    struct spi_priv *priv = net->priv;
    int i;


    for (i = 0; i < length; i++) {

        if (spi__wait(net, SPI_COND_TX_RDY)) {
            goto restart;
        }

        SPI_writeDataNonBlocking(priv->base, f[i].data[0]);
    }
    return i;

restart:
    SPI_disableModule(priv->base);
    spi__wait(net, SPI_COND_RESET);
    SPI_enableModule(priv->base);
    return i;
}
