/**************************************************************************************************
 * 
 * \file can.h
 * 
 * \brief CAN driverlib interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef CAN_H
#define CAN_H

#include "inc/hw_can.h"
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
 * \brief Calculates mailbox register offset
 * 
 * \param MBX mailbox identifier (1..32)
 * 
 *************************************************************************************************/
#define CAN_MBX_OFFSET(MBX)     (((MBX) - 1U) * ((CAN_O_MDH + CAN_S_MDH) - CAN_O_MSGID))

/**************************************************************************************************
 * 
 * The following definitions contain all error and status indicators that can be returned when
 * calling the \e CAN_getStatus() function.
 * 
 *************************************************************************************************/
#define CAN_STS_ERR_FE      CAN_ES_FE           /* Form error                                   */
#define CAN_STS_ERR_BE      CAN_ES_BE           /* Bit error                                    */
#define CAN_STS_ERR_SA1     CAN_ES_SA1          /* Stuck at dominant bit error                  */
#define CAN_STS_ERR_CRCE    CAN_ES_CRCE         /* CRC error                                    */
#define CAN_STS_ERR_SE      CAN_ES_SE           /* Stuff bit error                              */
#define CAN_STS_ERR_ACKE    CAN_ES_ACKE         /* Acknowledge error                            */
#define CAN_STS_ERR_BO      CAN_ES_BO           /* Bus-off status                               */
#define CAN_STS_ERR_EP      CAN_ES_EP           /* Error-pasive state                           */
#define CAN_STS_ERR_EW      CAN_ES_EW           /* Warning status for error counters            */
#define CAN_STS_ERR_SMA     CAN_ES_SMA          /* Suspend mode acknowledge                     */
#define CAN_STS_ERR_CCE     CAN_ES_CCE          /* Change configuration enabled                 */
#define CAN_STS_ERR_PDA     CAN_ES_PDA          /* Power-down mode acknowledge                  */
#define CAN_STS_ERR_RM      CAN_ES_RM           /* CAN module is in receive mode                */
#define CAN_STS_ERR_TM      CAN_ES_TM           /* CAN module is in transmit mode               */

/**************************************************************************************************
 * 
 * The following definitions contain all interrupt sources that can be passed as the \e intFlags
 * parameter in the \e CAN_enableInterrupt() and \e CAN_disableInterrupt() functions.
 * 
 *************************************************************************************************/
#define CAN_INT_SRC_I0      CAN_GIM_I0EN        /* Interrupt 0 line source                      */
#define CAN_INT_SRC_I1      CAN_GIM_I1EN        /* Interrupt 1 line source                      */
#define CAN_INT_SRC_WL      CAN_GIM_WLIM        /* Warning level interrupt source               */
#define CAN_INT_SRC_EP      CAN_GIM_EPIM        /* Error-passive interrupt source               */
#define CAN_INT_SRC_BO      CAN_GIM_BOIM        /* Bus-off interrupt source                     */
#define CAN_INT_SRC_RML     CAN_GIM_RMLIM       /* Received-message lost interrupt source       */
#define CAN_INT_SRC_WU      CAN_GIM_WUIM        /* Wake-up interrupt source                     */
#define CAN_INT_SRC_WD      CAN_GIM_WDIM        /* Write denied interrupt source                */
#define CAN_INT_SRC_AA      CAN_GIM_AAIM        /* Abort acknowledge interrupt source           */
#define CAN_INT_SRC_TCO     CAN_GIM_TCOM        /* Timestamp counter overflow interrupt source  */
#define CAN_INT_SRC_MTO     CAN_GIM_MTOM        /* Mailbox timeout interrupt source             */

/**************************************************************************************************
 * 
 * The following definitions contain all interrupt flags that can be returned when calling the
 * \e CAN_getInterruptSource() function.
 * 
 *************************************************************************************************/
#define CAN_INT_STS_WL      CAN_GIF0_WLIF       /* Warning level interrupt flag                 */
#define CAN_INT_STS_EP      CAN_GIF0_EPIF       /* Error-passive interrupt flag                 */
#define CAN_INT_STS_BO      CAN_GIF0_BOIF       /* Bus-off interrupt flag                       */
#define CAN_INT_STS_RML     CAN_GIF0_RMLIF      /* Received-message lost interrupt flag         */
#define CAN_INT_STS_WU      CAN_GIF0_WUIF       /* Wake-up interrupt flag                       */
#define CAN_INT_STS_WD      CAN_GIF0_WDIF       /* Write denied interrupt flag                  */
#define CAN_INT_STS_AA      CAN_GIF0_AAIF       /* Abort acknowledge interrupt flag             */
#define CAN_INT_STS_GM      CAN_GIF0_GMIF       /* Global mailbox interrupt flag                */
#define CAN_INT_STS_TCO     CAN_GIF0_TCOF       /* Timestamp counter overflow flag              */
#define CAN_INT_STS_MTO     CAN_GIF0_MTOF       /* Mailbox timeout interrupt flag               */

/**************************************************************************************************
 * 
 * The following definitions contain all interrupt flags that can be passed as the \e intFlags
 * parameter in the \e CAN_clearInterruptSource() function.
 * 
 *************************************************************************************************/
#define CAN_INT_CLR_WL      CAN_GIF0_WLIF       /* Warning level interrupt flag                 */
#define CAN_INT_CLR_EP      CAN_GIF0_EPIF       /* Error-passive interrupt flag                 */
#define CAN_INT_CLR_BO      CAN_GIF0_BOIF       /* Bus-off interrupt flag                       */
#define CAN_INT_CLR_WU      CAN_GIF0_WUIF       /* Wake-up interrupt flag                       */
#define CAN_INT_CLR_WD      CAN_GIF0_WDIF       /* Write denied interrupt flag                  */
#define CAN_INT_CLR_TCO     CAN_GIF0_TCOF       /* Timestamp counter overflow flag              */

/**************************************************************************************************
 * 
 * Useful definitions to mask all interrupt source and status flags
 * 
 *************************************************************************************************/

#define CAN_INT_SRC_MASK                                                                        \
    (CAN_INT_SRC_I0 | CAN_INT_SRC_I1 | CAN_INT_SRC_WL | CAN_INT_SRC_EP |                        \
     CAN_INT_SRC_BO | CAN_INT_SRC_RML | CAN_INT_SRC_WU | CAN_INT_SRC_WD |                       \
     CAN_INT_SRC_AA | CAN_INT_SRC_TCO | CAN_INT_SRC_MTO)

