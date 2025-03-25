/**************************************************************************************************
 * 
 * \file gpio.c
 * 
 * \brief General-purpose IO driverlib implementation
 * 
 * \note Most of this driverlib has been adapted from f28004x driverlib.
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "gpio.h"

/**************************************************************************************************
 * 
 * GPIO_setDirectionMode()
 * 
 *************************************************************************************************/
void
GPIO_setDirectionMode(uint32_t pin, GPIO_Direction pinIO)
{
    ASSERT(GPIO_isPinValid(pin) || GPIO_isPinAnalog(pin));

    GPIO_REG_CTRL(pin);

    uint32_t pinMask = ((uint32_t) 1U) << (pin % 32U);

    EALLOW;

    if (pinIO == GPIO_DIR_MODE_OUT) {
        gpioCtrlReg[GPIO_GPxDIR_INDEX] |= pinMask;
    } else {
        gpioCtrlReg[GPIO_GPxDIR_INDEX] &= ~pinMask;
    }
    
    EDIS;
}

/**************************************************************************************************
 * 
 * GPIO_getDirectionMode()
 * 
 *************************************************************************************************/
GPIO_Direction
GPIO_getDirectionMode(uint32_t pin)
{
    ASSERT(GPIO_isPinValid(pin) || GPIO_isPinAnalog(pin));

    GPIO_REG_CTRL(pin);

    uint32_t pinMask = ((uint32_t) 1U) << (pin % 32U);

    return (GPIO_Direction) ((gpioCtrlReg[GPIO_GPxDIR_INDEX] & pinMask) != 0U);
}

/**************************************************************************************************
 * 
 * GPIO_setPadConfig()
 * 
 *************************************************************************************************/
void
GPIO_setPadConfig(uint32_t pin, uint32_t pinType)
{
    ASSERT(GPIO_isPinValid(pin));

    GPIO_REG_CTRL(pin);

    uint32_t pinMask = ((uint32_t) 1U) << (pin % 32U);

    EALLOW;

    if (pinType & GPIO_PIN_TYPE_PULLUP) {
        gpioCtrlReg[GPIO_GPxPUD_INDEX] &= ~pinMask;
    } else {
        gpioCtrlReg[GPIO_GPxPUD_INDEX] |= pinMask;
    }

    EDIS;
}

/**************************************************************************************************
 * 
 * GPIO_getPadConfig()
 * 
 *************************************************************************************************/
uint32_t
GPIO_getPadConfig(uint32_t pin)
{
    ASSERT(GPIO_isPinValid(pin));

    GPIO_REG_CTRL(pin);

    uint32_t pinMask = ((uint32_t) 1U) << (pin % 32U);

    uint32_t pinType = 0U;

    if ((gpioCtrlReg[GPIO_GPxPUD_INDEX] & pinMask) == 0U) {
        pinType |= GPIO_PIN_TYPE_PULLUP;
    }

    return pinType;
}

/**************************************************************************************************
 * 
 * GPIO_setQualificationMode()
 * 
 *************************************************************************************************/
void
GPIO_setQualificationMode(uint32_t pin, GPIO_QualificationMode qualification)
{
    ASSERT(GPIO_isPinValid(pin));

    GPIO_REG_CTRL(pin);

    uint32_t shiftAmt = (pin % 16U) * 2U;
    uint32_t qSelIndex = GPIO_GPxQSEL_INDEX + ((pin % 32U) / 16U);

    EALLOW;

    gpioCtrlReg[qSelIndex] &= ~(((uint32_t) 3U) << shiftAmt);
    gpioCtrlReg[qSelIndex] |= ((uint32_t) qualification) << shiftAmt;

    EDIS;
}

/**************************************************************************************************
 * 
 * GPIO_getQualificationMode()
 * 
 *************************************************************************************************/
GPIO_QualificationMode
GPIO_getQualificationMode(uint32_t pin)
{
    ASSERT(GPIO_isPinValid(pin));

    GPIO_REG_CTRL(pin);

    uint32_t shiftAmt = (pin % 16U) * 2U;
    uint32_t qSelIndex = GPIO_GPxQSEL_INDEX + ((pin % 32U) / 16U);

    return (GPIO_QualificationMode) ((gpioCtrlReg[qSelIndex] >> shiftAmt) & 0x3U);
}

/**************************************************************************************************
 * 
 * GPIO_setQualificationPeriod()
 * 
 *************************************************************************************************/
void
GPIO_setQualificationPeriod(uint32_t pin, uint32_t divider)
{
    ASSERT(GPIO_isPinValid(pin));
    ASSERT(divider <= 510U && (divider % 2U) == 0U);

    GPIO_REG_CTRL(pin);

    uint32_t shiftAmt = (pin % 32U) & ~((uint32_t) 0x7U);
    uint32_t pinMask = (uint32_t) 0xFFU << shiftAmt;

    uint32_t regVal = ((divider / 2U) << shiftAmt) & pinMask;

    EALLOW;

    gpioCtrlReg[GPIO_GPxCTRL_INDEX] &= ~pinMask;
    gpioCtrlReg[GPIO_GPxCTRL_INDEX] |= regVal;

    EDIS;
}

/**************************************************************************************************
 * 
 * GPIO_setAnalogMode()
 * 
 *************************************************************************************************/
void
GPIO_setAnalogMode(uint32_t pin, GPIO_AnalogMode mode)
{
    ASSERT(GPIO_isPinAnalog(pin));

    GPIO_REG_CTRL(pin);

    uint32_t pinMask = ((uint32_t) 3U) << ((pin % 16U) * 2U);

    EALLOW;

    if (mode == GPIO_ANALOG_ENABLED) {
        gpioCtrlReg[GPIO_GPxMUX_INDEX] |= pinMask;
    } else {
        gpioCtrlReg[GPIO_GPxMUX_INDEX] &= ~pinMask;
    }

    EDIS;
}

/**************************************************************************************************
 * 
 * GPIO_setPinConfig()
 * 
 *************************************************************************************************/
void
GPIO_setPinConfig(uint32_t pinConfig)
{
    uint32_t muxRegAddr = ((uint32_t) GPIOCTRL_BASE) + ((pinConfig >> 16) & ((uint32_t) 0xFFFFU));
    uint32_t shiftAmt = ((pinConfig >> 8) & ((uint32_t) 0xFFU));
    uint32_t pinMask = ((uint32_t) 0x3U) << shiftAmt;

    EALLOW;

    /* Clear fields in MUX register first to avoid glitches */
    HWREG(muxRegAddr) &= ~pinMask;

    HWREG(muxRegAddr) |= ((pinConfig & ((uint32_t) 0x3U)) << shiftAmt);

    EDIS;
}
