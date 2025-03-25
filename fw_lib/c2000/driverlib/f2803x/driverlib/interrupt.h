/**************************************************************************************************
 * 
 * \file interrupt.h
 * 
 * \brief Peripheral interrupt (PIE) driverlib interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef INTERRUPT_H
#define INTERRUPT_H

#ifdef __TMS320C28XX__

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_pie.h"
#include "inc/hw_types.h"

#include "cpu.h"
#include "debug.h"

#include <stdint.h>
#include <stdbool.h>

/**************************************************************************************************
 * 
 * The following are values that can be passed to \e Interrupt_enableInCPU() and
 * \e Interrupt_disableInCPU() as the \e code parameter.
 * 
 *************************************************************************************************/

#define INTERRUPT_CPU_INT1      0x0001U     /* CPU Interrupt Number 1                           */
#define INTERRUPT_CPU_INT2      0x0002U     /* CPU Interrupt Number 2                           */
#define INTERRUPT_CPU_INT3      0x0004U     /* CPU Interrupt Number 3                           */
#define INTERRUPT_CPU_INT4      0x0008U     /* CPU Interrupt Number 4                           */
#define INTERRUPT_CPU_INT5      0x0010U     /* CPU Interrupt Number 5                           */
#define INTERRUPT_CPU_INT6      0x0020U     /* CPU Interrupt Number 6                           */
#define INTERRUPT_CPU_INT7      0x0040U     /* CPU Interrupt Number 7                           */
#define INTERRUPT_CPU_INT8      0x0080U     /* CPU Interrupt Number 8                           */
#define INTERRUPT_CPU_INT9      0x0100U     /* CPU Interrupt Number 9                           */
#define INTERRUPT_CPU_INT10     0x0200U     /* CPU Interrupt Number 10                          */
#define INTERRUPT_CPU_INT11     0x0400U     /* CPU Interrupt Number 11                          */
#define INTERRUPT_CPU_INT12     0x0800U     /* CPU Interrupt Number 12                          */
#define INTERRUPT_CPU_INT13     0x1000U     /* CPU Interrupt Number 13                          */
#define INTERRUPT_CPU_INT14     0x2000U     /* CPU Interrupt Number 14                          */
#define INTERRUPT_CPU_DLOGINT   0x4000U     /* CPU Data Logging Interrupt                       */
#define INTERRUPT_CPU_RTOSINT   0x8000U     /* CPU RTOS Interrupt                               */

/**************************************************************************************************
 * 
 * The following are values that can be passed to \e Interrupt_clearACKGroup() as the
 * \e code parameter.
 * 
 *************************************************************************************************/

#define INTERRUPT_ACK_GROUP1    0x0001U     /* Acknowledge PIE Interrupt Group 1                */
#define INTERRUPT_ACK_GROUP2    0x0002U     /* Acknowledge PIE Interrupt Group 2                */
#define INTERRUPT_ACK_GROUP3    0x0004U     /* Acknowledge PIE Interrupt Group 3                */
#define INTERRUPT_ACK_GROUP4    0x0008U     /* Acknowledge PIE Interrupt Group 4                */
#define INTERRUPT_ACK_GROUP5    0x0010U     /* Acknowledge PIE Interrupt Group 5                */
#define INTERRUPT_ACK_GROUP6    0x0020U     /* Acknowledge PIE Interrupt Group 6                */
#define INTERRUPT_ACK_GROUP7    0x0040U     /* Acknowledge PIE Interrupt Group 7                */
#define INTERRUPT_ACK_GROUP8    0x0080U     /* Acknowledge PIE Interrupt Group 8                */
#define INTERRUPT_ACK_GROUP9    0x0100U     /* Acknowledge PIE Interrupt Group 9                */
#define INTERRUPT_ACK_GROUP10   0x0200U     /* Acknowledge PIE Interrupt Group 10               */
#define INTERRUPT_ACK_GROUP11   0x0400U     /* Acknowledge PIE Interrupt Group 11               */
#define INTERRUPT_ACK_GROUP12   0x0800U     /* Acknowledge PIE Interrupt Group 12               */

/**************************************************************************************************
 * 
 * Extern compiler intrinsic prototypes. See compiler User's Guide for details.
 * 
 *************************************************************************************************/

extern uint16_t __disable_interrupts(void);
extern uint16_t __enable_interrupts(void);

