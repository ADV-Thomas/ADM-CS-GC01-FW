/**************************************************************************************************
 * 
 * \file i2c.h
 * 
 * \brief I2C driverlib interface
 * 
 * \note Most of this driverlib has been adapted from f28004x driverlib.
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef I2C_H
#define I2C_H

#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include "cpu.h"
#include "debug.h"

#include <stdint.h>
#include <stdbool.h>

/**************************************************************************************************
 * 
 * Defines for the API
 * 
 *************************************************************************************************/

/**************************************************************************************************
 * 
 * I2C state configuration flags
 * 
 *************************************************************************************************/
#define I2C_MASTER_SEND_MODE        0x0600U     /* Master transmitter mode                      */
#define I2C_MASTER_RECEIVE_MODE     0x0400U     /* Master receiver mode                         */
#define I2C_SLAVE_SEND_MODE         0x0200U     /* Slave transmitter mode                       */
#define I2C_SLAVE_RECEIVE_MODE      0x0000U     /* Slave receiver mode                          */

#define I2C_REPEAT_MODE             0x0080U     /* Only applies to master mode                  */
#define I2C_START_BYTE_MODE         0x0010U     /* Enable start byte mode                       */
#define I2C_FREE_DATA_FORMAT        0x0008U     /* Enable free data (no address) format         */

#define I2C_MDR_CFGMASK ((uint16_t)(I2C_MASTER_SEND_MODE | I2C_REPEAT_MODE |                    \
    I2C_START_BYTE_MODE | I2C_FREE_DATA_FORMAT))

/**************************************************************************************************
 * 
 * Flags used in \e intFlags parameter of \e I2C_enableInterrupt(), \e I2C_disableInterrupt(), and
 * \e I2C_clearInterruptStatus(), and returned by \e I2C_getInterruptStatus()
 * 
 *************************************************************************************************/
#define I2C_INT_ARB_LOST            0x00001U    /* Arbitration lost (ARBL)                      */
#define I2C_INT_NO_ACK              0x00002U    /* No acknowledgment (NACK)                     */
#define I2C_INT_REG_ACCESS_RDY      0x00004U    /* Register access ready (ARDY)                 */
#define I2C_INT_RX_DATA_RDY         0x00008U    /* Receive data ready (RRDY)                    */
#define I2C_INT_TX_DATA_RDY         0x00010U    /* Transmit data ready (XRDY)                   */
#define I2C_INT_STOP_CONDITION      0x00020U    /* Stop condition detected (SCD)                */
#define I2C_INT_ADDR_SLAVE          0x00200U    /* Addressed as slave (AAS)                     */
#define I2C_INT_RXFF                0x10000U    /* Receive FIFO level                           */
#define I2C_INT_TXFF                0x20000U    /* Transmit FIFO level                          */

/* Used to mask out bits in the I2CSTR register that are not associated with interrupts */
#define I2C_STR_INTMASK ((uint16_t)(I2C_INT_ARB_LOST | I2C_INT_NO_ACK | I2C_INT_REG_ACCESS_RDY |\
    I2C_INT_RX_DATA_RDY | I2C_INT_TX_DATA_RDY | I2C_INT_STOP_CONDITION | I2C_INT_ADDR_SLAVE))

/**************************************************************************************************
 * 
 * Flags used in \e stsFlags parameter of \e I2C_clearStatus() and returned by \e I2C_getStatus().
 * Note that unlike on \b f28004x, \b BYTE_SENT status flag is not supported on \b f2803x devices.
 * 
 *************************************************************************************************/
#define I2C_STS_ARB_LOST            0x0001U     /* Arbitration lost (ARBL)                      */
#define I2C_STS_NO_ACK              0x0002U     /* No acknowledgment (NACK)                     */
#define I2C_STS_REG_ACCESS_RDY      0x0004U     /* Register access ready (ARDY)                 */
#define I2C_STS_RX_DATA_RDY         0x0008U     /* Receive data ready (RRDY)                    */
#define I2C_STS_TX_DATA_RDY         0x0010U     /* Transmit data ready (XRDY)                   */
#define I2C_STS_STOP_CONDITION      0x0020U     /* Stop condition detected (SCD)                */
#define I2C_STS_ADDR_ZERO           0x0100U     /* Address of all zeros detected (AD0)          */
#define I2C_STS_ADDR_SLAVE          0x0200U     /* Addressed as slave (AAS)                     */
#define I2C_STS_TX_EMPTY            0x0400U     /* Transmit shift register empty (XSMT)         */
#define I2C_STS_RX_FULL             0x0800U     /* Receive shift register full (RSFULL)         */
#define I2C_STS_BUS_BUSY            0x1000U     /* Bus busy, wait for STOP or reset (BB)        */
#define I2C_STS_NACK_SENT           0x2000U     /* NACK was sent (NACKSNT)                      */
#define I2C_STS_SLAVE_DIR           0x4000U     /* Addressed as slave transmitter (SDIR)        */

