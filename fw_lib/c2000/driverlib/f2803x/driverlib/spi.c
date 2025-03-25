/**************************************************************************************************
 * 
 * \file spi.c
 * 
 * \brief SPI driverlib implementation
 * 
 * \note Most of this driverlib has been adapted from f28004x driverlib.
 * 
 * \author Jorge Sola
 * 
 *************************************************************************************************/

#include "spi.h"

//*****************************************************************************
//
// SPI_setConfig
//
//*****************************************************************************
void
SPI_setConfig(uint32_t base, uint32_t lspclkHz, SPI_TransferProtocol protocol,
              SPI_Mode mode, uint32_t bitRate, uint16_t dataWidth)
{
    uint32_t baud;

    //
    // Check the arguments.
    //
    ASSERT(SPI_isBaseValid(base));
    ASSERT(bitRate <= (lspclkHz / 4U));
    ASSERT((lspclkHz / bitRate) <= 128U);
    ASSERT((dataWidth >= 1U) && (dataWidth <= 16U));
    ASSERT((HWREGH(base + SPI_O_CCR) & SPI_CCR_SPISWRESET) == 0U);

    //
    // Set polarity and data width.
    //
    if(protocol == SPI_PROT_POL0PHA0 || protocol == SPI_PROT_POL0PHA1){
        HW_BIT_CLR(base,SPI,CCR,CLKPOLARITY);
    }else{
        HW_BIT_SET(base,SPI,CCR,CLKPOLARITY);
    }
    
    HW_FLD_SET(base,SPI,CCR,SPICHAR,(dataWidth - 1U));

    //
    // Set the mode and phase.
    //
    if(protocol == SPI_PROT_POL0PHA0 || protocol == SPI_PROT_POL1PHA0){
        HW_BIT_CLR(base,SPI,CTL,CLK_PHASE);
    }else{
        HW_BIT_SET(base,SPI,CTL,CLK_PHASE);
    }

    if(mode == SPI_MODE_SLAVE){
        HW_BIT_CLR(base,SPI,CTL,MASTER_SLAVE);
        HW_BIT_SET(base,SPI,CTL,TALK);
    }else if(mode == SPI_MODE_MASTER){
        HW_BIT_SET(base,SPI,CTL,MASTER_SLAVE);
        HW_BIT_SET(base,SPI,CTL,TALK);
    }else if(mode == SPI_MODE_SLAVE_OD){
        HW_BIT_CLR(base,SPI,CTL,MASTER_SLAVE);
        HW_BIT_CLR(base,SPI,CTL,TALK);
    }else{
        HW_BIT_SET(base,SPI,CTL,MASTER_SLAVE);
        HW_BIT_CLR(base,SPI,CTL,TALK);
    }


    //
    // Set the clock.
    //
    baud = (lspclkHz / bitRate) - 1U;
    HW_REG_SET(base,SPI,BRR,baud);
    HWREGH(base + SPI_O_BRR) = (uint16_t)baud;
}

//*****************************************************************************
//
// SPI_setBaudRate
//
//*****************************************************************************
void
SPI_setBaudRate(uint32_t base, uint32_t lspclkHz, uint32_t bitRate)
{
    uint32_t baud;

    //
    // Check the arguments.
    //
    ASSERT(SPI_isBaseValid(base));
    ASSERT(bitRate <= (lspclkHz / 4U));
    ASSERT((lspclkHz / bitRate) <= 128U);

    //
    // Set the clock.
    //
    baud = (lspclkHz / bitRate) - 1U;
    HW_REG_SET(base,SPI,BRR,baud);
}

