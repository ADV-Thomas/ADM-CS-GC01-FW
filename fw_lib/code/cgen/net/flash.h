/**************************************************************************************************
 * 
 * \file flash.h
 * 
 * \brief Flash communication interface code generator
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _CGEN_NET_FLASH_H
#define _CGEN_NET_FLASH_H

/**************************************************************************************************
 * 
 * \brief Initializes Flash sector object
 * 
 * \param ADDRESS sector start address
 * \param LENGTH sector length
 * 
 *************************************************************************************************/
#define FLASH_SECTOR_INIT(ADDRESS, LENGTH)                                                      \
    {   .address = ADDRESS,                                                                     \
        .length  = LENGTH                                                                       \
    }

#endif /* _CGEN_NET_FLASH_H */
