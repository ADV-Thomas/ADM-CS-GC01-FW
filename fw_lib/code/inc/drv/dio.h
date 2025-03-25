/**************************************************************************************************
 * 
 * \file dio.h
 * 
 * \brief Digital input-output driver interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_DRV_DIO_H
#define _INC_DRV_DIO_H

#include "inc/drv/io.h"

#include <stdbool.h>

/**************************************************************************************************
 * 
 * \brief Writes low or high to digital output
 * 
 * \param dio digital output pin configuration code
 * \param state output state
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
dio_write(enum io dio, bool state);

/**************************************************************************************************
 * 
 * \brief Toggles state on digital output
 * 
 * \param dio digital output pin configuration code
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
dio_toggle(enum io dio);

/**************************************************************************************************
 * 
 * \brief Reads digital input state
 * 
 * \param dio digital input pin configuration code
 * \param state storage for input pin state
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
dio_read(enum io dio, bool *state);

#endif /* _INC_DRV_DIO_H */
