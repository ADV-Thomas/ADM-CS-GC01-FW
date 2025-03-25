/**************************************************************************************************
 * 
 * \file interrupt.h
 * 
 * \brief Peripheral interrupt (PIE) driverlib implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "interrupt.h"

/**************************************************************************************************
 * 
 * \brief Clears the IFR flag in the CPU
 * 
 * \param group interrupt group to be cleared
 * 
 * \details This function clears the IFR flag. This switch is needed because clearing the IFR can
 * be done only with a constant.
 * 
 * \return None
 * 
 *************************************************************************************************/
static void
Interrupt_clearIFR(uint16_t group)
{
#define CLEAR_IFR(CODE)                                                                         \
    case CODE:                                                                                  \
        IFR &= ~((uint16_t) CODE);                                                              \
        break
    
    switch (group) {
    CLEAR_IFR(0x0001U);
    CLEAR_IFR(0x0002U);
    CLEAR_IFR(0x0004U);
    CLEAR_IFR(0x0008U);
    CLEAR_IFR(0x0010U);
    CLEAR_IFR(0x0020U);
    CLEAR_IFR(0x0040U);
    CLEAR_IFR(0x0080U);
    CLEAR_IFR(0x0100U);
    CLEAR_IFR(0x0200U);
    CLEAR_IFR(0x0400U);
    CLEAR_IFR(0x0800U);
    CLEAR_IFR(0x1000U);
    CLEAR_IFR(0x2000U);
    CLEAR_IFR(0x4000U);
    CLEAR_IFR(0x8000U);
    default:
        ASSERT(false);
        break;
    }
}

/**************************************************************************************************
 * 
 * Interrupt_initModule()
 * 
 *************************************************************************************************/
void
Interrupt_initModule(void)
{
    Interrupt_disableMaster();

    IER = 0x0000U;
    IFR = 0x0000U;

    unsigned i;
    for (i = 0U; i < 12U; i++) {
        HW_REG_SETO(PIECTRL_BASE, (2U * i), PIE, IERX, 0U);
        HW_REG_SETO(PIECTRL_BASE, (2U * i), PIE, IFRX, 0U);
    }

    Interrupt_enablePIE();
}

/**************************************************************************************************
 * 
 * Interrupt_initVectorTable()
 * 
 *************************************************************************************************/
void
Interrupt_initVectorTable(void)
{
    /* RESET and PIE Group 1..12 vectors are not used */
    unsigned i;
    for (i = 13U; i < 128U; i++) {
        uint32_t code = ((uint32_t) i << 16U) & 0xFFFF0000UL;
        Interrupt_unregister(code);
    }

    Interrupt_register(INT_NMI, Interrupt_nmiHandler);
    Interrupt_register(INT_ILLEGAL, Interrupt_illegalOperationHandler);
}

/**************************************************************************************************
 * 
 * Interrupt_enable()
 * 
 *************************************************************************************************/
void
Interrupt_enable(uint32_t code)
{
    uint16_t id = (uint16_t) (code >> 16) & 0xFFFFU;

    bool status = Interrupt_disableMaster();

    if (id >= 0x20U) {
        uint16_t x = ((code >> 8) & 0x00FFU) - 1U;
        uint16_t y = ((code >> 0) & 0x00FFU) - 1U;
        uint16_t group = 1U << x;

        HW_REG_BIT_SETO(PIECTRL_BASE, (x * 2U), PIE, IERX, y);

        /* PIE group interrupt */
        IER |= group;
    } else if ((id >= 0x0DU) && (id <= 0x10U)) {
        /* INT13, INT14, DLOGINT and RTOSINT */
        uint16_t group = 1U << (id - 1U);
        IER |= group;
    } else {
        /* Other interrupts */
    }

    /* Re-enable interrupts if they were enabled */
    if (!status) {
        Interrupt_enableMaster();
    }
}

/**************************************************************************************************
 * 
 * Interrupt_disable()
 * 
 *************************************************************************************************/
void
Interrupt_disable(uint32_t code)
{
    uint16_t id = (uint16_t) (code >> 16) & 0xFFFFU;

    bool status = Interrupt_disableMaster();

    if (id >= 0x20U) {
        uint16_t x = ((code >> 8) & 0x00FFU) - 1U;
        uint16_t y = ((code >> 0) & 0x00FFU) - 1U;
        uint16_t group = 1U << x;

        HW_REG_BIT_CLRO(PIECTRL_BASE, (x * 2U), PIE, IERX, y);

        /* Wait for any pending interrupts to get to the CPU */
        asm(" RPT #5 || NOP");

        Interrupt_clearIFR(group);
        Interrupt_clearACKGroup(group);
    } else if ((id >= 0x0DU) && (id <= 0x10U)) {
        /* INT13, INT14, DLOGINT and RTOSINT */
        uint16_t group = 1U << (id - 1U);

        IER &= ~group;

        /* Wait for any pending interrupts to get to the CPU */
        asm(" RPT #5 || NOP");

        Interrupt_clearIFR(group);
    } else {
        /* Other interrupts */
    }

    /* Re-enable interrupts if they were enabled */
    if (!status) {
        Interrupt_enableMaster();
    }
}
