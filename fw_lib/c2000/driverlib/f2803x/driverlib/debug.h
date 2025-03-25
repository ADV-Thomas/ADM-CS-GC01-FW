/**************************************************************************************************
 * 
 * \file debug.h
 * 
 * \brief Assert definition macro for debug
 * 
 * \note Most of this driverlib has been adapted from f28004x driverlib.
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef DEBUG_H
#define DEBUG_H

#include <stdint.h>

void __error__(char *filename, uint32_t line);

#ifdef DEBUG
#define ASSERT(expr)    do {                                                                    \
                            if (!(expr)) {                                                      \
                                __error__(__FILE__, __LINE__);                                  \
                            }                                                                   \
                        } while (0)
#else
#define ASSERT(expr)
#endif

#endif /* DEBUG_H */