/**************************************************************************************************
 * 
 * I2C interrupts returned by \e I2C_getInterruptSource()
 * 
 *************************************************************************************************/
typedef enum {
    I2C_INTSRC_NONE,                    /* No interrupt pending                                 */
    I2C_INTSRC_ARB_LOST,                /* Arbitration lost (ARBL)                              */
    I2C_INTSRC_NO_ACK,                  /* No acknowledgment (NACK)                             */
    I2C_INTSRC_REG_ACCESS_RDY,          /* Register access ready (ARDY)                         */
    I2C_INTSRC_RX_DATA_RDY,             /* Receive data ready (RRDY)                            */
    I2C_INTSRC_TX_DATA_RDY,             /* Transmit data ready (XRDY)                           */
    I2C_INTSRC_STOP_CONDITION,          /* Stop condition detected (SCD)                        */
    I2C_INTSRC_ADDR_SLAVE               /* Addressed as slave transmitter (AAS)                 */
} I2C_InterruptSource;

/**************************************************************************************************
 * 
 * Values that can be passed to \e I2C_setFIFOInterruptLevel() as \e txLevel parameter, returned
 * by \e I2C_getFIFOInterruptLevel() in \e txLevel parameter and by \e I2C_getTxFIFOStatus()
 * 
 *************************************************************************************************/
typedef enum {
    I2C_FIFO_TXEMPTY = 0,               /* Transmit FIFO empty                                  */
    I2C_FIFO_TX0     = 0,               /* Transmit FIFO empty                                  */
    I2C_FIFO_TX1     = 1,               /* Transmit FIFO 1/4 full                               */
    I2C_FIFO_TX2     = 2,               /* Transmit FIFO 2/4 full                               */
    I2C_FIFO_TX3     = 3,               /* Transmit FIFO 3/4 full                               */
    I2C_FIFO_TX4     = 4,               /* Transmit FIFO full                                   */
    I2C_FIFO_TXFULL  = 4                /* Transmit FIFO full                                   */
} I2C_TxFIFOLevel;

/**************************************************************************************************
 * 
 * Values that can be passed to \e I2C_setFIFOInterruptLevel() as \e rxLevel parameter, returned
 * by \e I2C_getFIFOInterruptLevel() in \e rxLevel parameter and by \e I2C_getRxFIFOStatus()
 * 
 *************************************************************************************************/
typedef enum {
    I2C_FIFO_RXEMPTY = 0,               /* Receive FIFO empty                                   */
    I2C_FIFO_RX0     = 0,               /* Receive FIFO empty                                   */
    I2C_FIFO_RX1     = 1,               /* Receive FIFO 1/4 full                                */
    I2C_FIFO_RX2     = 2,               /* Receive FIFO 2/4 full                                */
    I2C_FIFO_RX3     = 3,               /* Receive FIFO 3/4 full                                */
    I2C_FIFO_RX4     = 4,               /* Receive FIFO full                                    */
    I2C_FIFO_RXFULL  = 4                /* Receive FIFO full                                    */
} I2C_RxFIFOLevel;

/**************************************************************************************************
 * 
 * Values that can be passed to \e I2C_setBitCount() as \e size parameter
 * 
 *************************************************************************************************/
typedef enum {
    I2C_BITCOUNT_1 = 1,                 /* 1 bit per data byte                                  */
    I2C_BITCOUNT_2 = 2,                 /* 2 bits per data byte                                 */
    I2C_BITCOUNT_3 = 3,                 /* 3 bits per data byte                                 */
    I2C_BITCOUNT_4 = 4,                 /* 4 bits per data byte                                 */
    I2C_BITCOUNT_5 = 5,                 /* 5 bits per data byte                                 */
    I2C_BITCOUNT_6 = 6,                 /* 6 bits per data byte                                 */
    I2C_BITCOUNT_7 = 7,                 /* 7 bits per data byte                                 */
    I2C_BITCOUNT_8 = 0                  /* 8 bits per data byte                                 */
} I2C_BitCount;

/**************************************************************************************************
 * 
 * Values that can be passed to \e I2C_setAddressMode() as \e mode parameter
 * 
 *************************************************************************************************/
