/**************************************************************************************************
 * 
 * \file cpu.h
 * 
 * \brief Useful CPU defines
 * 
 * \note Most of this driverlib has been adapted from f28004x driverlib.
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef CPU_H
#define CPU_H

#include <stdint.h>

#ifndef __TMS320C28XX_CLA__
/* External reference to the interrupt flag register (IFR) register */
extern __cregister volatile uint16_t IFR;

/* External reference to the interrupt enable register (IER) register */
extern __cregister volatile uint16_t IER;
#endif /* __TMS320C28XX_CLA__ */

#ifndef EINT
#define EINT    __asm(" clrc INTM")     /* Define to enable interrupts                          */
#endif /* EINT */

#ifndef DINT
#define DINT    __asm(" setc INTM")     /* Define to disable interrupts                         */
#endif /* DINT */

#ifndef ERTM
#define ERTM    __asm(" clrc DBGM")     /* Define to enable debug events                        */
#endif /* ERTM */

#ifndef DRTM
#define DRTM    __asm(" setc DBGM")     /* Define to disable debug events                       */
#endif /* DRTM */

#ifndef EALLOW
#ifndef __TMS320C28XX_CLA__
#define EALLOW  __eallow()              /* Define to allow writes to protected registers        */
#else
#define EALLOW  __meallow()
#endif /* __TMS320C28XX_CLA__ */
#endif /* EALLOW */

#ifndef EDIS
#ifndef __TMS320C28XX_CLA__
#define EDIS    __edis()                /* Define to disable writes to protected registers      */
#else
#define EDIS    __medis()
#endif /* __TMS320C28XX_CLA__ */
#endif /* EDIS */

#ifndef ESTOP0
#define ESTOP0  __asm(" ESTOP0")        /* Define for emulation stop                            */
#endif /* ESTOP0 */

#ifndef ESTOP1
#define ESTOP1  __asm(" ESTOP1")        /* Define for emulation stop                            */
#endif /* ESTOP1 */

#ifndef NOP
#define NOP     __asm(" NOP")           /* Define for no operation                              */
#endif /* NOP */

#ifndef IDLE
#define IDLE    __asm(" IDLE")          /* Define for putting processor into a low-power mode   */
#endif /* IDLE */

/**************************************************************************************************
 * 
 * Extern compiler intrinsic prototypes. See compiler User's Guide for more details.
 * 
 *************************************************************************************************/

extern void __eallow(void);
extern void __edis(void);

#endif /* CPU_H */
