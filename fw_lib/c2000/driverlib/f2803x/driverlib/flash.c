/**************************************************************************************************
 * 
 * \file flash.c
 * 
 * \brief Flash driverlib implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "flash.h"

/**************************************************************************************************
 * 
 * Flash_initModule()
 * 
 *************************************************************************************************/
__attribute__((ramfunc)) void
Flash_initModule(uint16_t pageWait, uint16_t randWait, uint16_t otpWait)
{
    ASSERT(randWait >= pageWait);

    Flash_enablePipelineMode();

    Flash_setPagedReadWaitStates(pageWait);
    Flash_setRandomReadWaitStates(randWait);
    
    Flash_setOtpWaitStates(otpWait);

    Flash_setStandbyWaitCount();
    Flash_setActiveWaitCount();

    FLASH_DELAY_CONFIG;
}