#define CAN_INT_STS_MASK                                                                        \
    (CAN_INT_STS_WL | CAN_INT_STS_EP | CAN_INT_STS_BO | CAN_INT_STS_RML | CAN_INT_STS_WU |      \
     CAN_INT_STS_WD | CAN_INT_STS_AA | CAN_INT_STS_GM | CAN_INT_STS_TCO | CAN_INT_STS_MTO)

#define CAN_INT_CLR_MASK                                                                        \
    (CAN_INT_CLR_WL | CAN_INT_CLR_EP | CAN_INT_CLR_BO |                                         \
     CAN_INT_CLR_WU | CAN_INT_CLR_WD | CAN_INT_CLR_TCO)

/**************************************************************************************************
 * 
 * Values that can be passed to the \e CAN_setDataByteOrder() as the \e byteOrder parameter.
 * 
 *************************************************************************************************/
typedef enum {
    CAN_BYTE_ORDER_MSB,         /* CANMDL MSB first, CANMDH LSB last (default on reset)         */
    CAN_BYTE_ORDER_LSB          /* CANMDL LSB first, CANMDH MSB last                            */
} CAN_ByteOrder;

/**************************************************************************************************
 * 
 * Values that can be passed to the \e CAN_setControllerMode() as the \e mode parameter.
 * 
 *************************************************************************************************/
typedef enum {
    CAN_MODE_STANDARD,          /* Standard CAN controller mode (default on reset)              */
    CAN_MODE_ENHANCED           /* Enhanced CAN controller mode                                 */
} CAN_ControllerMode;

/**************************************************************************************************
 * 
 * Values that can be passed to the \e CAN_setEmulationMode() as the \e mode parameter.
 * 
 *************************************************************************************************/
typedef enum {
    CAN_EMULATION_SOFT,         /* CAN module stops running in SUSPEND state (default on reset) */
    CAN_EMULATION_FREE          /* CAN module continues to run in SUSPEND state                 */
} CAN_EmulationMode;

/**************************************************************************************************
 * 
 * Values that can be passed to the \e CAN_setMailboxDirection() as the \e direction parameter.
 * 
 *************************************************************************************************/
typedef enum {
    CAN_DIR_TRANSMIT,           /* The mailbox is used as a transmit mailbox (default on reset) */
    CAN_DIR_RECEIVE             /* The mailbox is used as a receive mailbox                     */
} CAN_MailboxDirection;

/**************************************************************************************************
 * 
 * Values that can be passed to the \e CAN_setIdentifierMode() as the \e identifier parameter,
 * and returned by the \e CAN_getIdentifierMode().
 * 
 *************************************************************************************************/
typedef enum {
    CAN_ID_MODE_STANDARD,       /* Standard identifier mode (11 bits) (default on reset)        */
    CAN_ID_MODE_EXTENDED        /* Extended identifier mode (29 bits)                           */
} CAN_IdentifierMode;

/**************************************************************************************************
 * 
 * Values that can be passed to the \e CAN_setAnswerMode() as the \e mode parameter.
 * 
 *************************************************************************************************/
typedef enum {
    CAN_ANSWER_MODE_NORMAL,     /* Mailbox does not reply to remote requests (default on reset) */
    CAN_ANSWER_MODE_AUTO        /* Mailbox replies to remote requests                           */
} CAN_AnswerMode;

/**************************************************************************************************
 * 
 * Values that can be passed to \e CAN_setGlobalInterruptLine(), \e CAN_setMailboxInterruptLine(),
 * \e CAN_getInterruptSource(), \e CAN_getInterruptMailbox(), and \e CAN_clearInterruptSource() as
 * the \e line parameter. 
 * 
 *************************************************************************************************/
typedef enum {
    CAN_LINE_I0,                /* Interrupt line 0 (default on reset)                          */
    CAN_LINE_I1                 /* Interrupt line 1                                             */
} CAN_InterruptLine;

/**************************************************************************************************
 * 
 * Prototypes for the APIs
 * 
 *************************************************************************************************/

#ifdef DEBUG
/**************************************************************************************************
 * 
 * \brief Checks CAN module base address
 * 
 * \param base base address of the CAN module
 * 
 * \return \b true if the CAN module base address is valid
 * 
 *************************************************************************************************/
static inline bool
CAN_isBaseValid(uint32_t base)
{
    return (base == CANA_BASE);
}
#endif

