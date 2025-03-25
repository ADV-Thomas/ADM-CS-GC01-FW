/**************************************************************************************************
 * 
 * \file crc.h
 * 
 * \brief CRC calculator interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_LIB_CRC_H
#define _INC_LIB_CRC_H

#include <stdint.h>

/**************************************************************************************************
 * 
 * \brief Calculates CRC for given data buffer
 * 
 * \param data data buffer
 * \param size data buffer size
 * 
 * \return CRC-16 value for data buffer
 * 
 *************************************************************************************************/
extern uint16_t
crc_calculate(const uint16_t *data, uint16_t size);

#endif /* _INC_LIB_CRC_H */
