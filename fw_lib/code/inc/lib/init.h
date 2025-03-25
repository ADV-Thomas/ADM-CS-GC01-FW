/**************************************************************************************************
 * 
 * \file init.h
 * 
 * \brief Common initializer interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_LIB_INIT_H
#define _INC_LIB_INIT_H

/**************************************************************************************************
 * 
 * Forward declarations
 * 
 *************************************************************************************************/

struct nfo;
struct mal;
struct net;

/**************************************************************************************************
 * 
 * \brief Common initialization for ADVANTICS power converters
 * 
 * \param mod module data object handler
 * \param boot bootloader data object handler
 * \param mal storage for MAL object handler
 * \param can storage for CAN driver handler
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
init(struct nfo *mod, struct nfo *boot, struct mal **mal, const struct net **can, unsigned long can_mask);

#endif /* _INC_LIB_INIT_H */