/**************************************************************************************************
 * 
 * \brief Resets CAN module parameters to default values
 * 
 * \param base base address of the CAN module
 * 
 * \details This function resets CAN module non-protected parameters to their default values.
 * The mailbox contents and error counters are not modified. Pending and ongoing transmissions
 * are canceled without perturbing the communication.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_resetModule(uint32_t base)
{
    ASSERT(CAN_isBaseValid(base));

    EALLOW;

    HW_BIT_SET(base, CAN, MC, SRES);

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Enables self test mode
 * 
 * \param base base address of the CAN module
 * 
 * \details This function enables self test mode in which CAN module generates its own acknowledge
 * (ACK) signal, thus enabling operation without a bus connected to the module. The message is not
 * sent but read back and stored in the appropriate mailbox.
 * 
 * If no mailbox has been configured to receive a transmitted frame, the frame will be stored in
 * MBX0, even if MBX0 has not been configured for receive operations. If LAMs are configured such
 * that some mailboxes can receive and store data frames, a frame that does not satisfy acceptance
 * filter for any receive mailbox will be lost.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_enableTestMode(uint32_t base)
{
    ASSERT(CAN_isBaseValid(base));

    EALLOW;

    HW_BIT_SET(base, CAN, MC, STM);

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Disables self test mode
 * 
 * \param base base address of the CAN module
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_disableTestMode(uint32_t base)
{
    ASSERT(CAN_isBaseValid(base));

    EALLOW;

    HW_BIT_CLR(base, CAN, MC, STM);

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Enables automatic bus-on
 * 
 * \param base base address of the CAN module
 * 
 * \details This function enables automatic bus-on. After the bus-off state, the CAN module goes
 * back automatically into bus-on state after 128 x 11 recessive bits have been monitored.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_enableAutoBusOn(uint32_t base)
{
    ASSERT(CAN_isBaseValid(base));

    EALLOW;

    HW_BIT_SET(base, CAN, MC, ABO);

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Disables automatic bus-on
 * 
 * \param base base address of the CAN module
 * 
 * \details This function disables automatic bus-on. The bus-off state may only be exited after
 * 128 x 11 recessive bits on the bus and after having cleared the CCR bit.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_disableAutoBusOn(uint32_t base)
{
    ASSERT(CAN_isBaseValid(base));

    EALLOW;

    HW_BIT_CLR(base, CAN, MC, ABO);

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Enables wake up on bus activity
 * 
 * \param base base address of the CAN module
 * 
 * \details This function enables wake up from power-down mode after detecting any bus activity.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_enableWakeUp(uint32_t base)
{
    ASSERT(CAN_isBaseValid(base));

    EALLOW;

    HW_BIT_SET(base, CAN, MC, WUBA);

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Disables wake up on bus activity
 * 
 * \param base base address of the CAN module
 * 
 * \details This function disables wake up from power-down mode on bus activity. The module can
 * leave power-down mode only by clearing PDR bit.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_disableWakeUp(uint32_t base)
{
    ASSERT(CAN_isBaseValid(base));

    EALLOW;

    HW_BIT_CLR(base, CAN, MC, WUBA);

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Sets data byte order for message data registers
 * 
 * \param base base address of the CAN module
 * \param byteOrder LSB or MSB data byte order
 * 
 * \details This function sets data byte order for \b MDL and \b MDH message data registers. Data
 * is always transmitted or received from the CAN bus starting with byte 0. The only difference is
 * how are bytes organized in \b MDL and \b MDH registers:
 * - \b CAN_BYTE_ORDER_MSB      Data byte 0 is MSB in \b MDL; Data byte 7 is LSB in \b MDH
 * - \b CAN_BYTE_ORDER_LSB      Data byte 0 is LSB in \b MDL; Data byte 7 is MSB in \b MDH
 * 
 * The \b CAN_BYTE_ORDER_MSB is default on reset.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_setDataByteOrder(uint32_t base, CAN_ByteOrder byteOrder)
{
    ASSERT(CAN_isBaseValid(base));

    EALLOW;

    if (byteOrder == CAN_BYTE_ORDER_MSB) {
        HW_BIT_CLR(base, CAN, MC, DBO);
    } else { /* byteOrder == CAN_BYTE_ORDER_LSB */
        HW_BIT_SET(base, CAN, MC, DBO);
    }

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Starts power down mode
 * 
 * \param base base address of the CAN module
 * 
 * \note If an application sets TRSn bit for a mailbox and then immediately starts power down mode,
 * the CAN module will go into low-power mode without transmitting the data frame. This is because
 * it takes about 80 CPU cycles for the data to be transferred from the mailbox RAM to the transmit
 * buffer. The application has to ensure that any pending transmission has been completed before
 * starting power down mode. The TAn bit could be polled for transmission completion status.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_startPowerDown(uint32_t base)
{
    ASSERT(CAN_isBaseValid(base));

    EALLOW;

    HW_BIT_SET(base, CAN, MC, PDR);

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Stops power down mode
 * 
 * \param base base address of the CAN module
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_stopPowerDown(uint32_t base)
{
    ASSERT(CAN_isBaseValid(base));

    EALLOW;

    HW_BIT_CLR(base, CAN, MC, PDR);

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Triggers configuration register change request
 * 
 * \param base base address of the CAN module
 * 
 * \details This function triggers configuration register change request. Bit-timing configuration
 * register (CANBTC) and acceptance mask register (CANGAM, LAM[0], and LAM[3]) of the SCC can be
 * changed only when CCR bit is set. 
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_startConfigChange(uint32_t base)
{
    ASSERT(CAN_isBaseValid(base));

    EALLOW;

    HW_BIT_SET(base, CAN, MC, CCR);

    EDIS;

    uint32_t timeout = 0UL;

    /* Wait until CPU gets access to configuration registers */
    /** TODO: Is 10U loop cycles enough for the timeout!? */
    while (!HW_BIT_GET(base, CAN, ES, CCE)) {
        if (++timeout > 10UL) {
            ASSERT(false);
        }
    }
}

/**************************************************************************************************
 * 
 * \brief Clears configuration register change request
 * 
 * \param base base address of the CAN module
 * 
 * \note Configuration register change request is set by default (on reset). It should be cleared
 * only after the configuration timing register (CANBTC) has been configured.
 * 
 * \note The configuration register change request will also be set upon a bus-off condition if
 * the ABO bit is not set. The bus-off condition can be exited by clearing the request.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_stopConfigChange(uint32_t base)
{
    ASSERT(CAN_isBaseValid(base));

    EALLOW;

    HW_BIT_CLR(base, CAN, MC, CCR);

    EDIS;

    uint32_t timeout = 0UL;

    /* Wait until CPU losses access to configuration registers */
    /** TODO: Is 10U cycles enough for the timeout!? */
    while (HW_BIT_GET(base, CAN, ES, CCE)) {
        if (++timeout > 10UL) {
            ASSERT(false);
        }
    }
}

/**************************************************************************************************
 * 
 * \brief Sets CAN controller mode
 * 
 * \param base base address of the CAN module
 * \param mode eCAN or SCC CAN controller mode
 * 
 * \details This function sets standard or extended CAN controller mode. In standard mode, only
 * mailboxes 15 to 0 can be used and timestamping feature is not available.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_setControllerMode(uint32_t base, CAN_ControllerMode mode)
{
    ASSERT(CAN_isBaseValid(base));

    EALLOW;

    if (mode == CAN_MODE_STANDARD) {
        HW_BIT_CLR(base, CAN, MC, SCB);
    } else { /* mode == CAN_MODE_EXTENDED */
        HW_BIT_SET(base, CAN, MC, SCB);
    }

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Sets behavior in emulation mode
 * 
 * \param base base address of the CAN module
 * \param mode behavior in emulation mode
 * 
 * \details This function sets the CAN module behavior in emulation mode:
 * - \b CAN_EMULATION_SOFT      CAN module stops running while in SUSPEND state (default on reset)
 * - \b CAN_EMULATION_FREE      CAN module continues to run while in SUSPEND state
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_setEmulationMode(uint32_t base, CAN_EmulationMode mode)
{
    ASSERT(CAN_isBaseValid(base));

    if (mode == CAN_EMULATION_SOFT) {
        HW_BIT_CLR(base, CAN, MC, SUSP);
    } else { /* mode == CAN_EMULATION_FREE */
        HW_BIT_SET(base, CAN, MC, SUSP);
    }
}

