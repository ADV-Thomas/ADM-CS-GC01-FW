/**************************************************************************************************
 * 
 * \file debug.h
 * 
 * \brief Debug library provides ASSERT() macro
 * 
 * \note debug.h in f28004x driverlib defines __error__() function prototype which uses uint32_t
 * but without loading stdint library first. f2803x driverlib is not affected by this.
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_LIB_DEBUG_H
#define _INC_LIB_DEBUG_H

#include <stdint.h>

#include "driverlib/debug.h"

#endif /* _INC_LIB_DEBUG_H */