/**************************************************************************************************
 * 
 * Prototypes for the APIs
 * 
 *************************************************************************************************/

/**************************************************************************************************
 * 
 * \brief The default interrupt handler
 * 
 * \param None
 * 
 * \details This is the default interrupt handler. The \e Interrupt_initVectorTable() function sets
 * all vectors to this function. When an interrupt is unregistered using \e Interrupt_unregister()
 * function, this handler takes its place. This should never be called during normal operation.
 * 
 * \note The \b ESTOP0 statement is for debug purposes only. Remove and replace with appropriate
 * error handling routine for your program.
 * 
 * \return None
 * 
 *************************************************************************************************/
static void
Interrupt_defaultHandler(void)
{
    ESTOP0;
    while (true);
}

/**************************************************************************************************
 * 
 * \brief The default illegal instruction trap interrupt handler
 * 
 * \param None
 * 
 * \details This is the default interrupt handler for an illegal instruction trap (ITRAP). The
 * \e Interrupt_initVectorTable() function sets the appropriate vector to this function. This
 * should never be called during normal operation.
 * 
 * \note The \b ESTOP0 statement is for debug purposes only. Remove and replace with appropriate
 * error handling routine for your program.
 * 
 * \return None
 * 
 *************************************************************************************************/
static void
Interrupt_illegalOperationHandler(void)
{
    ESTOP0;
    while (true);
}

/**************************************************************************************************
 * 
 * \brief The default non-maskable interrupt handler
 * 
 * \param None
 * 
 * \details This is the default interrupt handler for a non-maskable interrupt (NMI). The
 * \e Interrupt_initVectorTable() function sets the appropriate vector to this function.
 * This should never be called during normal operation.
 * 
 * \note The \b ESTOP0 statement is for debug purposes only. Remove and replace with appropriate
 * error handling routine for your program.
 * 
 * \return None
 * 
 *************************************************************************************************/
static void
Interrupt_nmiHandler(void)
{
    ESTOP0;
    while (true);
}

/**************************************************************************************************
 * 
 * \brief Allows CPU to process interrupts
 * 
 * \param None
 * 
 * \details This function clears the global interrupt mask bit (INTM) in the CPU, allowing the
 * processor to respond to interrupts.
 * 
 * \return \b true if interrupts were disabled when this function was called; \b false if they
 * were initially enabled
 * 
 *************************************************************************************************/
static inline bool
Interrupt_enableMaster(void)
{
    return (((__enable_interrupts() & 0x1U) != 0U) ? true : false);
}

/**************************************************************************************************
 * 
 * \brief Stops CPU from processing interrupts
 * 
 * \param None
 * 
 * \details This function sets the global interrupt mask bit (INTM) in the CPU, preventing the
 * processor to respond from receiving maskable interrupts.
 * 
 * \return \b true if interrupts were disabled when this function was called; \b false if they
 * were initially enabled
 * 
 *************************************************************************************************/
static inline bool
Interrupt_disableMaster(void)
{
    return (((__disable_interrupts() & 0x1U) != 0U) ? true : false);
}

