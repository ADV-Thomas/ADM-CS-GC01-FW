/**************************************************************************************************
 * 
 * \file timer.h
 * 
 * \brief CPU timer driver interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_DRV_TIMER_H
#define _INC_DRV_TIMER_H

#include <stdint.h>
#include <stdbool.h>

/**************************************************************************************************
 * 
 * \brief Configures and starts CPU timer to operate at specified frequency
 * 
 * \param freq timer frequency in Hz
 * 
 * \details This function allocates first available CPU timer, and it configures it to operate at
 * frequency specified by \e freq parameter. Once configured, CPU timer is automatically started.
 * 
 * \note There are 3 32-bit CPU timers available on f2803x and f28004x devices.
 * 
 * \return Allocated CPU timer identifier; -1 CPU timer could not be allocated
 * 
 *************************************************************************************************/
extern int
timer_setup(uint32_t freq);

/**************************************************************************************************
 * 
 * \brief Stops and deallocates CPU timer
 * 
 * \param id CPU timer identifier (0, 1, or 2)
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
timer_free(unsigned id);

/**************************************************************************************************
 * 
 * \brief Gets current CPU timer status and clears overflow flag
 * 
 * \param id CPU timer identifier (0, 1, or 2)
 * 
 * \return \b true when CPU timer has overflowed; \b false otherwise. Returns \b false if timer
 * identifier is invalid.
 * 
 *************************************************************************************************/
extern bool
timer_status(unsigned id);

/**************************************************************************************************
 * 
 * \brief Gets current CPU timer count
 * 
 * \param id CPU timer identifier (0, 1, or 2)
 * 
 * \return Current CPU timer count. Returns 0 if timer identifier is invalid.
 * 
 *************************************************************************************************/
extern uint32_t
timer_state(unsigned id);

#endif /* _INC_DRV_TIMER_H */