typedef enum {
    I2C_ADDR_MODE_7BITS  = 0x0000U,     /* 7-bit address mode                                   */
    I2C_ADDR_MODE_10BITS = 0x0100U      /* 10-bit address mode                                  */
} I2C_AddressMode;

/**************************************************************************************************
 * 
 * Values that can be passed to \e I2C_setExtendedMode() as \e mode parameter. Note that unlike
 * on \b f28004x, forward compatibility is not supported on \b f2803x devices.
 * 
 *************************************************************************************************/
typedef enum {
    I2C_EADDR_MODE_BC = 1               /* Backwards compatibility                              */
} I2C_ExtendedMode;

/**************************************************************************************************
 * 
 * Values that can be passed to \e I2C_setEmulationMode() as \e mode parameter
 * 
 *************************************************************************************************/
typedef enum {
    I2C_EMULATION_STOP_SCL_LOW = 0x0000U,   /* Keep SCL low once it goes low in emulation mode  */
    I2C_EMULATION_FREE_RUN     = 0x4000U    /* Continue I2C operation in emulation mode         */
} I2C_EmulationMode;

/**************************************************************************************************
 * 
 * Values that can be passed to \e I2C_initMaster() as \e dutyCycle parameter
 * 
 *************************************************************************************************/
typedef enum {
    I2C_DUTYCYCLE_33,                   /* Clock duty cycle is 33%                              */
    I2C_DUTYCYCLE_50                    /* Clock duty cycle is 50%                              */
} I2C_DutyCycle;

/**************************************************************************************************
 * 
 * Prototypes for the APIs
 * 
 *************************************************************************************************/

#ifdef DEBUG
/**************************************************************************************************
 * 
 * \brief Checks I2C module base address
 * 
 * \param base base address of the I2C module
 * 
 * \return \b true if the I2C module base address is valid
 * 
 *************************************************************************************************/
static inline bool
I2C_isBaseValid(uint32_t base)
{
    return (base == I2CA_BASE);
}
#endif

/**************************************************************************************************
 * 
 * \brief Enables the I2C module
 * 
 * \param base base address of the I2C module
 * 
 * \note This has the effect of releasing the I2C bus if I2C peripheral is holding it.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
I2C_enableModule(uint32_t base)
{
    ASSERT(I2C_isBaseValid(base));

    HW_BIT_SET(base, I2C, MDR, IRS);
}

/**************************************************************************************************
 * 
 * \brief Disables the I2C module
 * 
 * \param base base address of the I2C module
 * 
 * \note When I2C peripheral is disabled, all status flags in the \b I2CSTR register are reset
 * to their default values.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
I2C_disableModule(uint32_t base)
{
    ASSERT(I2C_isBaseValid(base));

    HW_BIT_CLR(base, I2C, MDR, IRS);
}

/**************************************************************************************************
 * 
 * \brief Enables transmit and receive I2C FIFOs
 * 
 * \param base base address of the I2C module
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
I2C_enableFIFO(uint32_t base)
{
    ASSERT(I2C_isBaseValid(base));

    HW_BIT_SET(base, I2C, FFTX, I2CFFEN);
    HW_BIT_SET(base, I2C, FFTX, TXFFRST);
    HW_BIT_SET(base, I2C, FFRX, RXFFRST);
}

/**************************************************************************************************
 * 
 * \brief Disables transmit and receive I2C FIFOs
 * 
 * \param base base address of the I2C module
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
I2C_disableFIFO(uint32_t base)
{
    ASSERT(I2C_isBaseValid(base));

    HW_BIT_CLR(base, I2C, FFTX, I2CFFEN);
    HW_BIT_CLR(base, I2C, FFTX, TXFFRST);
    HW_BIT_CLR(base, I2C, FFRX, RXFFRST);
}

/**************************************************************************************************
 * 
 * \brief Sets FIFO level at which interrupts are generated
 * 
 * \param base base address of the I2C module
 * \param txLevel enumerated value from I2C_TxFIFOLevel
 * \param rxLevel enumerated value from I2C_RxFIFOLevel
 * 
 * \details This function sets FIFO level at which transmit and receive interrupts are generated.
 * The transmit FIFO interrupt flag will be set when FIFO reaches a value less than or equal to
 * \e txLevel. The receive FIFO interrupt flag will be set when FIFO reaches a value greater than
 * or equal to \e rxLevel.
 * 
 * \note If receive FIFO interrupt source is enabled before setting receive FIFO interrupt level,
 * an interrupt will be generated since default level value is 0. Configure interrupt level first
 * in order to avoid this behavior.
 * 
 * \note Transmit and receive FIFOs on this device are only 4 bytes deep!
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
I2C_setFIFOInterruptLevel(uint32_t base, I2C_TxFIFOLevel txLevel, I2C_RxFIFOLevel rxLevel)
{
    ASSERT(I2C_isBaseValid(base));
    ASSERT((uint16_t) txLevel <= 4U);
    ASSERT((uint16_t) rxLevel <= 4U);

    HW_FLD_SET(base, I2C, FFTX, TXFFIL, txLevel);
    HW_FLD_SET(base, I2C, FFRX, RXFFIL, rxLevel);
}

/**************************************************************************************************
 * 
 * \brief Gets FIFO level at which interrupts are generated
 * 
 * \param base base address of the I2C module
 * \param txLevel pointer to storage for the transmit FIFO level
 * \param rxLevel pointer to storage for the receive FIFO level
 * 
 * \details This function gets FIFO level at which transmit and receive interrupts are generated.
 * The transmit FIFO interrupt flag will be set when FIFO reaches a value less than or equal to
 * \e txLevel. The receive FIFO interrupt flag will be set when FIFO reaches a value greater than
 * or equal to \e rxLevel.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
I2C_getFIFOInterruptLevel(uint32_t base, I2C_TxFIFOLevel *txLevel, I2C_RxFIFOLevel *rxLevel)
{
    ASSERT(I2C_isBaseValid(base) && txLevel && rxLevel);

    *txLevel = (I2C_TxFIFOLevel) HW_FLD_GET(base, I2C, FFTX, TXFFIL);
    *rxLevel = (I2C_RxFIFOLevel) HW_FLD_GET(base, I2C, FFRX, RXFFIL);
}

/**************************************************************************************************
 * 
 * \brief Gets the transmit FIFO status
 * 
 * \param base base address of the I2C module
 * 
 * \return Current number of words (bytes) in the transmit FIFO
 * 
 *************************************************************************************************/
