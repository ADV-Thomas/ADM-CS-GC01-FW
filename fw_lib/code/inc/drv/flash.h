/**************************************************************************************************
 * 
 * \file flash.h
 * 
 * \brief Flash driver interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_DRV_FLASH_H
#define _INC_DRV_FLASH_H

/**************************************************************************************************
 * 
 * Forward declarations
 * 
 *************************************************************************************************/

struct net;

/**************************************************************************************************
 * 
 * \brief Creates on-chip Flash communication driver
 * 
 * \param drv driver handler for physical communication
 * 
 * \return Communication driver handler; \b NULL in case of failure
 * 
 *************************************************************************************************/
extern const struct net *
flash_new(const struct net *drv);

#endif /* _INC_DRV_FLASH_H */