/**************************************************************************************************
 * 
 * \brief Registers a function to be called when interrupt occurs
 * 
 * \param code interrupt code
 * \param handler function handler
 * 
 * \details This function is used to specify the function to be called when the given interrupt is
 * asserted to the processor. When the interrupt occurs, if it's enabled via \e Interrupt_enable(),
 * the given function will be called in interrupt context. Since function can preempt other code,
 * care must be taken to protect memory or peripherals that are accessed by the function.
 * 
 * The allowed \e code parameter values are defined in \e inc/hw_ints.h
 * 
 * \note This function assumes that the PIE module has been enabled. See \e Interrupt_initModule().
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
Interrupt_register(uint32_t code, void (*handler)(void))
{
    uint32_t address = PIEVECTTABLE_BASE + (((code >> 16U) & 0x0000FFFFUL) * 2U);

    EALLOW;

    HWREG(address) = (uint32_t) handler;

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Unregisters the function to be called when interrupt occurs
 * 
 * \param code interrupt code
 * 
 * \details This function is used to set the default handler to \e Interrupt_defaultHandler().
 * Call \e Interrupt_disable() to disable the interrupt before calling this function.
 * 
 * The allowed \e code parameter values are defined in \e inc/hw_ints.h
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
Interrupt_unregister(uint32_t code)
{
    uint32_t address = PIEVECTTABLE_BASE + (((code >> 16U) & 0x0000FFFFUL) * 2U);

    EALLOW;

    HWREG(address) = (uint32_t) Interrupt_defaultHandler;

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Enables CPU interrupt channels
 * 
 * \param code CPU interrupts to be enabled
 * 
 * \details This function enables the specified interrupts in the CPU. The \e code parameter
 * is a logical OR between any of the \b INTERRUPT_CPU_INTx, \b INTERRUPT_CPU_DLOGINT, and
 * \b INTERRUPT_CPU_RTOSINT constants.
 * 
 * \note Interrupts \b INTERRUPT_CPU_INT1 to \b INTERRUPT_CPU_INT12 correspond to PIE groups with
 * those same numbers.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
Interrupt_enableInCPU(uint16_t code)
{
    IER |= code;
}

/**************************************************************************************************
 * 
 * \brief Disables CPU interrupt channels
 * 
 * \param code CPU interrupts to be disabled
 * 
 * \details This function disables the specified interrupts in the CPU. The \e code parameter
 * is a logical OR between any of the \b INTERRUPT_CPU_INTx, \b INTERRUPT_CPU_DLOGINT, and
 * \b INTERRUPT_CPU_RTOSINT constants.
 * 
 * \note Interrupts \b INTERRUPT_CPU_INT1 to \b INTERRUPT_CPU_INT12 correspond to PIE groups with
 * those same numbers.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
Interrupt_disableInCPU(uint16_t code)
{
    IER &= ~code;
}

/**************************************************************************************************
 * 
 * \brief Acknowledges PIE interrupt group
 * 
 * \param group interrupt group(s) to be acknowledged
 * 
 * \details This function acknowledges PIE interrupt group. The \e group parameter is a logical OR
 * between any of the \b INTERRUPT_ACK_GROUPx constants.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
Interrupt_clearACKGroup(uint16_t group)
{
    HW_REG_SET(PIECTRL_BASE, PIE, ACK, group);
}

/**************************************************************************************************
 * 
 * \brief Enables PIE module
 * 
 * \param None
 * 
 * \details This function enables the vector fetching for the peripheral interrupts by enabling
 * the PIE module.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
Interrupt_enablePIE(void)
{
    HW_BIT_SET(PIECTRL_BASE, PIE, CTRL, ENPIE);
}

/**************************************************************************************************
 * 
 * \brief Disables PIE module
 * 
 * \param None
 * 
 * \details This function disables the vector fetching for the peripheral interrupts by disabling
 * the PIE module. \b PIEACK, \b PIEIFR, and \b PIEIER registers can be accessed even when the PIE
 * module is disabled.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
Interrupt_disablePIE(void)
{
    HW_BIT_CLR(PIECTRL_BASE, PIE, CTRL, ENPIE);
}

/**************************************************************************************************
 * 
 * \brief Initializes PIE control registers to known state
 * 
 * \param None
 * 
 * \details This function initializes PIE control registers. After globally disabling interrupts
 * and enabling the PIE, it clears all of the PIE interrupt enable bits and interrupt flags.
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
Interrupt_initModule(void);

/**************************************************************************************************
 * 
 * \brief Initializes PIE vector table
 * 
 * \param None
 * 
 * \details This function initializes the PIE vector table by setting all vectors to a default
 * handler function.
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
Interrupt_initVectorTable(void);

/**************************************************************************************************
 * 
 * \brief Enables an interrupt
 * 
 * \param code interrupt code
 * 
 * \details This function enables the specified interrupt in the interrupt controller. Other
 * enables for the interrupt are unaffected by this function.
 * 
 * The allowed \e code parameter values are defined in \e inc/hw_ints.h
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
Interrupt_enable(uint32_t code);

/**************************************************************************************************
 * 
 * \brief Disables an interrupt
 * 
 * \param code interrupt code
 * 
 * \details This function disables the specified interrupt in the interrupt controller. Other
 * enables for the interrupt are unaffected by this function.
 * 
 * The allowed \e code parameter values are defined in \e inc/hw_ints.h
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
Interrupt_disable(uint32_t code);

#endif /* __TMS320C28XX__*/

#endif /* INTERRUPT_H */