static inline I2C_TxFIFOLevel
I2C_getTxFIFOStatus(uint32_t base)
{
    ASSERT(I2C_isBaseValid(base));

    return (I2C_TxFIFOLevel) HW_FLD_GET(base, I2C, FFTX, TXFFST);
}

/**************************************************************************************************
 * 
 * \brief Gets the receive FIFO status
 * 
 * \param base base address of the I2C module
 * 
 * \return Current number of words (bytes) in the receive FIFO
 * 
 *************************************************************************************************/
static inline I2C_RxFIFOLevel
I2C_getRxFIFOStatus(uint32_t base)
{
    ASSERT(I2C_isBaseValid(base));

    return (I2C_RxFIFOLevel) HW_FLD_GET(base, I2C, FFRX, RXFFST);
}

/**************************************************************************************************
 * 
 * \brief Sets the address that I2C master places on the bus
 * 
 * \param base base address of the I2C module
 * \param slaveAddress 7-bit or 10-bit slave address
 * 
 * \details This function configures the address that I2C master places on the bus when initiating
 * a transaction.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
I2C_setSlaveAddress(uint32_t base, uint16_t slaveAddress)
{
    ASSERT(I2C_isBaseValid(base));
    ASSERT((slaveAddress & (I2C_SAR_SAR_M >> I2C_SAR_SAR_S)) == slaveAddress);

    HW_FLD_SET(base, I2C, SAR, SAR, slaveAddress);
}

/**************************************************************************************************
 * 
 * \brief Sets the slave address for this I2C module
 * 
 * \param base base address of the I2C module
 * \param slaveAddress 7-bit or 10-bit slave address
 * 
 * \details This function configures the I2C slave own address. The parameter \e slaveAddress is
 * the value that is compared against the slave address sent by an I2C master.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
I2C_setOwnSlaveAddress(uint32_t base, uint16_t slaveAddress)
{
    ASSERT(I2C_isBaseValid(base));
    ASSERT((slaveAddress & (I2C_OAR_OAR_M >> I2C_OAR_OAR_S)) == slaveAddress);

    HW_FLD_SET(base, I2C, OAR, OAR, slaveAddress);
}

/**************************************************************************************************
 * 
 * \brief Indicates if I2C bus is busy
 * 
 * \param base base address of the I2C module
 * 
 * \details This function returns an indication of whether or not the I2C bus is busy. This can be
 * used in a multi-master environment to determine if the bus is free for another data transfer.
 * 
 * \return \b true if the I2C is busy
 * 
 *************************************************************************************************/