/**************************************************************************************************
 * 
 * \brief Gets error and status flags
 * 
 * \param base base address of the CAN module
 * 
 * \details This function returns error and status flags as logical OR of any of the following:
 * - \b CAN_STS_ERR_FE          Form error
 * - \b CAN_STS_ERR_BE          Bit error
 * - \b CAN_STS_ERR_SA1         Stuck at dominant bit error
 * - \b CAN_STS_ERR_CRCE        CRC error
 * - \b CAN_STS_ERR_SE          Stuff bit error
 * - \b CAN_STS_ERR_ACKE        Acknowledge error
 * - \b CAN_STS_ERR_BO          Bus-off status
 * - \b CAN_STS_ERR_EP          Error-passive state
 * - \b CAN_STS_ERR_EW          Warning status for error counters
 * - \b CAN_STS_ERR_SMA         Suspend mode acknowledge
 * - \b CAN_STS_ERR_CCE         Change configuration enabled
 * - \b CAN_STS_ERR_PDA         Power-down mode acknowledge
 * - \b CAN_STS_ERR_RM          CAN module is in receive mode
 * - \b CAN_STS_ERR_TM          CAN module is in transmit mode
 * 
 * \return Error and status flags
 * 
 *************************************************************************************************/
static inline uint32_t
CAN_getStatus(uint32_t base)
{
    ASSERT(CAN_isBaseValid(base));

    return HW_REG_GET(base, CAN, ES);
}

/**************************************************************************************************
 * 
 * \brief Enables CANTX pin to be used for CAN transmit functions
 * 
 * \param base base address of the CAN module
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_enableTxPin(uint32_t base)
{
    ASSERT(CAN_isBaseValid(base));

    EALLOW;

    HW_REG_SET(base, CAN, TIOC, 0x8UL);

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Enables CANRX pin to be used for CAN receive functions
 * 
 * \param base base address of the CAN module
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_enableRxPin(uint32_t base)
{
    ASSERT(CAN_isBaseValid(base));

    EALLOW;

    HW_REG_SET(base, CAN, RIOC, 0x8UL);

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Enables mailbox
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * 
 * \note The mailbox must be disabled before writing to the contents of any identifier field. If
 * the mailbox is enabled, write access to the identifier is denied and a write-denied interrupt
 * is generated.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_enableMailbox(uint32_t base, uint16_t mbx)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT(mbx >= 1U && mbx <= 32U);

    HW_REG_BIT_SET(base, CAN, ME, mbx-1U);
}

/**************************************************************************************************
 * 
 * \brief Disables mailbox
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * 
 * \details This function disables mailbox. After power-up, all mailboxes are disabled by default.
 * Disabled mailboxes can be used as additional memory for the CPU.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_disableMailbox(uint32_t base, uint16_t mbx)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT(mbx >= 1U && mbx <= 32U);

    HW_REG_BIT_CLR(base, CAN, ME, mbx-1U);
}

/**************************************************************************************************
 * 
 * \brief Sets mailbox direction
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * \param direction transmit or receive mailbox direction
 * 
 * \details This function sets mailbox direction:
 * - \b CAN_DIR_TRANSMIT    The mailbox is used as a transmit mailbox (default on reset)
 * - \b CAN_DIR_RECEIVE     The mailbox is used as a receive mailbox
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_setMailboxDirection(uint32_t base, uint16_t mbx, CAN_MailboxDirection direction)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT(mbx >= 1U && mbx <= 32U);

    if (direction == CAN_DIR_TRANSMIT) {
        HW_REG_BIT_CLR(base, CAN, MD, mbx-1U);
    } else { /* direction == CAN_DIR_RECEIVE */
        HW_REG_BIT_SET(base, CAN, MD, mbx-1U);
    }
}

/**************************************************************************************************
 * 
 * \brief Triggers mailbox transmission request
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_setTransmissionRequest(uint32_t base, uint16_t mbx)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT(mbx >= 1U && mbx <= 32U);

    HW_REG_SET_BIT(base, CAN, TRS, mbx-1U);
}

/**************************************************************************************************
 * 
 * \brief Gets mailbox transmission request status
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * 
 * \return \b true when transmission is still pending
 * 
 *************************************************************************************************/
static inline bool
CAN_getTransmissionRequest(uint32_t base, uint16_t mbx)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT(mbx >= 1U && mbx <= 32U);

    return HW_REG_BIT_GET(base, CAN, TRS, mbx-1U);
}

/**************************************************************************************************
 * 
 * \brief Resets (cancels) mailbox transmission request
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_resetTransmissionRequest(uint32_t base, uint16_t mbx)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT(mbx >= 1U && mbx <= 32U);

    HW_REG_SET_BIT(base, CAN, TRR, mbx-1U);
}

/**************************************************************************************************
 * 
 * \brief Gets mailbox transmission-acknowledge status
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * 
 * \return \b true when mailbox message has been successfully sent
 * 
 *************************************************************************************************/
static inline bool
CAN_getTransmissionAcknowledge(uint32_t base, uint16_t mbx)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT(mbx >= 1U && mbx <= 32U);

    return HW_REG_BIT_GET(base, CAN, TA, mbx-1U);
}

/**************************************************************************************************
 * 
 * \brief Clears mailbox transmission-acknowledge flag
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_clearTransmissionAcknowledge(uint32_t base, uint16_t mbx)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT(mbx >= 1U && mbx <= 32U);

    HW_REG_SET_BIT(base, CAN, TA, mbx-1U);
}

/**************************************************************************************************
 * 
 * \brief Gets mailbox abort-acknowledge status
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * 
 * \return \b true when mailbox message transmission has been aborted
 * 
 *************************************************************************************************/
static inline bool
CAN_getAbortAcknowledge(uint32_t base, uint16_t mbx)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT(mbx >= 1U && mbx <= 32U);

    return HW_REG_BIT_GET(base, CAN, AA, mbx-1U);
}

/**************************************************************************************************
 * 
 * \brief Clears mailbox abort-acknowledge flag
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_clearAbortAcknowledge(uint32_t base, uint16_t mbx)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT(mbx >= 1U && mbx <= 32U);

    HW_REG_SET_BIT(base, CAN, AA, mbx-1U);
}

/**************************************************************************************************
 * 
 * \brief Gets received-message pending status
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * 
 * \return \b true when mailbox contains received message
 * 
 *************************************************************************************************/
static inline uint32_t
CAN_getReceivedMessagePending(uint32_t base, uint16_t mbx)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT(mbx >= 1U && mbx <= 32U);

    return HW_REG_BIT_GET(base, CAN, RMP, mbx-1U);
}

/**************************************************************************************************
 * 
 * \brief Clears received-message pending flag
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * 
 * \note This will also clear corresponding status flag in the \b RML register.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_clearReceivedMessagePending(uint32_t base, uint16_t mbx)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT(mbx >= 1U && mbx <= 32U);

    HW_REG_SET_BIT(base, CAN, RMP, mbx-1U);
}

/**************************************************************************************************
 * 
 * \brief Gets received-message lost status
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * 
 * \note This status flag is cleared by clearing the received-message pending flag.
 * 
 * \return \b true when an old (unread) message has been overwritten by a new message
 * 
 *************************************************************************************************/
