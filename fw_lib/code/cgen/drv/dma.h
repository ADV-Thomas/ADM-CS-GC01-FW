/**************************************************************************************************
 * 
 * \file dma.h
 * 
 * \brief DMA driver code generator
 * 
 * \author Jorge Sola
 * 
 *************************************************************************************************/

#ifndef _CGEN_DRV_DMA_H
#define _CGEN_DRV_DMA_H

#include "cgen/api/obj.h"

/**************************************************************************************************
 * 
 * \brief Generates new static private variables structure
 * 
 * \param None
 * 
 * \details This function generates private variables structure for DMA driver object:
 * - variable type is 'struct _dma_priv'
 * - variable name is 'dma_priv'
 * - object structure type is 'struct _dma'
 * - object type is 'OBJ__DMA'
 * 
 *************************************************************************************************/
#define _DMA_PRIV_NEW()                                                                         \
    OBJ_PRIV_NEW(_dma_priv, dma_priv, _dma, OBJ__DMA,                                           \
        .init = 0U                                                                              \
    )

/**************************************************************************************************
 * 
 * \brief Generates new static DMA driver object
 * 
 * \param ... optional initializers
 * 
 * \details This function generates DMA driver object with its private variables structure:
 * - object structure type is 'struct _dma'
 * - object name is 'dma'
 * 
 *************************************************************************************************/
#define _DMA_OBJ_NEW(...)                                                                       \
    _DMA_PRIV_NEW();                                                                            \
    OBJ_NEW(_dma, dma, &dma_priv, __VA_ARGS__)

/**************************************************************************************************
 * 
 * \brief Generates new static DMA variable with default configuration data
 * 
 * \param NAME DMA variable name
 * 
 *************************************************************************************************/
#define _DMA_VAR_NEW(NAME)                                                                      \
    static struct _dma_var NAME##_dma = {                                                       \
        .enable             = false,                                                                       \
        .channel            = 0U,                                                                          \
        .base               = 0U,                                                                          \
        .src_addr           = 0,                                                                         \
        .dest_addr          = 0,                                                                         \
        .trg_source         = 0,                                                                       \
        .words_per_burst    = 0U,                                                                          \
        .word_step_src      = 0U,                                                                          \
        .word_step_dest     = 0U,                                                                          \
        .bursts_per_transfer= 0,                                                                       \
        .burst_step_src     = 0,                                                                        \
        .burst_step_dest    = 0U,                                                                          \
        .wrap_src           = 0,                                                                         \
        .wrap_dest          = 0,                                                                         \
        .mode               = 0,                                                                        \
        .word_size          = false                                                                         \
    }

/**************************************************************************************************
 * 
 * \brief Initializes DMA variable with configuration data
 * 
 * \param NAME DMA variable name
 * \param CH DMA Channel (1..6)
 * \param MODE Continuous, or one-shot mode
 * \param SRC_ADDR DMA Source address
 * \param DEST_ADDR DMA Destination address
 * \param TRG_SOURCE DMA Trigger source
 * \param WORDS_PER_BURST Number of words per burst (1 to 32)
 * \param WORD_STEP_SRC Amount to increment source address after each word transmitted
 * \param WORD_STEP_DEST Amount to increment destination address after each word transmitted
 * \param BURSTS_PER_TRANSFER Number of bursts per transfer (1 to 65536)
 * \param BURST_STEP_SRC Amount to increment source address after each burst transmitted
 * \param BURST_STEP_DEST Amount to increment destination address after each burst transmitted
 * \param WRAP_SRC Amount burst to be transmitted before a wrap on the source address
 * \param WRAP_DEST Amount burst to be transmitted before a wrap on the destination address
 * \param WORD_SIZE False for 16-bit word or True for 32-bit words
 * 
 *************************************************************************************************/
#define _DMA_VAR_INIT(NAME, CH, MODE, SRC_ADDR, DEST_ADDR, TRG_SOURCE, WORDS_PER_BURST, \
WORD_STEP_SRC, WORD_STEP_DEST, BURSTS_PER_TRANSFER, BURST_STEP_SRC, BURST_STEP_DEST,    \
WRAP_SRC, WRAP_DEST, WORD_SIZE)                                                         \
    *hapi->dma->NAME = (struct _dma_var) {                                                      \
        .enable                 = true,                                                                        \
        .channel                = CH,                                                                         \
        .base                   = 0U,                                                                          \
        .src_addr               = SRC_ADDR,                                                                        \
        .dest_addr              = DEST_ADDR,                                                                        \
        .trg_source             = TRG_SOURCE,                                                                     \
        .words_per_burst        = WORDS_PER_BURST,                                                                        \
        .word_step_src          = WORD_STEP_SRC,                                                                          \
        .word_step_dest         = WORD_STEP_DEST,                                                                     \
        .bursts_per_transfer    = BURSTS_PER_TRANSFER,                                                                     \
        .burst_step_src         = BURST_STEP_SRC,                                                                       \
        .burst_step_dest        = BURST_STEP_DEST,                                                                       \
        .wrap_src               = WRAP_SRC,                                                                       \
        .wrap_dest              = WRAP_DEST,                                                               \
        .mode                   = MODE,                                                                       \
        .word_size              = WORD_SIZE,                                                                               \
    }

/**************************************************************************************************
 * 
 * \brief Disables DMA variable
 * 
 * \param NAME DMA variable name
 * 
 *************************************************************************************************/
#define _DMA_VAR_DISABLE(NAME)                                                                  \
    hapi->dma->NAME->enable = false

/**************************************************************************************************
 * 
 * \brief Generates DMA object structure definition
 * 
 * \param ... optional structure members
 * 
 *************************************************************************************************/
#define _DMA_OBJ_STRUCT(...)                                                                    \
    OBJ_STRUCT(_dma, _dma_priv, __VA_ARGS__)

/**************************************************************************************************
 * 
 * \brief Adds variable to DMA object structure definition
 * 
 * \param NAME variable name
 * 
 *************************************************************************************************/
#define _DMA_OBJ_STRUCT_MEMBER(NAME)                                                            \
    OBJ_STRUCT_MEMBER(_dma_var, NAME)

/**************************************************************************************************
 * 
 * \brief Sets structure member to variable handler
 * 
 * \param NAME variable name
 * 
 *************************************************************************************************/
#define _DMA_MEMBER_SET(NAME)                                                                   \
    .NAME = &NAME##_dma

#endif /* _CGEN_DRV_DMA_H */