static inline bool
I2C_isBusBusy(uint32_t base)
{
    ASSERT(I2C_isBaseValid(base));

    return HW_BIT_GET(base, I2C, STR, BB);
}

/**************************************************************************************************
 * 
 * \brief Gets the current I2C module status
 * 
 * \param base base address of the I2C module
 * 
 * \note \b BYTE_SENT status flag is not supported on this device.
 * 
 * \return Current module status enumerated as a bit field of
 * - \b I2C_STS_ARB_LOST        - Arbitration lost (ARBL)
 * - \b I2C_STS_NO_ACK          - No acknowledgment (NACK)
 * - \b I2C_STS_REG_ACCESS_RDY  - Register access ready (ARDY)
 * - \b I2C_STS_RX_DATA_RDY     - Receive data ready (RRDY)
 * - \b I2C_STS_TX_DATA_RDY     - Transmit data ready (XRDY)
 * - \b I2C_STS_STOP_CONDITION  - Stop condition detected (SCD)
 * - \b I2C_STS_ADDR_ZERO       - Address of all zeros detected (AD0)
 * - \b I2C_STS_ADDR_SLAVE      - Addressed as slave (AAS)
 * - \b I2C_STS_TX_EMPTY        - Transmit shift register empty (XSMT)
 * - \b I2C_STS_RX_FULL         - Receive shift register full (RSFULL)
 * - \b I2C_STS_BUS_BUSY        - Bus busy, wait for STOP or reset (BB)
 * - \b I2C_STS_NACK_SENT       - NACK was sent (NACKSENT)
 * - \b I2C_STS_SLAVE_DIR       - Addressed as slave transmitter (SDIR)
 * 
 *************************************************************************************************/
static inline uint16_t
I2C_getStatus(uint32_t base)
{
    ASSERT(I2C_isBaseValid(base));

    return HW_REG_GET(base, I2C, STR);
}

/**************************************************************************************************
 * 
 * \brief Clears I2C status flags
 * 
 * \param base base address of the I2C module
 * \param stsFlags bit mask of status flags to be cleared
 * 
 * \details This function clears the specified I2C status flags. The \e stsFlags parameter is
 * logical OR of the following values:
 * - \b I2C_STS_ARB_LOST
 * - \b I2C_STS_NO_ACK
 * - \b I2C_STS_REG_ACCESS_RDY
 * - \b I2C_STS_RX_DATA_RDY
 * - \b I2C_STS_STOP_CONDITION
 * - \b I2C_STS_NACK_SENT
 * - \b I2C_STS_SLAVE_DIR
 * 
 * \note Some of the status flags returned by \e I2C_getStatus() cannot be cleared by this
 * function. Some may only be cleared by hardware or a reset of the I2C module.
 * 
 * \note \b BYTE_SENT status flag is not supported on this device.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
I2C_clearStatus(uint32_t base, uint16_t stsFlags)
{
    ASSERT(I2C_isBaseValid(base));

    HW_REG_SET(base, I2C, STR, stsFlags);
}

/**************************************************************************************************
 * 
 * \brief Controls the I2C module state
 * 
 * \param base base address of the I2C module
 * \param config bit mask with I2C master configuration
 * 
 * \details This function controls the state of the master and slave send and receive operations.
 * The \e config parameter is logical OR of the following options:
 * 
 * One of the following four options:
 * - \b I2C_MASTER_SEND_MODE    - Master-transmitter mode
 * - \b I2C_MASTER_RECEIVE_MODE - Master-receiver mode
 * - \b I2C_MASTER_SEND_MODE    - Slave-transmitter mode
 * - \b I2C_MASTER_RECEIVE_MODE - Slave-receiver mode
 * 
 * Any of the following:
 * - \b I2C_REPEAT_MODE         - Sends data until stop bit is set, ignores data count
 * - \b I2C_START_BYTE_MODE     - Sends start byte (0x01U) after start condition
 * - \b I2C_FREE_DATA_FORMAT    - Use free data format, transfers have no address 
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
I2C_setConfig(uint32_t base, uint16_t config)
{
    ASSERT(I2C_isBaseValid(base));

    /* Read current I2C configuration */
    uint16_t rMDR = HW_REG_GET(base, I2C, MDR);

    /* Clear configuration bits (flags) */
    rMDR &= ~I2C_MDR_CFGMASK;

    /* Apply new configuration */
    rMDR |= (config & I2C_MDR_CFGMASK);

    /* Write new configuration */
    HW_REG_SET(base, I2C, MDR, rMDR);
}

