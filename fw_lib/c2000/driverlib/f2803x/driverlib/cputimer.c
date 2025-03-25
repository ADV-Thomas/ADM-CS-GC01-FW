/**************************************************************************************************
 * 
 * \file cputimer.c
 * 
 * \brief CPU timer driverlib implementation
 * 
 * \note Most of this driverlib has been adapted from f28004x driverlib.
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "cputimer.h"

/**************************************************************************************************
 * 
 * CPUTimer_setEmulationMode()
 * 
 *************************************************************************************************/
void
CPUTimer_setEmulationMode(uint32_t base, CPUTimer_EmulationMode mode)
{
    ASSERT(CPUTimer_isBaseValid(base));

    HW_FLD_SET(base, CPUTIMER, TCR, FREESOFT, mode);
}
