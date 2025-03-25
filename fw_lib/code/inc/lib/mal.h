/**************************************************************************************************
 * 
 * \file mal.h
 * 
 * \brief Memory abstraction layer interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_LIB_MAL_H
#define _INC_LIB_MAL_H

#include <stdint.h>

/**************************************************************************************************
 * 
 * Forward declarations
 * 
 *************************************************************************************************/

struct mal;

struct net;
struct mem;

/**************************************************************************************************
 * 
 * User configuration data definition
 * 
 *************************************************************************************************/
struct mal_usr {
    const struct mem *mem;          /* Memory object handler                                    */
};

/**************************************************************************************************
 * 
 * \brief Creates new memory abstraction layer object
 * 
 * \param eeprom EEPROM driver handler
 * 
 * \return Memory abstraction layer object handler
 * 
 *************************************************************************************************/
extern struct mal *
mal_new(const struct net *eeprom);

extern struct mal *
mal_new_flash(const struct net *flash);

/**************************************************************************************************
 * 
 * \brief Opens external memory (EEPROM) driver
 * 
 * \param mal MAL object handler
 * \param usr user configuration data
 * 
 * \return 0 if connected successfully; -1 otherwise
 * 
 *************************************************************************************************/
extern int
mal_open(struct mal *mal, const struct mal_usr *usr);

/**************************************************************************************************
 * 
 * \brief Closes external memory (EEPROM) driver
 * 
 * \param mal MAL object handler
 * 
 * \return 0 if disconnected successfully; -1 otherwise
 * 
 *************************************************************************************************/
extern int
mal_close(struct mal *mal);

/**************************************************************************************************
 * 
 * \brief Erases whole memory
 * 
 * \param mal MAL object handler
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
mal_erase(struct mal *mal);

/**************************************************************************************************
 * 
 * \brief Erases single memory page
 * 
 * \param mal MAL object handler
 * \param num page number
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
mal_erase_page(struct mal *mal, uint16_t num);

/**************************************************************************************************
 * 
 * \brief Reads external memory serial number
 * 
 * \param mal MAL object handler
 * \param serial storage for serial number
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
mal_read_serial_number(struct mal *mal, uint32_t *serial);

/**************************************************************************************************
 * 
 * \brief Writes external memory serial number
 * 
 * \param mal MAL object handler
 * \param serial Input serial number to be written in memory
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
mal_write_serial_number(struct mal *mal, uint32_t serial);

/**************************************************************************************************
 * 
 * \brief Reads single page from memory to buffer
 * 
 * \param mal MAL object handler
 * \param num page number
 * \param data data buffer
 * \param size data buffer size
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
mal_read_page(struct mal *mal, uint16_t num, uint16_t *data, uint16_t size);

/**************************************************************************************************
 * 
 * \brief Reads single page from memory to buffer
 * 
 * \param mal MAL object handler
 * \param num page number
 * \param data data buffer
 * \param size data buffer size
 * 
 * \details This function reads page as raw bytes and does not perform any page header check.
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
mal_read_page_raw(struct mal *mal, uint16_t num, uint16_t *data, uint16_t size);

/**************************************************************************************************
 * 
 * \brief Writes single page to memory from buffer
 * 
 * \param mal MAL object handler
 * \param num memory page number
 * \param data data buffer
 * \param size data buffer size
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
mal_write_page(struct mal *mal, uint16_t num, const uint16_t *data, uint16_t size);

#endif /* _INC_LIB_MAL_H */
