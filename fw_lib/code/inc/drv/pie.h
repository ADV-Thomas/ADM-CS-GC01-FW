/**************************************************************************************************
 * 
 * \file pie.h
 * 
 * \brief PIE driver interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_DRV_PIE_H
#define _INC_DRV_PIE_H

#include "driverlib/interrupt.h"

#include <stdint.h>

/**************************************************************************************************
 * 
 * \brief Initializes interrupt module
 * 
 * \param None
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
pie_init(void);

/**************************************************************************************************
 * 
 * \brief Registers function handler and enables interrupt line
 * 
 * \param code interrupt code
 * \param handler function handler
 * 
 * \note Valid values for the \e code parameter are defined in \e inc/hw_ints.h
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
pie_register(uint32_t code, void (*handler)(void));

/**************************************************************************************************
 * 
 * \brief Clears PIE flag
 * 
 * \param code interrupt code
 * 
 * \note Valid values for the \e code parameter are defined in \e inc/hw_ints.h
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
pie_clear(uint32_t code)
{
    uint16_t x = (code >> 8) & 0xFFU;

    ASSERT((x >= 1U) && (x <= 12U));

    Interrupt_clearACKGroup(1U << (x-1U));
}

#endif /* _INC_DRV_PIE_H */
