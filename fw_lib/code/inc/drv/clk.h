/**************************************************************************************************
 * 
 * \file clk.h
 * 
 * \brief System clock driver interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_DRV_CLK_H
#define _INC_DRV_CLK_H

#include <stdint.h>

/**************************************************************************************************
 * 
 * Clock object definition
 * 
 *************************************************************************************************/
struct clk {
    uint32_t Hz;                /* Clock frequency (Hz)                                         */
    uint32_t MHz;               /* Clock frequency (MHz)                                        */
};

/**************************************************************************************************
 * 
 * Clock object pack definition
 * 
 *************************************************************************************************/
struct _clk {
    struct clk xtal;            /* External crystal oscillator                                  */
    struct clk cpu;             /* CPU (system) clock                                           */
    struct clk can;             /* CAN peripheral clock                                         */
    struct clk pwm;             /* PWM peripheral clock                                         */
    struct clk adc;             /* ADC peripheral clock                                         */
};

/**************************************************************************************************
 * 
 * \brief Configures system clock to operate at maximum allowed frequency
 * 
 * \param None
 * 
 * \details This function configures system clock to operate at maximum allowed frequency, derived
 * from external crystal oscillator. It auto-detects crystal oscillator frequency by using two CPU
 * timers. Only 20 MHz and 30 MHz external crystal oscillators are supported.
 * 
 * \note 30 MHz external crystal oscillator frequency is outside of the recommended operating range
 * for f2803x and f28004x devices.
 * 
 * \note Maximum allowed frequency is 60 MHz for f2803x ,  100 MHz for f28004x devices, 200 MHz for f28p65x devices.
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
clk_init(void);

/**************************************************************************************************
 * 
 * \brief Returns clock object handler
 * 
 * \param None
 * 
 * \return Clock object pack handler; \b null if CPU clock is not configured
 * 
 *************************************************************************************************/
extern const struct _clk *
clk_get(void);

/**************************************************************************************************
 * 
 * \brief Sets ADC clock frequency
 * 
 * \param f ADC clock frequency (Hz)
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
clk_set_adc(uint32_t f);

/**************************************************************************************************
 * 
 * \brief Delays program by executing NOP instructions
 * 
 * \param time delay time in microseconds
 * 
 * \brief This function delays program by executing NOP instructions. It assumes that CPU clock is
 * configured to operate at its maximum frequency: 60 MHz on f2803x and 100 MHz on f28004x devices.
 * Minimum delay time is 1 us.
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
clk_delay(uint32_t time);

#endif /* _INC_DRV_CLK_H */
