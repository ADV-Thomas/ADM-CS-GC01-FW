/**************************************************************************************************
 * 
 * \file dma.h
 * 
 * \brief DMA driver interface
 * 
 * \author Jorge Sola
 * 
 *************************************************************************************************/

#ifndef _INC_DRV_DMA_H
#define _INC_DRV_DMA_H

#include "driverlib/dma.h"

#include "cgen/drv/dma.h"
#include "inc/api/obj.h"
#include "inc/drv/io.h"

#include <stdint.h>
#include <stdbool.h>

enum dma_modes{
    DMA_CONTINUOUS = 0,     /* DMA performs transfers continuously every trigger */
    DMA_CONTINUOUS_ONESHOT, /* DMA performs transfers continously but needs to receiver trigger every burst */
    DMA_SINGLE_TRANSFER     /* DMA performs only one transfer and then the channel is disabled */
};

/**************************************************************************************************
 * 
 * Forward declarations
 * 
 *************************************************************************************************/

struct _dma;

/**************************************************************************************************
 * 
 * Private variables definition
 * 
 *************************************************************************************************/
OBJ_PRIV_STRUCT(_dma_priv,
    uint16_t init;                      /* Bit mask which indicates if DMA generator is in use */
);

/**************************************************************************************************
 * 
 * DMA variable definition
 * 
 *************************************************************************************************/
struct _dma_var {
    bool enable;                        /* Enables DMA variable if true                         */
    uint16_t channel;
    uint32_t base;
    uint16_t *src_addr;
    uint16_t *dest_addr;
    uint16_t trg_source;
    uint16_t words_per_burst;
    uint16_t word_step_src;
    uint16_t word_step_dest;
    uint16_t bursts_per_transfer;
    uint16_t burst_step_src;
    uint16_t burst_step_dest;
    uint16_t wrap_src;
    uint16_t wrap_dest;
    uint16_t mode;
    bool word_size;
};

/**************************************************************************************************
 * 
 * \brief Configures all DMA variables in the DMA object
 * 
 * \param dma DMA object handler
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
dma_setup(const struct _dma *dma);

#endif /* _INC_DRV_DMA_H */
