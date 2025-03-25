/**************************************************************************************************
 * 
 * \file gpio.h
 * 
 * \brief General-purpose IO driverlib interface
 * 
 * \note Most of this driverlib has been adapted from f28004x driverlib.
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef GPIO_H
#define GPIO_H

#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include "cpu.h"
#include "debug.h"

#include <stdint.h>
#include <stdbool.h>

/**************************************************************************************************
 * 
 * Useful defines used within the driver functions to access GPIO registers. They are not intended
 * for use by application code.
 * 
 * Divide by 2 is for C28x devices which have word (16-bit) access.
 * 
 *************************************************************************************************/

#define GPIO_NAME_CTRL          gpioCtrlReg
#define GPIO_NAME_DATA          gpioDataReg

#define GPIO_REG(TYPE, PORT)                                                                    \
    volatile uint32_t *GPIO_NAME_##TYPE =                                                       \
        (uint32_t *) ((uintptr_t) GPIO##TYPE##_BASE) + (PORT * GPIO_##TYPE##_REGS_STEP)

#define GPIO_REG_CTRL(PIN)      GPIO_REG(CTRL, (PIN / 32U))
#define GPIO_REG_DATA(PIN)      GPIO_REG(DATA, (PIN / 32U))

#define GPIO_CTRL_REGS_STEP     ((GPIO_O_GPBCTRL - GPIO_O_GPACTRL) / 2U)
#define GPIO_DATA_REGS_STEP     ((GPIO_O_GPBDAT - GPIO_O_GPADAT) / 2U)

#define GPIO_GPxCTRL_INDEX      (GPIO_O_GPACTRL / 2U)
#define GPIO_GPxQSEL_INDEX      (GPIO_O_GPAQSEL1 / 2U)
#define GPIO_GPxMUX_INDEX       (GPIO_O_GPAMUX1 / 2U)
#define GPIO_GPxDIR_INDEX       (GPIO_O_GPADIR / 2U)
#define GPIO_GPxPUD_INDEX       (GPIO_O_GPAPUD / 2U)

#define GPIO_GPxDAT_INDEX       (GPIO_O_GPADAT / 2U)
#define GPIO_GPxSET_INDEX       (GPIO_O_GPASET / 2U)
#define GPIO_GPxCLEAR_INDEX     (GPIO_O_GPACLEAR / 2U)
#define GPIO_GPxTOGGLE_INDEX    (GPIO_O_GPATOGGLE / 2U)

/**************************************************************************************************
 * 
 * Values that can be passed to \e GPIO_setPadConfig() as \e pinType parameter, and returned by
 * \e GPIO_getPadConfig().
 * 
 *************************************************************************************************/

#define GPIO_PIN_TYPE_STD       0x0000U     /* Pull-up disable for input                        */
#define GPIO_PIN_TYPE_PULLUP    0x0001U     /* Pull-up enable for input                         */

/**************************************************************************************************
 * 
 * Values that can be passed to \e GPIO_setDirectionMode() as \e pinIO parameter, and returned by
 * \e GPIO_getDirectionMode().
 * 
 *************************************************************************************************/
typedef enum {
    GPIO_DIR_MODE_IN,           /* GPIO pin is an input                                         */
    GPIO_DIR_MODE_OUT           /* GPIO pin is an output                                        */
} GPIO_Direction;

/**************************************************************************************************
 * 
 * Values that can be passed to \e GPIO_setQualificationMode() as \e qualification parameter, and
 * return by \e GPIO_getQualificationMode().
 * 
 *************************************************************************************************/
typedef enum {
    GPIO_QUAL_SYNC,             /* Synchronization to SYSCLK                                    */
    GPIO_QUAL_3SAMPLE,          /* Qualified with 3 samples                                     */
    GPIO_QUAL_6SAMPLE,          /* Qualified with 6 samples                                     */
    GPIO_QUAL_ASYNC             /* No synchronization                                           */
} GPIO_QualificationMode;

/**************************************************************************************************
 * 
 * Values that can be passed to \e GPIO_setAnalogMode() as \e mode parameter.
 * 
 *************************************************************************************************/
typedef enum {
    GPIO_ANALOG_DISABLED,       /* GPIO pin is in digital mode                                  */
    GPIO_ANALOG_ENABLED         /* GPIO pin is in analog mode                                   */
} GPIO_AnalogMode;

/**************************************************************************************************
 * 
 * Values that can be passed to \e GPIO_readPortData(), \e GPIO_setPortPins(),
 * \e GPIO_clearPortPins(), and \e GPIO_togglePortPins() as \e port parameter.
 * 
 *************************************************************************************************/
typedef enum {
    GPIO_PORT_A = 0,            /* GPIO port A                                                  */
    GPIO_PORT_B = 1,            /* GPIO port B                                                  */
    GPIO_PORT_D = 3             /* GPIO port D (analog pins)                                    */
} GPIO_Port;

/**************************************************************************************************
 * 
 * Prototypes for the APIs
 * 
 *************************************************************************************************/

/**************************************************************************************************
 * 
 * \brief Checks if GPIO pin is connected to analog subsystem
 * 
 * \param pin GPIO pin number
 * 
 * \note The analog signals are multiplexed with digital inputs and outputs (AIO section). The
 * AIO section is disabled by default, i.e. these pins are configured for analog operation.
 * 
 * \note This function is implemented in f2803x driverlib, but not in f28004x driverlib.
 * 
 * \return \b true if pin is connected to analog subsystem; \b false otherwise
 * 
 *************************************************************************************************/
static bool
GPIO_isPinAnalog(uint32_t pin)
{
    return ((pin == 98U) || (pin == 100U) || (pin == 102U) ||
        (pin == 106U) || (pin == 108U) || (pin == 110U));
}

#ifdef DEBUG
/**************************************************************************************************
 * 
 * \brief Checks if pin is a valid GPIO pin
 * 
 * \param pin GPIO pin number
 * 
 * \return \b true when pin is a valid GPIO pin; \b false otherwise
 * 
 *************************************************************************************************/
static inline bool
GPIO_isPinValid(uint32_t pin)
{
    return (pin <= 44U);
}
#endif

/**************************************************************************************************
 * 
 * \brief Reads a value on the specified GPIO pin
 * 
 * \param pin GPIO pin number
 * 
 * \details This function reads a value from the specified GPIO pin. The value is returned for
 * both input and output pins.
 * 
 * \return Value of the specified GPIO pin
 * 
 *************************************************************************************************/
static inline uint32_t
GPIO_readPin(uint32_t pin)
{
    ASSERT(GPIO_isPinValid(pin) || GPIO_isPinAnalog(pin));

    GPIO_REG_DATA(pin);
    
    return ((gpioDataReg[GPIO_GPxDAT_INDEX] >> (pin % 32U)) & ((uint32_t) 0x1U));
}

/**************************************************************************************************
 * 
 * \brief Writes a value to the specified GPIO pin
 * 
 * \param pin GPIO pin number
 * \param outVal value to write to the pin
 * 
 * \details This function writes the corresponding bit value to the output pin specified by the
 * \e pin parameter. Writing to a pin configured as an input pin has no effect.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
GPIO_writePin(uint32_t pin, uint32_t outVal)
{
    ASSERT(GPIO_isPinValid(pin) || GPIO_isPinAnalog(pin));

    GPIO_REG_DATA(pin);

    uint32_t pinMask = ((uint32_t) 1U) << (pin % 32U);

    if (outVal == 0U) {
        gpioDataReg[GPIO_GPxCLEAR_INDEX] = pinMask;
    } else {
        gpioDataReg[GPIO_GPxSET_INDEX] = pinMask;
    }
}

/**************************************************************************************************
 * 
 * \brief Toggles the specified GPIO pin
 * 
 * \param pin GPIO pin number
 * 
 * \details This function writes the corresponding bit value to the output pin specified by the
 * \e pin parameter. Writing to a pin configured as an input pin has no effect.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
GPIO_togglePin(uint32_t pin)
{
    ASSERT(GPIO_isPinValid(pin) || GPIO_isPinAnalog(pin));

    GPIO_REG_DATA(pin);

    uint32_t pinMask = ((uint32_t) 1U) << (pin % 32U);

    gpioDataReg[GPIO_GPxTOGGLE_INDEX] = pinMask;
}

/**************************************************************************************************
 * 
 * \brief Reads data on a GPIO port
 * 
 * \param port GPIO port being accessed
 * 
 * \details This function reads a value on the port specified by \e port parameter. The value is
 * a bit-packed parameter, where each bit represents a bit on the GPIO port. Bit 0 represents
 * GPIO port pin 0 etc.
 * 
 * \return GPIO port bit-packed value
 * 
 *************************************************************************************************/
static inline uint32_t
GPIO_readPortData(GPIO_Port port)
{
    GPIO_REG(DATA, ((uint32_t) port));

    return gpioDataReg[GPIO_GPxDAT_INDEX];
}

/**************************************************************************************************
 * 
 * \brief Writes a value to a GPIO port
 * 
 * \param port GPIO port being accessed
 * \param outVal value to write to the port
 * 
 * \details This function write a value specified by \e outVal parameter on the port specified by
 * \e port parameter. The \e outVal is a bit-packed value, where each bit represents a bit on the
 * GPIO port. Bit 0 represents GPIO port pin 0 etc.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
GPIO_writePortData(GPIO_Port port, uint32_t outVal)
{
    GPIO_REG(DATA, ((uint32_t) port));

    gpioDataReg[GPIO_GPxDAT_INDEX] = outVal;
}

/**************************************************************************************************
 * 
 * \brief Sets specified pins of a GPIO port
 * 
 * \param port GPIO port being accessed
 * \param pinMask bit-packed value with pins to be affected
 * 
 * \details This function sets all of the pins specified by \e pinMask parameter on the port
 * specified by \e port parameter. The \e pinMask is a bit-packed value, where each bit that
 * is set identifies the pin to be set. Bit 0 represents GPIO port pin 0 etc.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
GPIO_setPortPins(GPIO_Port port, uint32_t pinMask)
{
    GPIO_REG(DATA, ((uint32_t) port));

    gpioDataReg[GPIO_GPxSET_INDEX] = pinMask;
}

/**************************************************************************************************
 * 
 * \brief Clears specified pins of a GPIO port
 * 
 * \param port GPIO port being accessed
 * \param pinMask bit-packed value with pins to be affected
 * 
 * \details This function clears all of the pins specified by \e pinMask parameter on the port
 * specified by \e port parameter. The \e pinMask is a bit-packed value, where each bit that is
 * set identifies the pin to be cleared. Bit 0 represents GPIO port pin 0 etc.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
GPIO_clearPortPins(GPIO_Port port, uint32_t pinMask)
{
    GPIO_REG(DATA, ((uint32_t) port));

    gpioDataReg[GPIO_GPxCLEAR_INDEX] = pinMask;
}

/**************************************************************************************************
 * 
 * \brief Toggles specified pins of a GPIO port
 * 
 * \param port GPIO port being accessed
 * \param pinMask bit-packed value with pins to be affected
 * 
 * \details This function toggles all of the pins specified by \e pinMask parameter on the port
 * specified by \e port parameter. The \e pinMask is a bit-packed value, where each bit tha is
 * set identifies the pin to be toggled. Bit 0 represents GPIO port pin 0 etc.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
GPIO_togglePortPins(GPIO_Port port, uint32_t pinMask)
{
    GPIO_REG(DATA, ((uint32_t) port));

    gpioDataReg[GPIO_GPxTOGGLE_INDEX] = pinMask;
}

/**************************************************************************************************
 * 
 * \brief Sets direction mode of a GPIO pin
 * 
 * \param pin GPIO pin number
 * \param pinIO pin direction mode
 * 
 * \details This function configures a GPIO pin as either input or output.
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
GPIO_setDirectionMode(uint32_t pin, GPIO_Direction pinIO);

/**************************************************************************************************
 * 
 * \brief Gets direction mode of a GPIO pin
 * 
 * \param pin GPIO pin number
 * 
 * \return Direction mode as enumerated value from GPIO_Direction
 * 
 *************************************************************************************************/
extern GPIO_Direction
GPIO_getDirectionMode(uint32_t pin);

/**************************************************************************************************
 * 
 * \brief Sets pad configuration of a GPIO pin
 * 
 * \param pin GPIO pin number
 * \param pinType pin type specifier
 * 
 * \details This functions sets pad configuration of a GPIO pin. The \e pinType parameter can take
 * following values:
 * - \b GPIO_PIN_TYPE_PULLUP    - enables internal pull-up
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
GPIO_setPadConfig(uint32_t pin, uint32_t pinType);

/**************************************************************************************************
 * 
 * \brief Gets pad configuration of a GPIO pin
 * 
 * \param pin GPIO pin number
 * 
 * \return GPIO pin pad configuration
 * 
 *************************************************************************************************/
extern uint32_t
GPIO_getPadConfig(uint32_t pin);

/**************************************************************************************************
 * 
 * \brief Sets qualification mode of a GPIO pin
 * 
 * \param pin GPIO pin number
 * \param qualification enumerated value from GPIO_QualificationMode
 * 
 * \note To set the qualification sampling period use \e GPIO_setQualificationPeriod().
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
GPIO_setQualificationMode(uint32_t pin, GPIO_QualificationMode qualification);

/**************************************************************************************************
 * 
 * \brief Gets qualification mode of a GPIO pin
 * 
 * \param pin GPIO pin number
 * 
 * \return Qualification mode as enumerated value from GPIO_QualificationMode
 * 
 *************************************************************************************************/
extern GPIO_QualificationMode
GPIO_getQualificationMode(uint32_t pin);

/**************************************************************************************************
 * 
 * \brief Sets qualification period for a set of 8 pins
 * 
 * \param pin GPIO pin number
 * \param divider output drive strength
 * 
 * \details This functions sets qualification period for a set of \b 8 \b pins specified by \e pin
 * parameter. For instance, passing in 3 as the value of \e pin will set qualification period of
 * GPIO0 through GPIO7. In general, for a GPIO pin number \e n, qualification period will be set
 * for GPIO (n & ~7) through GPIO ((n & ~7) + 7). The \e divider parameter is the value by which
 * SYSCLKOUT frequency is divided. It can be 1 or an even value between 2 and 510 inclusive.
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
GPIO_setQualificationPeriod(uint32_t pin, uint32_t divider);

/**************************************************************************************************
 * 
 * \brief Sets analog mode of a GPIO pin
 * 
 * \param pin GPIO pin number
 * \param mode enumerated value from GPIO_AnalogMode
 * 
 * \details This function configures the GPIO pin for either analog or digital mode. Not all GPIO
 * pins can be switched to analog mode, refer to the technical reference manual for details. The
 * \e GPIO_isPinAnalog() can be used to check if a GPIO pin can be used in analog mode.
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
GPIO_setAnalogMode(uint32_t pin, GPIO_AnalogMode mode);

/**************************************************************************************************
 * 
 * \brief Configures alternate function of a GPIO pin
 * 
 * \param pinConfig pin configuration value defined by \b GPIO_#_?? macro
 * 
 * \details This function configures the pin mux that selects peripheral function associated with
 * the particular GPIO pin. Only one peripheral function at a time can be associated with a GPIO
 * pin. Despite the fact that many of peripheral functions can be associated with more than one
 * GPIO pin, it should only be associated with a single GPIO pin.
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
GPIO_setPinConfig(uint32_t pinConfig);

#endif /* GPIO_H */
