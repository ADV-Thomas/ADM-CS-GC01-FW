/**************************************************************************************************
 * 
 * \file hapi.h
 * 
 * \brief Hardware application interface code generator
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _CGEN_HAL_HAPI_H
#define _CGEN_HAL_HAPI_H

/**************************************************************************************************
 * 
 * \brief Initializes IO pin in HAPI object
 * 
 * \param NAME pin name
 * \param IO pin configuration code
 * 
 *************************************************************************************************/
#define _HAPI_MAP_INIT(NAME, IO)                                                                \
    _hapi->map->NAME = IO

#endif /* _CGEN_HAL_HAPI_H */
