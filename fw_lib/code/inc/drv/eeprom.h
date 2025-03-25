/**************************************************************************************************
 * 
 * \file eeprom.h
 * 
 * \brief EEPROM driver interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_DRV_EEPROM_H
#define _INC_DRV_EEPROM_H

/**************************************************************************************************
 * 
 * Forward declarations
 * 
 *************************************************************************************************/

struct net;

/**************************************************************************************************
 * 
 * \brief Creates EEPROM communication driver
 * 
 * \param drv driver handler for physical communication
 * 
 * \details This function creates EEPROM communication driver. The driver wraps around another
 * communication driver that does physical communication (eg i2c, file etc.).
 * 
 * \return Communication driver handler; \b NULL in case of failure
 * 
 *************************************************************************************************/
extern const struct net *
eeprom_new(const struct net *drv);

#endif /* _INC_DRV_EEPROM_H */