static inline bool
CAN_getReceivedMessageLost(uint32_t base, uint16_t mbx)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT(mbx >= 1U && mbx <= 32U);

    return HW_REG_BIT_GET(base, CAN, RML, mbx-1U);
}

/**************************************************************************************************
 * 
 * \brief Gets remote-frame pending status
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * 
 * \note If a remote frame is stored in a receive mailbox (AAM=0, CANMD=1), the corresponding
 * remote-frame pending status flag will not be set.
 * 
 * \return \b true when a remote-frame request was received by the module
 * 
 *************************************************************************************************/
static inline bool
CAN_getRemoteFramePending(uint32_t base, uint16_t mbx)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT(mbx >= 1U && mbx <= 32U);

    return HW_REG_BIT_GET(base, CAN, RFP, mbx-1U);
}

/**************************************************************************************************
 * 
 * \brief Clears remote-frame pending flag
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_clearRemoteFramePending(uint32_t base, uint16_t mbx)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT(mbx >= 1U && mbx <= 32U);

    HW_REG_SET_BIT(base, CAN, RFP, mbx-1U);
}

/**************************************************************************************************
 * 
 * \brief Enables overwrite protection
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * 
 * \details This function enables overwrite protection, which protects old message against being
 * overwritten by new message. In that case, next mailboxes are checked for a matching identifier,
 * and if no other mailbox is found the message is lost without further notification.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_enableOverwriteProtection(uint32_t base, uint16_t mbx)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT(mbx >= 1U && mbx <= 32U);

    HW_REG_BIT_SET(base, CAN, OPC, mbx-1U);
}

/**************************************************************************************************
 * 
 * \brief Disables overwrite protection
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_disableOverwriteProtection(uint32_t base, uint16_t mbx)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT(mbx >= 1U && mbx <= 32U);

    HW_REG_BIT_CLR(base, CAN, OPC, mbx-1U);
}

/**************************************************************************************************
 * 
 * \brief Gets timestamp counter value
 * 
 * \param base base address of the CAN module
 * 
 * \note Timestamp counter is a free-running 32-bit timer which is clocked by the CAN bus bit
 * clock. For example, at a bit rate of 1 Mbps, the counter increments every 1 us.
 * 
 * \return Timestamp counter value
 * 
 *************************************************************************************************/
static inline uint32_t
CAN_getTimestampCounter(uint32_t base)
{
    ASSERT(CAN_isBaseValid(base));

    return HW_REG_GET(base, CAN, TSC);
}

/**************************************************************************************************
 * 
 * \brief Sets mailbox timeout value
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * \param timeout timeout value relative to timestamp counter
 * 
 * \note This functionality cannot be used in standard CAN controller (SCC) mode.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_setTimeoutValue(uint32_t base, uint16_t mbx, uint32_t timeout)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT(mbx >= 1U && mbx <= 32U);

    uint16_t offset = (mbx - 1U) * CAN_S_MOTO;

    HW_REG_SETO(base, offset, CAN, MOTO, timeout);
}

/**************************************************************************************************
 * 
 * \brief Gets mailbox timestamp value
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * 
 * \note This functionality cannot be used in standard CAN controller (SCC) mode.
 * 
 * \return Timestamp value relative to timestamp counter
 * 
 *************************************************************************************************/
static inline uint32_t
CAN_getTimestampValue(uint32_t base, uint16_t mbx)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT(mbx >= 1U && mbx <= 32U);

    uint16_t offset = (mbx - 1U) * CAN_S_MOTS;

    return HW_REG_GETO(base, offset, CAN, MOTS);
}

/**************************************************************************************************
 * 
 * \brief Enables mailbox timeout
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * 
 * \note Before enabling the mailbox timeout, the corresponding \b MOTO register should be loaded
 * with the timeout value relative to timestamp counter register.
 * 
 * \note This functionality cannot be used in standard CAN controller (SCC) mode.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_enableMailboxTimeout(uint32_t base, uint16_t mbx)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT(mbx >= 1U && mbx <= 32U);

    HW_REG_BIT_SET(base, CAN, TOC, mbx-1U);
}

/**************************************************************************************************
 * 
 * \brief Disables mailbox timeout
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_disableMailboxTimeout(uint32_t base, uint16_t mbx)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT(mbx >= 1U && mbx <= 32U);

    HW_REG_BIT_CLR(base, CAN, TOC, mbx-1U);
}

/**************************************************************************************************
 * 
 * \brief Gets mailbox timeout status
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * 
 * \return \b true when mailbox timeout has occurred
 * 
 *************************************************************************************************/
static inline bool
CAN_getTimeoutStatus(uint32_t base, uint16_t mbx)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT(mbx >= 1U && mbx <= 32U);

    return HW_REG_BIT_GET(base, CAN, TOS, mbx-1U);
}

/**************************************************************************************************
 * 
 * \brief Sets message identifier
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * \param identifier message identifier
 * 
 * \note The message identifier can be set only while the corresponding mailbox is disabled.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_setMessageIdentifier(uint32_t base, uint16_t mbx, uint32_t identifier)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT(mbx >= 1U && mbx <= 32U);

    uint16_t offset = CAN_MBX_OFFSET(mbx);

    HW_FLD_SETO(base, offset, CAN, MSGID, ID, identifier);
}

/**************************************************************************************************
 * 
 * \brief Gets message identifier
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * 
 * \return Message identifier
 * 
 *************************************************************************************************/
static inline uint32_t
CAN_getMessageIdentifier(uint32_t base, uint16_t mbx)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT(mbx >= 1U && mbx <= 32U);

    uint16_t offset = CAN_MBX_OFFSET(mbx);

    return HW_FLD_GETO(base, offset, CAN, MSGID, ID);
}

/**************************************************************************************************
 * 
 * \brief Sets mailbox answer mode
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * \param mode normal or auto answer mode
 * 
 * \details This function sets mailbox answer mode as defined by the \e mode parameter:
 * - \b CAN_ANSWER_MODE_NORMAL      The mailbox does reply to remote requests
 * - \b CAN_ANSWER_MODE_AUTO        If a matching remote request is received, the CAN module
 *                                  answers to the remote request
 * 
 * \note This configuration is only valid for transmit mailbox direction. For receive mailbox
 * direction, the mailbox is always configured for normal receive operation.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_setAnswerMode(uint32_t base, uint16_t mbx, CAN_AnswerMode mode)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT(mbx >= 1U && mbx <= 32U);

    uint16_t offset = CAN_MBX_OFFSET(mbx);

    if (mode == CAN_ANSWER_MODE_NORMAL) {
        HW_BIT_CLRO(base, offset, CAN, MSGID, AAM);
    } else { /* mode == CAN_ANSWER_MODE_AUTO */
        HW_BIT_SETO(base, offset, CAN, MSGID, AAM);
    }
}

