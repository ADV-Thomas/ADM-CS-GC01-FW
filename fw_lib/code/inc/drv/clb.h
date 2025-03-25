/**************************************************************************************************
 * 
 * \file clb.h
 * 
 * \brief CLB (Configurable Logic Block) interface
 * 
 * \author Jorge Sola
 * 
 *************************************************************************************************/

#ifndef _INC_DRV_CLB_H
#define _INC_DRV_CLB_H

#include <stdint.h>

void clb_init_for_fsi_sync(uint16_t core, uint32_t delay);

#endif
