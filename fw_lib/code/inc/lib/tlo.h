/**************************************************************************************************
 * 
 * \file tlo.h
 * 
 * \brief Top-level object interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_LIB_TLO_H
#define _INC_LIB_TLO_H

/**************************************************************************************************
 * 
 * \brief Creates new top-level object
 * 
 * \param None
 * 
 * \return Top-level object handler; \b NULL in case of an error
 * 
 *************************************************************************************************/
extern const struct tlo *
tlo_new(void);

#endif /* _INC_LIB_TLO_H */