/**************************************************************************************************
 * 
 * \brief Enables acceptance mask
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * 
 * \note Acceptance mask is used only for receive mailboxes.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_enableAcceptanceMask(uint32_t base, uint16_t mbx)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT(mbx >= 1U && mbx <= 32U);

    uint16_t offset = CAN_MBX_OFFSET(mbx);

    HW_BIT_SETO(base, offset, CAN, MSGID, AME);
}

/**************************************************************************************************
 * 
 * \brief Disables acceptance mask
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * 
 * \details This function disables acceptance mask which is used only for receive mailboxes. When
 * acceptance mask is disabled, all identifier bits must match to receive the message.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_disableAcceptanceMask(uint32_t base, uint16_t mbx)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT(mbx >= 1U && mbx <= 32U);

    uint16_t offset = CAN_MBX_OFFSET(mbx);

    HW_BIT_CLRO(base, offset, CAN, MSGID, AME);
}

/**************************************************************************************************
 * 
 * \brief Sets mailbox identifier mode
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * \param mode standard or extended mailbox identifier mode
 * 
 * \details This function sets mailbox identifier mode:
 * - \b CAN_ID_MODE_STANDARD    The message has standard identifier (11 bits) (default on reset)
 * - \b CAN_ID_MODE_EXTENDED    The message has extended identifier (29 bits)
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_setIdentifierMode(uint32_t base, uint16_t mbx, CAN_IdentifierMode mode)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT(mbx >= 1U && mbx <= 32U);

    uint16_t offset = CAN_MBX_OFFSET(mbx);

    if (mode == CAN_ID_MODE_STANDARD) {
        HW_BIT_CLRO(base, offset, CAN, MSGID, IDE);
    } else { /* mode == CAN_ID_MODE_EXTENDED */
        HW_BIT_SETO(base, offset, CAN, MSGID, IDE);
    }
}

/**************************************************************************************************
 * 
 * \brief Gets mailbox identifier mode
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * 
 * \return Mailbox identifier mode
 * 
 *************************************************************************************************/
static inline CAN_IdentifierMode
CAN_getIdentifierMode(uint32_t base, uint16_t mbx)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT(mbx >= 1U && mbx <= 32U);

    uint16_t offset = CAN_MBX_OFFSET(mbx);

    bool ide = HW_BIT_GETO(base, offset, CAN, MSGID, IDE);

    return ide ? CAN_ID_MODE_EXTENDED : CAN_ID_MODE_STANDARD;
}

/**************************************************************************************************
 * 
 * \brief Sets message data length
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * \param length message data length as number of bytes
 * 
 * \note Valid message data length is from 0 to 8 bytes.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_setMessageDataLength(uint32_t base, uint16_t mbx, uint16_t length)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT(mbx >= 1U && mbx <= 32U);
    ASSERT(length <= 8U);

    uint16_t offset = CAN_MBX_OFFSET(mbx);

    HW_FLD_SETO(base, offset, CAN, MSGCTRL, DLC, length);
}

/**************************************************************************************************
 * 
 * \brief Get message data length
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * 
 * \return Message data length as number of bytes
 * 
 *************************************************************************************************/
static inline uint16_t
CAN_getMessageDataLength(uint32_t base, uint16_t mbx)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT(mbx >= 1U && mbx <= 32U);

    uint16_t offset = CAN_MBX_OFFSET(mbx);

    return HW_FLD_GETO(base, offset, CAN, MSGCTRL, DLC);
}

/**************************************************************************************************
 * 
 * \brief Sets remote-transmission request
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * 
 * \details This function sets a remote-transmission request flag. For receive mailboxes, remote
 * remote frame is transmitted and the corresponding data frame is received in the same mailbox.
 * Once the remote frame is sent, this flag is cleared by CAN module. For transmit mailbox, remote
 * frame is transmitted, but the corresponding data frame has to be received by another mailbox.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_setRemoteTransmissionRequest(uint32_t base, uint16_t mbx)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT(mbx >= 1U && mbx <= 32U);

    uint16_t offset = CAN_MBX_OFFSET(mbx);

    HW_BIT_SETO(base, offset, CAN, MSGCTRL, RTR);
}

/**************************************************************************************************
 * 
 * \brief Clears remote-transmission request
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_clearRemoteTransmissionRequest(uint32_t base, uint16_t mbx)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT(mbx >= 1U && mbx <= 32U);

    uint16_t offset = CAN_MBX_OFFSET(mbx);

    HW_BIT_CLRO(base, offset, CAN, MSGCTRL, RTR);
}

/**************************************************************************************************
 * 
 * \brief Gets remote-transmission request status
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * 
 * \return Remote transmission request status
 * 
 *************************************************************************************************/
static inline bool
CAN_getRemoteTransmissionRequest(uint32_t base, uint16_t mbx)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT(mbx >= 1U && mbx <= 32U);

    uint16_t offset = CAN_MBX_OFFSET(mbx);

    return HW_BIT_GETO(base, offset, CAN, MSGCTRL, RTR);
}

