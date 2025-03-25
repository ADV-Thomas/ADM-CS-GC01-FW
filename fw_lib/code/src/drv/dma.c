/**************************************************************************************************
 * 
 * \file dma.c
 * 
 * \brief DMA driver implementation
 * 
 * \author Jorge Sola
 * 
 *************************************************************************************************/

#include "inc/drv/dma.h"

#include "driverlib/dma.h"
#include "driverlib/sysctl.h"

#include "inc/drv/clk.h"
#include "inc/lib/utils.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>


/**************************************************************************************************
 * 
 * \brief Setup single DMA generator
 * 
 * \param var DMA variable
 * 
 * \return 0 if operations is successful; -1 otherwise
 * 
 *************************************************************************************************/
static int
setup_dma_channel(struct _dma_var *var)
{
    ASSERT(var && (var->base > 0U));

    uint16_t dma_word_size;

    DMA_configAddresses(var->base, var->dest_addr,var->src_addr);
    DMA_configBurst(var->base, var->words_per_burst, var->word_step_src, var->word_step_dest);
    DMA_configTransfer(var->base, var->bursts_per_transfer, var->burst_step_src, var->burst_step_dest);
    DMA_configWrap(var->base, var->wrap_src, 0, var->word_step_dest, 0);

    if(var->word_size){
        /* Word is 32-bit size */
        dma_word_size = 0x4000U;
    }else{
        /* Word is 16-bit size */
        dma_word_size = 0x0000U;
    }
    if(var->mode == DMA_CONTINUOUS){
        DMA_configMode(var->base, var->trg_source, DMA_CFG_CONTINUOUS_ENABLE | dma_word_size);
    }
    else if(var->mode == DMA_CONTINUOUS_ONESHOT){
        DMA_configMode(var->base, var->trg_source, DMA_CFG_ONESHOT_ENABLE | DMA_CFG_CONTINUOUS_ENABLE | dma_word_size);
    }
    else{
        DMA_configMode(var->base, var->trg_source, dma_word_size);
    }
    
    DMA_setInterruptMode(var->base, DMA_INT_AT_END);
    DMA_disableOverrunInterrupt(var->base);
    DMA_enableTrigger(var->base);
    DMA_startChannel(var->base);

    return 0;
}

/**************************************************************************************************
 * 
 * Generic DMA object definition
 * 
 *************************************************************************************************/
_DMA_OBJ_STRUCT(
    _DMA_OBJ_STRUCT_MEMBER(var);
);

/**************************************************************************************************
 * 
 * dma_setup()
 * 
 *************************************************************************************************/
int
dma_setup(const struct _dma *dma)
{
#if defined(F2803X)
    /* F2803X does not have DMA implemented */
    return -1;

#elif defined(F28004X)
    struct _dma_priv *priv = dma->priv;
    struct _dma_var * const *var = &dma->var;

    ASSERT(obj_check(dma, OBJ__DMA));

    int ret;

    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_DMA);
    /**
     * It takes 2-5 CPU cycles before peripheral clock is enabled. Configuration registers cannot
     * be modified before peripheral clock has been enabled!
     */
    asm(" RPT #5 || NOP");

    DMA_initController();

    unsigned i;
    for (i = 0U; i < priv->size; i++) {
        ASSERT(var[i]);

        if (!var[i]->enable) {
            continue;
        }

        ASSERT((var[i]->channel >= 1U) && (var[i]->channel <= 6U));

        var[i]->base = DMA_CH1_BASE + ((var[i]->channel - 1U) * (DMA_CH2_BASE - DMA_CH1_BASE));

        uint16_t mask = 1U << var[i]->channel;
        if ((priv->init & mask) == 0U) {
            ret = setup_dma_channel(var[i]);
            if (ret < 0) {
                return -1;
            } else {
                priv->init |= mask;
            }
        } else {
            return -1;
        }
    }

    return 0;

#endif
}