/**************************************************************************************************
 * 
 * \brief Sets the data byte bit count
 * 
 * \param base base address of the I2C module
 * \param size enumerated value from I2C_BitCount
 * 
 * \details This functions sets the data byte bit count. The \e size parameter is an enumerated
 * value from I2C_BitCount. The maximum size is 8 bits (default).
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
I2C_setBitCount(uint32_t base, I2C_BitCount size)
{
    ASSERT(I2C_isBaseValid(base));

    HW_FLD_SET(base, I2C, MDR, BC, size);
}

/**************************************************************************************************
 * 
 * \brief Issues I2C START condition
 * 
 * \param base base address of the I2C module
 * 
 * \details This function causes the I2C module to generate a start condition. This function is
 * only valid when the I2C module operates as a master.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
I2C_sendStartCondition(uint32_t base)
{
    ASSERT(I2C_isBaseValid(base));

    HW_BIT_SET(base, I2C, MDR, STT);
}

/**************************************************************************************************
 * 
 * \brief Issues I2C STOP condition
 * 
 * \param base base address of the I2C module
 * 
 * \details This function causes the I2C module to generate a stop condition. This function is
 * only valid when the I2C module operates as a master.
 * 
 * \note The \e I2C_getStopConditionStatus() can be used to check status of the STOP condition.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
I2C_sendStopCondition(uint32_t base)
{
    ASSERT(I2C_isBaseValid(base));

    HW_BIT_SET(base, I2C, MDR, STP);
}

/**************************************************************************************************
 * 
 * \brief Issues I2C NACK bit
 * 
 * \param base base address of the I2C module
 * 
 * \details This function causes the I2C module to generate a NACK bit. This function is only
 * valid when the I2C module is acting as a receiver.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
I2C_sendNACK(uint32_t base)
{
    ASSERT(I2C_isBaseValid(base));

    HW_BIT_SET(base, I2C, MDR, NACKMOD);
}

/**************************************************************************************************
 * 
 * \brief Reads data byte from the data receive register
 * 
 * \param base base address of the I2C module
 * 
 * \return Byte received by the I2C module
 * 
 *************************************************************************************************/
static inline uint16_t
I2C_getData(uint32_t base)
{
    ASSERT(I2C_isBaseValid(base));

    return HW_FLD_GET(base, I2C, DRR, DATA);
}

/**************************************************************************************************
 * 
 * \brief Puts data byte to the data transmit register
 * 
 * \param base base address of the I2C module
 * \param data data byte to be transmitted
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
I2C_putData(uint32_t base, uint16_t data)
{
    ASSERT(I2C_isBaseValid(base));

    /**
     * Initially we used HW_FLD_SET() here, but that is wrong as it will first write zeros to
     * the DXR[DATA] field!
     */
    HW_REG_SET(base, I2C, DXR, data);
}

/**************************************************************************************************
 * 
 * \brief Gets stop condition status
 * 
 * \param base base address of the I2C module
 * 
 * \note This bit is automatically cleared after the stop condition has been generated.
 * 
 * \return \b true if the STP bit has been set by the device to generate a stop condition when the
 * internal data counter of the I2C module has reached 0. \b false when the STP bit is zero.
 * 
 *************************************************************************************************/
static inline bool
I2C_getStopConditionStatus(uint32_t base)
{
    ASSERT(I2C_isBaseValid(base));

    return HW_BIT_GET(base, I2C, MDR, STP);
}

/**************************************************************************************************
 * 
 * \brief Sets number of bytes to transfer or receive when repeat mode is off
 * 
 * \param base base address of the I2C module
 * \param count number of bytes to transfer or receive
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
I2C_setDataCount(uint32_t base, uint16_t count)
{
    ASSERT(I2C_isBaseValid(base));

    HW_REG_SET(base, I2C, CNT, count);
}

/**************************************************************************************************
 * 
 * \brief Sets the addressing mode to either 7-bit or 10-bit
 * 
 * \param base base address of the I2C module
 * \param mode enumerated value from I2C_AddressMode
 * 
 * \details This function configures the I2C module for either a 7-bit address (default) or a
 * 10-bit address mode, as defined by \e mode parameter.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
I2C_setAddressMode(uint32_t base, I2C_AddressMode mode)
{
    ASSERT(I2C_isBaseValid(base));

    if (mode == I2C_ADDR_MODE_7BITS) {
        HW_BIT_CLR(base, I2C, MDR, XA);
    } else { /* I2C_ADDR_MODE_10BITS */
        HW_BIT_SET(base, I2C, MDR, XA);
    }
}