/**************************************************************************************************
 * 
 * \brief Sets transmit mailbox priority level
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * \param level transmit mailbox priority level
 * 
 * \details This function sets transmit mailbox priority level. The highest number has the highest
 * priority. When two mailboxes have the same priority, the one with the higher mailbox number is
 * transmitted first.
 * 
 * \note This functionality is not available in standard CAN controller (SCC) mode.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_setTransmitPriorityLevel(uint32_t base, uint16_t mbx, uint16_t level)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT(mbx >= 1U && mbx <= 32U);
    ASSERT(level < 32U);

    uint16_t offset = CAN_MBX_OFFSET(mbx);

    HW_FLD_SETO(base, offset, CAN, MSGCTRL, TPL, level);
}

/**************************************************************************************************
 * 
 * \brief Sets mailbox global acceptance mask and identifier mode
 * 
 * \param base base address of the CAN module
 * \param mask acceptance mask
 * \param extension identifier mode extension
 * 
 * \details This function sets global acceptance filter mask and identifier mode. Received message
 * identifier bits are compared only for mask bits that are not \b 1. The \e extension parameter
 * defines identifier mode as follows:
 * - \b false   \b IDE bit in the \b MSGID register defines behavior (default on reset)
 * - \b true    Both standard and extended frames can be received
 * 
 * \note Global acceptance filter is used only in standard CAN controller (SCC) mode.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_setGlobalAcceptance(uint32_t base, uint32_t mask, bool extension)
{
    ASSERT(CAN_isBaseValid(base));

    HW_FLD_SET(base, CAN, GAM, GAM, mask);

    if (!extension) {
        HW_BIT_CLR(base, CAN, GAM, AMI);
    } else {
        HW_BIT_SET(base, CAN, GAM, AMI);
    }
}

/**************************************************************************************************
 * 
 * \brief Sets mailbox local acceptance mask and identifier mode
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * \param mask acceptance mask
 * \param extension identifier mode extension
 * 
 * \details This function sets local acceptance filter mask and identifier mode. Received message
 * identifier bits are compared only for mask bits that are not \b 1. The \e extension parameter
 * defines identifier mode as follows:
 * - \b false   \b IDE bit in the \b MSGID register defines behavior (default on reset)
 * - \b true    Both standard and extended frames can be received
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_setLocalAcceptance(uint32_t base, uint16_t mbx, uint32_t mask, bool extension)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT(mbx >= 1U && mbx <= 32U);

    uint16_t offset = (mbx - 1U) * CAN_S_LAM;

    HW_FLD_SETO(base, offset, CAN, LAM, LAM, mask);

    if (!extension) {
        HW_BIT_CLRO(base, offset, CAN, LAM, LAMI);
    } else {
        HW_BIT_SETO(base, offset, CAN, LAM, LAMI);
    }
}

/**************************************************************************************************
 * 
 * \brief Enables individual CAN module interrupt sources
 * 
 * \param base base address of the CAN module
 * \param intFlags bit mask of interrupt sources
 * 
 * \details This function enables interrupt sources for CAN module. Only enabled sources will
 * cause a CPU interrupt. The \e intFlags parameter is logical OR of any of the following:
 * - \b CAN_INT_SRC_I0      Interrupt line 0
 * - \b CAN_INT_SRC_I1      Interrupt line 1
 * - \b CAN_INT_SRC_WL      Warning level interrupt
 * - \b CAN_INT_SRC_EP      Error passive interrupt
 * - \b CAN_INT_SRC_BO      Bus-off interrupt
 * - \b CAN_INT_SRC_RML     Received-message lost interrupt
 * - \b CAN_INT_SRC_WU      Wake-up interrupt
 * - \b CAN_INT_SRC_WD      Write-denied interrupt
 * - \b CAN_INT_SRC_AA      Abort-acknowledge interrupt
 * - \b CAN_INT_SRC_TCO     Timestamp counter overflow interrupt
 * - \b CAN_INT_SRC_MTO     Mailbox timeout interrupt
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_enableInterrupt(uint32_t base, uint32_t intFlags)
{
    ASSERT(CAN_isBaseValid(base));

    intFlags &= CAN_INT_SRC_MASK;

    EALLOW;

    HW_REG_MASK_SET(base, CAN, GIM, intFlags);

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Disables individual CAN module interrupt sources
 * 
 * \param base base address of the CAN module
 * \param intFlags bit mask of interrupt sources
 * 
 * \details This function disables interrupt sources for CAN module. Only enabled sources will
 * cause a CPU interrupt. The \e intFlags parameter is logical OR of any of the following:
 * - \b CAN_INT_SRC_I0      Interrupt line 0
 * - \b CAN_INT_SRC_I1      Interrupt line 1
 * - \b CAN_INT_SRC_WL      Warning level interrupt
 * - \b CAN_INT_SRC_EP      Error passive interrupt
 * - \b CAN_INT_SRC_BO      Bus-off interrupt
 * - \b CAN_INT_SRC_RML     Received-message lost interrupt
 * - \b CAN_INT_SRC_WU      Wake-up interrupt
 * - \b CAN_INT_SRC_WD      Write-denied interrupt
 * - \b CAN_INT_SRC_AA      Abort-acknowledge interrupt
 * - \b CAN_INT_SRC_TCO     Timestamp counter overflow interrupt
 * - \b CAN_INT_SRC_MTO     Mailbox timeout interrupt
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_disableInterrupt(uint32_t base, uint32_t intFlags)
{
    ASSERT(CAN_isBaseValid(base));

    intFlags &= CAN_INT_SRC_MASK;

    EALLOW;

    HW_REG_MASK_CLR(base, CAN, GIM, intFlags);

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Sets global interrupt line
 * 
 * \param base base address of the CAN module
 * \param line interrupt line 0 or 1
 * 
 * \details This assigns global interrupt to an interrupt line:
 * - \b CAN_LINE_I0     Global interrupts are mapped to I0 interrupt line
 * - \b CAN_LINE_I1     Global interrupts are mapped to I1 interrupt line
 * 
 * Global interrupts sources are: TCO, WD, WU, BO, EP, RML, AA, and WL.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_setGlobalInterruptLine(uint32_t base, CAN_InterruptLine line)
{
    ASSERT(CAN_isBaseValid(base));

    EALLOW;

    if (line == CAN_LINE_I0) {
        HW_BIT_CLR(base, CAN, GIM, GIL);
    } else { /* line == CAN_LINE_I1 */
        HW_BIT_SET(base, CAN, GIM, GIL);
    }

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Gets interrupt flags for the given interrupt line
 * 
 * \param base base address of the CAN module
 * \param line interrupt line 0 or 1
 * 
 * \details This function gets interrupt flags for the given interrupt line. Interrupt flags are
 * logical OR of any of the following:
 * - \b CAN_INT_STS_MTO     Mailbox timeout flag
 * - \b CAN_INT_STS_TCO     Timestamp counter overflow flag
 * - \b CAN_INT_STS_GMI     Global mailbox interrupt flag
 * - \b CAN_INT_STS_AA      Abort acknowledge interrupt flag
 * - \b CAN_INT_STS_WD      Write defined interrupt flag
 * - \b CAN_INT_STS_WU      Wake-up interrupt flag
 * - \b CAN_INT_STS_RML     Received-message lost interrupt flag
 * - \b CAN_INT_STS_BO      Bus-off interrupt flag
 * - \b CAN_INT_STS_EP      Error passive interrupt flag
 * - \b CAN_INT_STS_WL      Warning level interrupt flag
 * 
 * \return Interrupt flags
 * 
 *************************************************************************************************/
static inline uint32_t
CAN_getInterruptSource(uint32_t base, CAN_InterruptLine line)
{
    ASSERT(CAN_isBaseValid(base));

    uint32_t gifReg;

    if (line == CAN_LINE_I0) {
        gifReg = HW_REG_GET(base, CAN, GIF0);
    } else { /* line == CAN_LINE_I1 */
        gifReg = HW_REG_GET(base, CAN, GIF1);
    }
    
    return gifReg & CAN_INT_STS_MASK;
}

