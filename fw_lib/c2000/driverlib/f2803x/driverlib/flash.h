/**************************************************************************************************
 * 
 * \file flash.h
 * 
 * \brief Flash driverlib interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef FLASH_H
#define FLASH_H

#include "inc/hw_flash.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include "cpu.h"
#include "debug.h"

#include <stdint.h>

/**************************************************************************************************
 * 
 * Defines for the API. Not intended for use by application code.
 * 
 *************************************************************************************************/

#define FLASH_STDBYWAIT_DEFAULT     0x01FFU     /* Default value for FSTDBYWAIT register        */
#define FLASH_ACTIVEWAIT_DEFAULT    0x01FFU     /* Default value for FACTIVEWAIT register       */

/* Delay of 8 cycles that allows for Flash register configuration to complete */
#define FLASH_DELAY_CONFIG      __asm(" RPT #7 || NOP")

/**************************************************************************************************
 * 
 * Prototypes for the APIs
 *
 *************************************************************************************************/

/**************************************************************************************************
 * 
 * \brief Disables Flash pipeline mode
 * 
 * \param None
 * 
 * \return None
 * 
 *************************************************************************************************/
__attribute__((ramfunc)) static inline void
Flash_disablePipelineMode(void)
{
    EALLOW;
    HW_BIT_CLR(FLASHCFG_BASE, FLASH, FOPT, ENPIPE);
    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Enables Flash pipeline mode
 * 
 * \param None
 * 
 * \details This function enables Flash pipeline mode. The pipeline mode improves performance of
 * instruction fetches by pre-fetching instructions.
 * 
 * \note When pipeline mode is enabled, paged and random wait states must be greater than zero.
 * 
 * \return None
 * 
 *************************************************************************************************/
__attribute__((ramfunc)) static inline void
Flash_enablePipelineMode(void)
{
    EALLOW;
    HW_BIT_SET(FLASHCFG_BASE, FLASH, FOPT, ENPIPE);
    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Sets wait states for random read operation to Flash bank
 * 
 * \param waitState number of wait states for random read operation to Flash bank
 * 
 * \note See device-specific data manual for the minimum time required for paged Flash access.
 * Random wait must be greater than or equal to paged wait.
 * 
 * \return None
 * 
 *************************************************************************************************/
__attribute__((ramfunc)) static inline void
Flash_setRandomReadWaitStates(uint16_t waitState)
{
    ASSERT((waitState > 0U) && (waitState <= 15U));

    uint16_t pageWait = HW_FLD_GET(FLASHCFG_BASE, FLASH, FBANKWAIT, PAGEWAIT);
    ASSERT(waitState >= pageWait);

    EALLOW;
    HW_FLD_SET(FLASHCFG_BASE, FLASH, FBANKWAIT, RANDWAIT, waitState);
    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Sets wait states for paged read operation to Flash bank
 * 
 * \param waitState number of wait states for paged read operation to Flash bank
 * 
 * \note See device-specific data manual for the minimum time required for paged Flash access.
 * 
 * \return None
 * 
 *************************************************************************************************/
__attribute__((ramfunc)) static inline void
Flash_setPagedReadWaitStates(uint16_t waitState)
{
    ASSERT(waitState <= 15U);

    EALLOW;
    HW_FLD_SET(FLASHCFG_BASE, FLASH, FBANKWAIT, PAGEWAIT, waitState);
    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Sets wait states for read operation to OTP memory
 * 
 * \param waitState number of wait states for read operation to OTP memory
 * 
 * \return None
 * 
 *************************************************************************************************/
__attribute__((ramfunc)) static inline void
Flash_setOtpWaitStates(uint16_t waitState)
{
    ASSERT((waitState > 0U) && (waitState <= 31U));

    EALLOW;
    HW_FLD_SET(FLASHCFG_BASE, FLASH, FOTPWAIT, OTPWAIT, waitState);
    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Sets Flash standby wait register to a default value
 * 
 * \param None
 * 
 * \return None
 * 
 *************************************************************************************************/
__attribute__((ramfunc)) static inline void
Flash_setStandbyWaitCount(void)
{
    EALLOW;
    HW_REG_SET(FLASHCFG_BASE, FLASH, FSTDBYWAIT, FLASH_STDBYWAIT_DEFAULT);
    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Sets Flash standby to active wait counter register to a default value
 * 
 * \param None
 * 
 * \return None
 * 
 *************************************************************************************************/
__attribute__((ramfunc)) static inline void
Flash_setActiveWaitCount(void)
{
    EALLOW;
    HW_REG_SET(FLASHCFG_BASE, FLASH, FACTIVEWAIT, FLASH_ACTIVEWAIT_DEFAULT);
    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Initializes the Flash control registers
 * 
 * \param pageWait number of wait states for paged read operation to Flash bank
 * \param randWait number of wait states for random read operation to Flash bank
 * \param otpWait number of wait states for read operation to OTP memory
 * 
 * \note You must set RANDWAIT to a value greater than or equal to PAGEWAIT setting.
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
Flash_initModule(uint16_t pageWait, uint16_t randWait, uint16_t otpWait);

#endif /* FLASH_H */