//*****************************************************************************
//
// SPI_enableInterrupt
//
//*****************************************************************************
void
SPI_enableInterrupt(uint32_t base, uint32_t intFlags)
{
    //
    // Check the arguments.
    //
    ASSERT(SPI_isBaseValid(base));

    //
    // Enable the specified non-FIFO interrupts.
    //
    if((intFlags & SPI_INT_RX_DATA_TX_EMPTY))
    {
        HW_BIT_SET(base,SPI,CTL,SPIINTENA);
    }

    if((intFlags & SPI_INT_RX_OVERRUN))
    {
        HW_BIT_SET(base,SPI,CTL,OVERRUNINTENA);
    }

    //
    // Enable the specified FIFO-mode interrupts.
    //
    if((intFlags & SPI_INT_TXFF))
    {
        HW_BIT_SET(base,SPI,FFTX,TXFFIENA);
    }

    if((intFlags & (SPI_INT_RXFF | SPI_INT_RXFF_OVERFLOW)))
    {
        HW_BIT_SET(base,SPI,FFRX,RXFFIENA);
    }
}

//*****************************************************************************
//
// SPI_disableInterrupt
//
//*****************************************************************************
void
SPI_disableInterrupt(uint32_t base, uint32_t intFlags)
{
    //
    // Check the arguments.
    //
    ASSERT(SPI_isBaseValid(base));

    //
    // Disable the specified non-FIFO interrupts.
    //
    if((intFlags & SPI_INT_RX_DATA_TX_EMPTY))
    {
        HW_BIT_CLR(base,SPI,CTL,SPIINTENA);
    }

    if((intFlags & SPI_INT_RX_OVERRUN))
    {
        HW_BIT_CLR(base,SPI,CTL,OVERRUNINTENA);
    }

    //
    // Disable the specified FIFO-mode interrupts.
    //
    if((intFlags & SPI_INT_TXFF))
    {
        HW_BIT_CLR(base,SPI,FFTX,TXFFIENA);
    }

    if((intFlags & (SPI_INT_RXFF | SPI_INT_RXFF_OVERFLOW)))
    {
        HW_BIT_CLR(base,SPI,FFRX,RXFFIENA);
    }
}

//*****************************************************************************
//
// SPI_getInterruptStatus
//
//*****************************************************************************
uint32_t
SPI_getInterruptStatus(uint32_t base)
{
    uint32_t temp = 0;

    //
    // Check the arguments.
    //
    ASSERT(SPI_isBaseValid(base));

    if((HW_BIT_GET(base,SPI,STS,INT_FLAG)))
    {
        temp |= SPI_INT_RX_DATA_TX_EMPTY;
    }

    if((HW_BIT_GET(base,SPI,STS,OVERRUN_FLAG)))
    {
        temp |= SPI_INT_RX_OVERRUN;
    }

    if((HW_BIT_GET(base,SPI,FFTX,TXFFINT)))
    {
        temp |= SPI_INT_TXFF;
    }

    if((HW_BIT_GET(base,SPI,FFRX,RXFFINT)))
    {
        temp |= SPI_INT_RXFF;
    }

    if((HW_BIT_GET(base,SPI,FFRX,RXFFOVF)))
    {
        temp |= SPI_INT_RXFF_OVERFLOW;
    }

    return(temp);
}

//*****************************************************************************
//
// SPI_clearInterruptStatus
//
//*****************************************************************************
void
SPI_clearInterruptStatus(uint32_t base, uint32_t intFlags)
{
    //
    // Check the arguments.
    //
    ASSERT(SPI_isBaseValid(base));

    //
    // Clear the specified non-FIFO interrupt sources.
    //
    if((intFlags & SPI_INT_RX_DATA_TX_EMPTY))
    {
        HW_BIT_CLR(base,SPI,CCR,SPISWRESET);
        HW_BIT_SET(base,SPI,CCR,SPISWRESET);
    }

    if((intFlags & SPI_INT_RX_OVERRUN))
    {
        HW_BIT_SET(base,SPI,STS,OVERRUN_FLAG);
    }

    //
    // Clear the specified FIFO-mode interrupt sources.
    //
    if((intFlags & SPI_INT_TXFF))
    {
        HW_BIT_SET(base,SPI,FFTX,TXFFINTCLR);
    }

    if((intFlags & SPI_INT_RXFF))
    {
        HW_BIT_SET(base,SPI,FFRX,RXFFINTCLR);
    }

    if((intFlags & SPI_INT_RXFF_OVERFLOW))
    {
        HW_BIT_SET(base,SPI,FFRX,RXFFOVFCLR);
    }
}
