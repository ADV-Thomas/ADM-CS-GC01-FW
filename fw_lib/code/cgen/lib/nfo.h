/**************************************************************************************************
 * 
 * \file nfo.h
 * 
 * \brief Module data code generator
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _CGEN_LIB_NFO_H
#define _CGEN_LIB_NFO_H

/**************************************************************************************************
 * 
 * \brief Creates new module data object
 * 
 * \param NAME object name
 * \param ID module identifier
 * 
 *************************************************************************************************/
#define NFO_OBJ_NEW(NAME, ID)                                                                   \
    static struct nfo NAME = {                                                                  \
        .id         = ID,                                                                       \
        .serial     = NFO_SN_DEFAULT,                                                           \
        .version    = { 0U, 0U, 0U, 0U, 0U, 0U, 0U },                                           \
        .timestamp  = { 0U, 0U, 0U, 0U, 0U, 0U, 0U },                                           \
        .variant    = NFO_HW_DEFAULT,                                                           \
        .revision   = NFO_HW_DEFAULT,                                                           \
        .address    = 0U,                                                                       \
        .stack_size = 1U                                                                        \
    }

#endif /* _CGEN_LIB_NFO_H */
