/**************************************************************************************************
 * 
 * \file hal.h
 * 
 * \brief Hardware abstraction layer interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_HAL_HAL_H
#define _INC_HAL_HAL_H

#include <stdint.h>
#include <stdbool.h>

/**************************************************************************************************
 * 
 * Forward declarations
 * 
 *************************************************************************************************/

struct nfo;
struct net;

/**************************************************************************************************
 * 
 * \brief Initializes hardware abstraction layer
 * 
 * \param None
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
hal_init(void);

/**************************************************************************************************
 * 
 * \brief Setups HAL based on module data
 * 
 * \param mod module data object handler
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
hal_setup(const struct nfo *mod);

/**************************************************************************************************
 * 
 * \brief Resets CPU
 * 
 * \param None
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
hal_reset(void);

/**************************************************************************************************
 * 
 * \brief Triggers stopwatch
 * 
 * \param None
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
hal_tic(void);

/**************************************************************************************************
 * 
 * \brief Returns number of CPU cycles since stopwatch was last triggered with \e hal_tic()
 * 
 * \param None
 * 
 * \return Number of CPU cycles
 * 
 *************************************************************************************************/
extern uint32_t
hal_toc(void);

/**************************************************************************************************
 * 
 * \brief Returns number of CPU cycles since stopwatch was last triggered with \e hal_tic()
 * 
 * \param None
 * 
 * \return Number of CPU cycles
 * 
 *************************************************************************************************/
extern uint32_t
hal_get_cpu_busy(void);

/**************************************************************************************************
 * 
 * \brief Returns task scheduler tick frequency
 * 
 * \param None
 * 
 * \return Task scheduler tick frequency in Hz
 * 
 *************************************************************************************************/
extern uint32_t
hal_get_frequency(void);

/**************************************************************************************************
 * 
 * \brief Gets status of the task scheduler timer
 * 
 * \param None
 * 
 * \return \b true when timer has overflowed since last call
 * 
 *************************************************************************************************/
extern bool
hal_tick(void);

/**************************************************************************************************
 * 
 * \brief Turns microcontroller LED on or off
 * 
 * \param state \b true to turn on, \b false to turn off
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
hal_led(bool state);

/**************************************************************************************************
 * 
 * \brief Retrieves unique 32-bit ID from DSP
 * 
 * \param None
 * 
 * \return 32-bit unique ID
 * 
 *************************************************************************************************/
extern unsigned long
hal_get_SN(void);

/**************************************************************************************************
 * 
 * \brief Toggles microcontroller LED state
 * 
 * \param None
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
hal_led_toggle(void);

/**************************************************************************************************
 * 
 * \brief Gets primary CAN driver handler
 * 
 * \param None
 * 
 * \return Primary CAN driver handler
 * 
 *************************************************************************************************/
extern const struct net *
hal_get_can(void);

/**************************************************************************************************
 * 
 * \brief Gets Flash driver handler
 * 
 * \param None
 * 
 * \note Flash driver is used only in the bootloader application.
 * 
 * \return Flash driver handler
 * 
 *************************************************************************************************/
extern const struct net *
hal_get_flash(void);

/**************************************************************************************************
 * 
 * \brief Gets EEPROM driver handler
 * 
 * \param None
 * 
 * \return EEPROM driver handler
 * 
 *************************************************************************************************/
extern const struct net *
hal_get_eeprom(void);

/**************************************************************************************************
 * 
 * \brief Gets SPI driver handler
 * 
 * \param None
 * 
 * \return SPI driver handler
 * 
 *************************************************************************************************/
extern const struct net *
hal_get_spi(void);

/**************************************************************************************************
 * 
 * \brief Gets I2C driver handler
 * 
 * \param None
 * 
 * \return I2C driver handler
 * 
 *************************************************************************************************/
extern const struct net *
hal_get_i2c(void);

/**************************************************************************************************
 * 
 * \brief Gets I2C driver handler
 * 
 * \param None
 * 
 * \return I2C driver handler
 * 
 *************************************************************************************************/
extern const struct net *
hal_get_fsi(void);

#endif /* _INC_HAL_HAL_H */