/**************************************************************************************************
 * 
 * \brief Sets I2C eumlation mode
 * 
 * \param base base address of the I2C module
 * \param mode enumerated value from I2C_EmulationMode
 * 
 * \details This function sets behavior of the I2C operation when emulation suspend occurs. The
 * \e mode parameter can be one of the following:
 * - \b I2C_EMULATION_STOP_SCL_LOW - If SCL is low when breakpoint occurs, the I2C module stops
 *   immediately. If SCL is high, the I2C module waits until SCL becomes low and then stops.
 * - \b I2C_EMULATION_FREE_RUN - I2C operation continues regardless of breakpoint suspend.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
I2C_setEmulationMode(uint32_t base, I2C_EmulationMode mode)
{
    ASSERT(I2C_isBaseValid(base));
    
    if (mode == I2C_EMULATION_STOP_SCL_LOW) {
        HW_BIT_CLR(base, I2C, MDR, FREE);
    } else { /* I2C_EMULATION_FREE_RUN */
        HW_BIT_SET(base, I2C, MDR, FREE);
    }
}

/**************************************************************************************************
 * 
 * \brief Enables I2C loopback mode
 * 
 * \param base base address of the I2C module
 * 
 * \details This function enables loopback mode. This mode is only valid during master mode and is
 * helpful during device testing as it causes data transmitted out of the data transmit register
 * to be received in data receive register.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
I2C_enableLoopback(uint32_t base)
{
    ASSERT(I2C_isBaseValid(base));

    HW_BIT_SET(base, I2C, MDR, DLB);
}

/**************************************************************************************************
 * 
 * \brief Disables I2C loopback mode
 * 
 * \param base base address of the I2C module
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
I2C_disableLoopback(uint32_t base)
{
    ASSERT(I2C_isBaseValid(base));

    HW_BIT_CLR(base, I2C, MDR, DLB);
}

/**************************************************************************************************
 * 
 * \brief Returns the current I2C interrupt source
 * 
 * \param base base address of the I2C module
 * 
 * \details This function returns the event that generated the I2C basic (non-FIFO) interrupt.
 * Possible interrupt sources are as follows: \b I2C_INTSRC_NONE, \b I2C_INTSRC_ARB_LOST,
 * \b I2C_INTSRC_NO_ACK, \b I2C_INTSRC_REG_ACCESS_RDY, \b I2C_INTSRC_RX_DATA_RDY,
 * \b I2C_INTSRC_TX_DATA_RDY, \b I2C_INTSRC_STOP_CONDITION, and \b I2C_INTSRC_ADDR_SLAVE.
 * 
 * Calling this function will result in hardware automatically clearing the current interrupt
 * code (flag), and if ready, loading the next pending enabled interrupt. It will also clear
 * the corresponding flag in \b I2CSTR register if interrupt source is \b I2C_INTSRC_ARB_LOST,
 * \b I2C_INTSRC_NO_ACK, or \b I2C_INTSRC_STOP_CONDITION.
 * 
 * \note This function differs from the \e I2C_getInterruptSource() in that it returns a single
 * interrupt source. The \e I2C_getInterruptSource() returns the status of all interrupt flags
 * possible, including the flags that are not necessarily enabled to generate interrupts.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline I2C_InterruptSource
I2C_getInterruptSource(uint32_t base)
{
    ASSERT(I2C_isBaseValid(base));

    return (I2C_InterruptSource) HW_FLD_GET(base, I2C, ISRC, INTCODE);
}

/**************************************************************************************************
 * 
 * \brief Sets the compatibility mode to support I2C extended mode
 * 
 * \param base base address of the I2C module
 * \param mode enumerated value from I2C_ExtendedMode
 * 
 * \details This function configures the I2C module for backward compatibility.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
I2C_setExtendedMode(uint32_t base, I2C_ExtendedMode mode)
{
    ASSERT(I2C_isBaseValid(base));

    /** TODO: How to clear BC bit? */
    if (mode == I2C_EADDR_MODE_BC) {
        HW_BIT_SET(base, I2C, EMDR, BC);
    }
}

