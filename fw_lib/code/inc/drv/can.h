/**************************************************************************************************
 * 
 * \file can.h
 * 
 * \brief CAN driver interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_DRV_CAN_H
#define _INC_DRV_CAN_H

/**************************************************************************************************
 * 
 * Forward declarations
 * 
 *************************************************************************************************/

struct net;

/**************************************************************************************************
 * 
 * \brief Creates CAN communication driver via CAN-A module
 * 
 * \param None
 * 
 * \return Communication driver handler; \b NULL in case of failure
 * 
 *************************************************************************************************/
extern const struct net *
can_a_new(void);

#if defined(F28004X) 
/**************************************************************************************************
 * 
 * \brief Creates CAN communication driver via CAN-B module
 * 
 * \param None
 * 
 * \details This function creates \b net object to communicate with external systems via CAN
 * communication protocol.
 * 
 * \note f2803x devices do not have CAN-B module.
 * 
 * \return Communication driver handler; \b NULL in case of failure
 * 
 *************************************************************************************************/
extern const struct net *
can_b_new(void);
#endif /* F28004X */

#endif /* _INC_DRV_CAN_H */
