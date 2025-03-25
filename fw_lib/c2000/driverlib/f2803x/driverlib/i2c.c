/**************************************************************************************************
 * 
 * \file i2c.c
 * 
 * \brief I2C driverlib implementation
 * 
 * \note Most of this driverlib has been adapted from f28004x driverlib.
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "i2c.h"

/**************************************************************************************************
 * 
 * I2C_initMaster()
 * 
 *************************************************************************************************/
void
I2C_initMaster(uint32_t base, uint32_t sysClkHz, uint32_t bitRate, I2C_DutyCycle dutyCycle)
{
    ASSERT(I2C_isBaseValid(base));
    ASSERT((10000000U / bitRate) > 10U);

    /* I2C peripheral clock is prescaled to produce I2C module clock at 10 MHz */
    uint32_t modPrescale = (sysClkHz / 10000000U) - 1U;

    /* Set I2C peripheral clock prescaler */
    HW_FLD_SET(base, I2C, PSC, IPSC, modPrescale);

    uint32_t dValue = (modPrescale == 0U) ? 7U : ((modPrescale == 1U) ? 6U : 5U);

    uint32_t divider = (10000000U / bitRate) - (2U * dValue);

    uint16_t rCLKH = 0U;

    if (dutyCycle == I2C_DUTYCYCLE_50) {
        rCLKH = divider / 2U;
    } else { /* I2C_DUTYCYCLE_33 */
        rCLKH = divider / 3U;
    }

    /* Set clock high- and low-time divide values */
    HW_REG_SET(base, I2C, CLKH, rCLKH);
    HW_REG_SET(base, I2C, CLKL, (divider - rCLKH));
}

/**************************************************************************************************
 * 
 * I2C_enableInterrupt()
 * 
 *************************************************************************************************/
void
I2C_enableInterrupt(uint32_t base, uint32_t intFlags)
{
    ASSERT(I2C_isBaseValid(base));

    /* Get I2CIER register current state */
    uint16_t rIER = HW_REG_GET(base, I2C, IER);

    /* Enable interrupt sources */
    HW_REG_SET(base, I2C, IER, (rIER | (intFlags & 0x003FU)));

    /**
     * Enable addressed-as-slave interrupt source separately because its bit position is
     * different between I2CIER and I2CSTR registers.
     */
    if (intFlags & I2C_INT_ADDR_SLAVE) {
        HW_BIT_SET(base, I2C, IER, AAS);
    }

    /* Enable transmit FIFO interrupt source */
    if (intFlags & I2C_INT_TXFF) {
        HW_BIT_SET(base, I2C, FFTX, TXFFIENA);
    }

    /* Enable receive FIFO interrupt source */
    if (intFlags & I2C_INT_RXFF) {
        HW_BIT_SET(base, I2C, FFRX, RXFFIENA);
    }
}

/**************************************************************************************************
 * 
 * I2C_disableInterrupt()
 * 
 *************************************************************************************************/
void
I2C_disableInterrupt(uint32_t base, uint32_t intFlags)
{
    ASSERT(I2C_isBaseValid(base));

    /* Get I2CIER register current state */
    uint16_t rIER = HW_REG_GET(base, I2C, IER);

    /* Disable interrupt sources */
    HW_REG_SET(base, I2C, IER, (rIER & ~(intFlags & 0x003FU)));

    /**
     * Disable addressed-as-slave interrupt source separately because its bit position is
     * different between I2CIER and I2CSTR registers.
     */
    if (intFlags & I2C_INT_ADDR_SLAVE) {
        HW_BIT_CLR(base, I2C, IER, AAS);
    }

    /* Disable transmit FIFO interrupt source */
    if (intFlags & I2C_INT_TXFF) {
        HW_BIT_CLR(base, I2C, FFTX, TXFFIENA);
    }

    /* Disable receive FIFO interrupt source */
    if (intFlags & I2C_INT_RXFF) {
        HW_BIT_CLR(base, I2C, FFRX, RXFFIENA);
    }
}

/**************************************************************************************************
 * 
 * I2C_getInterruptStatus()
 * 
 *************************************************************************************************/
uint32_t
I2C_getInterruptStatus(uint32_t base)
{
    ASSERT(I2C_isBaseValid(base));

    /* Read status flags associated with interrupts */
    uint32_t temp = HW_REG_GET(base, I2C, STR) & I2C_STR_INTMASK;

    /* Read transmit FIFO interrupt status flag */
    if (HW_BIT_GET(base, I2C, FFTX, TXFFINT)) {
        temp |= I2C_INT_TXFF;
    }

    /* Read receive FIFO interrupt status flag */
    if (HW_BIT_GET(base, I2C, FFRX, RXFFINT)) {
        temp |= I2C_INT_RXFF;
    }

    return temp;
}

/**************************************************************************************************
 * 
 * I2C_clearInterruptStatus()
 * 
 *************************************************************************************************/
void
I2C_clearInterruptStatus(uint32_t base, uint32_t intFlags)
{
    ASSERT(I2C_isBaseValid(base));

    /* Clear interrupt flags that are located in the I2CSTR register */
    HW_REG_SET(base, I2C, STR, ((uint16_t) intFlags & I2C_STR_INTMASK));

    /* Clear transmit FIFO interrupt flag */
    if (intFlags & I2C_INT_TXFF) {
        HW_BIT_SET(base, I2C, FFTX, TXFFINTCLR);
    }

    /* Clear receive FIFO interrupt flag */
    if (intFlags & I2C_INT_RXFF) {
        HW_BIT_SET(base, I2C, FFRX, RXFFINTCLR);
    }
}