/**************************************************************************************************
 * 
 * \brief Gets mailbox interrupt vector
 * 
 * \param base base address of the CAN module
 * \param line interrupt line 0 or 1
 * 
 * \details This function returns mailbox interrupt vector. This vector indicates the number of
 * the mailbox that set the global mailbox interrupt flag. It keeps vector until the appropriate
 * \b MIF bit is cleared or when a higher priority mailbox interrupt occurred.
 * 
 * \note The value is undefined if no flag is set in \b TA and \b RMP register, and \b GMIF0 or
 * \b GMIF1 are also cleared.
 * 
 * \return Mailbox interrupt vector
 * 
 *************************************************************************************************/
static inline uint16_t
CAN_getInterruptMailbox(uint32_t base, CAN_InterruptLine line)
{
    ASSERT(CAN_isBaseValid(base));

    uint16_t mivFld;

    if (line == CAN_LINE_I0) {
        mivFld = HW_FLD_GET(base, CAN, GIF0, MIV);
    } else { /* line == CAN_LINE_I1 */
        mivFld = HW_FLD_GET(base, CAN, GIF1, MIV);
    }

    return mivFld;
}

/**************************************************************************************************
 * 
 * \brief Clears interrupt source for given interrupt line
 * 
 * \param base base address of the CAN module
 * \param line interrupt line 0 or 1
 * \param intFlags bit mask of interrupt flags
 * 
 * \details This function clears interrupt flags for the given interrupt line. Interrupt flags in
 * the \e intFlags parameter are logical OR of any of the following:
 * - \b CAN_INT_CLR_TCO     Timestamp counter overflow flag
 * - \b CAN_INT_CLR_WD      Write-denied interrupt flag
 * - \b CAN_INT_CLR_WU      Wake-up interrupt flag
 * - \b CAN_INT_CLR_BO      Bus-off interrupt flag
 * - \b CAN_INT_CLR_EP      Error passive interrupt flag
 * - \b CAN_INT_CLR_WL      Warning level interrupt flag
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_clearInterruptSource(uint32_t base, CAN_InterruptLine line, uint32_t intFlags)
{
    ASSERT(CAN_isBaseValid(base));

    intFlags &= CAN_INT_CLR_MASK;

    if (line == CAN_LINE_I0) {
        HW_REG_SET(base, CAN, GIF0, intFlags);
    } else { /* line == CAN_LINE_I1 */
        HW_REG_SET(base, CAN, GIF1, intFlags);
    }
}

/**************************************************************************************************
 * 
 * \brief Enables mailbox interrupt
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * 
 * \details This function enables mailbox interrupt. An interrupt is generated if a message has
 * been transmitted successfully or if a message has been received without any error.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_enableMailboxInterrupt(uint32_t base, uint16_t mbx)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT(mbx >= 1U && mbx <= 32U);

    EALLOW;

    HW_REG_BIT_SET(base, CAN, MIM, mbx-1U);

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Disables mailbox interrupt
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_disableMailboxInterrupt(uint32_t base, uint16_t mbx)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT(mbx >= 1U && mbx <= 32U);

    EALLOW;

    HW_REG_BIT_CLR(base, CAN, MIM, mbx-1U);

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Sets mailbox interrupt line
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * \param line interrupt line 0 or 1
 * 
 * \details This function assigns mailbox to an interrupt line:
 * - \b CAN_LINE_I0     Mailbox interrupt is generated on line 0 (default on reset)
 * - \b CAN_LINE_I1     Mailbox interrupt is generated on line 1
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
CAN_setMailboxInterruptLine(uint32_t base, uint16_t mbx, CAN_InterruptLine line)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT(mbx >= 1U && mbx <= 32U);

    if (line == CAN_LINE_I0) {
        HW_REG_BIT_CLR(base, CAN, MIL, mbx-1U);
    } else { /* line == CAN_LINE_I1 */
        HW_REG_BIT_SET(base, CAN, MIL, mbx-1U);
    }
}

/**************************************************************************************************
 * 
 * \brief Gets transmit error counter register
 * 
 * \param base base address of the CAN module
 * 
 * \return Transmit error counter register
 * 
 *************************************************************************************************/
static inline uint32_t
CAN_getTransmitErrorCounter(uint32_t base)
{
    ASSERT(CAN_isBaseValid(base));

    return HW_FLD_GET(base, CAN, TEC, TEC);
}

/**************************************************************************************************
 * 
 * \brief Gets receive error counter register
 * 
 * \param base base address of the CAN module
 * 
 * \return Receive error counter register
 * 
 *************************************************************************************************/
static inline uint32_t
CAN_getReceiveErrorCounter(uint32_t base)
{
    ASSERT(CAN_isBaseValid(base));

    return HW_FLD_GET(base, CAN, REC, REC);
}

/**************************************************************************************************
 * 
 * \brief Reads data bytes from mailbox message data registers
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * \param data storage for data bytes
 * \param length message data length as number of bytes
 * 
 * \details This function reads data bytes from  \b MDL and \b MDH mailbox message data registers
 * with respect to the data byte order defined in \b MC register.
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
CAN_readData(uint32_t base, uint16_t mbx, uint16_t *data, uint16_t length);

/**************************************************************************************************
 * 
 * \brief Writes data bytes to mailbox message data register
 * 
 * \param base base address of the CAN module
 * \param mbx mailbox identifier (1..32)
 * \param data storage for data bytes
 * \param length message length (number of bytes)
 * 
 * \details This function writes data bytes to \b MDL and \b MDH mailbox message data registers
 * with respect to the data byte order defined in \b MC register.
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
CAN_writeData(uint32_t base, uint16_t mbx, const uint16_t *data, uint16_t length);

/**************************************************************************************************
 * 
 * \brief Sets CAN controller bit timing
 * 
 * \param base base address of the CAN module
 * \param brp baud rate prescaler
 * \param tSeg1 time segment 1
 * \param tSeg2 time segment 2
 * \param sjw synchronization jump width
 * 
 * \note True bit-timing parameter is obtained by adding 1 to the register (field) value. For
 * example, if \e BRP of 10 is wanted, then \e brp on input should be 9.
 * 
 * \note Triple sample mode (SAM = 1) is automatically enabled if true bit-rate prescaler (BRP)
 * is greater than 4!
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
CAN_setBitTiming(uint32_t base, uint16_t brp, uint16_t tSeg1, uint16_t tSeg2, uint16_t sjw);

#endif /* CAN_H */