/**************************************************************************************************
 * 
 * \brief Initializes the I2C master
 * 
 * \param base base address of the I2C module
 * \param sysClkHz I2C peripheral clock rate in Hz
 * \param bitRate rate of the master clock signal (SCL) in Hz
 * \param dutyCycle enumerated value from I2C_DutyCycle
 * 
 * \details This function initializes operation of the I2C master by configuring the bus speed.
 * Note that the I2C module \b must be put into reset before calling this function. This can be
 * done by calling the \e I2C_disableModule() function.
 * 
 * A programmable prescaler in the I2C module divides down the I2C peripheral clock specified by
 * the \e sysClkHz parameter to produce I2C module clock of around 10 MHz. That clock is further
 * divided down to configure the master clock signal (SCL) to run at the rate specified by the
 * \e bitRate parameter. The \e dutyCycle parameter determines the high- and low-time percentage
 * of the clock signal. Valid values are \b I2C_DUTYCYCLE_33 for 33% and \b I2C_DUTYCYCLE_50 for
 * 50% duty cycles.
 * 
 * \note The I2C peripheral clock is derived from the system clock, rate of which is returned by
 * the \e SysCtl_getClock() function. If the system clock rate is constant and known it can be
 * explicitly hard coded to save execution overhead of a call to \e SysCtl_getClock().
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
I2C_initMaster(uint32_t base, uint32_t sysClkHz, uint32_t bitRate, I2C_DutyCycle dutyCycle);

/**************************************************************************************************
 * 
 * \brief Enables I2C interrupt sources
 * 
 * \param base base address of the I2C module
 * \param intFlags bit mask with interrupt sources
 * 
 * \details This function enables I2C interrupt sources. Only the sources that are enabled can
 * be reflected to the processor interrupt. Disabled sources have no effect on the processor.
 * 
 * The \e intFlags parameter is logical OR of any of the following:
 * - \b I2C_INT_ARB_LOST        - Arbitration lost
 * - \b I2C_INT_NO_ACK          - No acknowledgment (NACK)
 * - \b I2C_INT_REG_ACCESS_RDY  - Register access ready (ARDY)
 * - \b I2C_INT_RX_DATA_RDY     - Receive data ready
 * - \b I2C_INT_TX_DATA_RDY     - Transmit data ready
 * - \b I2C_INT_STOP_CONDITION  - Stop condition detected
 * - \b I2C_INT_ADDR_SLAVE      - Addressed as slave
 * - \b I2C_INT_RXFF            - Receive FIFO level
 * - \b I2C_INT_TXFF            - Transmit FIFO level
 * 
 * \note \b I2C_INT_RXFF and \b I2C_INT_TXFF are associated with the I2C FIFO interrupt vector.
 * All other flags are associated with the I2C basic interrupt.
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
I2C_enableInterrupt(uint32_t base, uint32_t intFlags);

/**************************************************************************************************
 * 
 * \brief Disables the I2C interrupt sources
 * 
 * \param base base address of the I2C module
 * \param intFlags bit mask with interrupt sources
 * 
 * \details This function disables I2C interrupt sources. Only the sources that are enabled can
 * be reflected to the processor interrupt. Disabled sources have no effect on the processor.
 * 
 * \note See \e I2C_enableInterrupt() for more details on the \e intFlags parameter.
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
I2C_disableInterrupt(uint32_t base, uint32_t intFlags);

/**************************************************************************************************
 * 
 * \brief Gets the current I2C interrupt status
 * 
 * \param base base address of the I2C module
 * 
 * \note This function will only return the status flags associated with interrupts. However, an
 * interrupt flag may be set even if its corresponding source is disabled.
 * 
 * \return Current interrupt status enumerated as a bit field of \b I2C_INT_ARB_LOST,
 * \b I2C_INT_NO_ACK, \b I2C_INT_REG_ACCESS_RDY, \b I2C_INT_RX_DATA_RDY, \b I2C_INT_TX_DATA_RDY,
 * \b I2C_INT_STOP_CONDITION, \b I2C_INT_ADDR_SLAVE, \b I2C_INT_RXFF, and \b I2C_INT_TXFF.
 * 
 *************************************************************************************************/
extern uint32_t
I2C_getInterruptStatus(uint32_t base);

/**************************************************************************************************
 * 
 * \brief Clears the I2C interrupt sources
 * 
 * \param base base address of the I2C module
 * \param intFlags bit mask with interrupt sources
 * 
 * \details This functions clears the I2C interrupt sources so that they are no longer asserted.
 * It must be called in the interrupt handler to keep the interrupt from being triggered again
 * immediately upon exit.
 * 
 * \note See \e I2C_enableInterrupt() for more details on the \e intFlags parameter.
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
I2C_clearInterruptStatus(uint32_t base, uint32_t intFlags);

#endif /* I2C_H */
