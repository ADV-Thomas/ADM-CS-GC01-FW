/**************************************************************************************************
 * 
 * \file hw_can.h
 * 
 * \brief Memory map of the f2803x device
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef HW_MEMMAP_H
#define HW_MEMMAP_H

#define DEVCFG_BASE                 0x00000880UL
#define ADCARESULT_BASE             0x00000B00UL
#define CPUTIMER0_BASE              0x00000C00UL
#define CPUTIMER1_BASE              0x00000C08UL
#define CPUTIMER2_BASE              0x00000C10UL
#define PIECTRL_BASE                0x00000CE0UL
#define PIEVECTTABLE_BASE           0x00000D00UL
#define FLASHCFG_BASE               0x00000A80UL
#define CANA_BASE                   0x00006000UL
#define CANA_MSG_RAM_BASE           0x00006100UL
#define EPWM1_BASE                  0x00006800UL
#define EPWM2_BASE                  0x00006840UL
#define EPWM3_BASE                  0x00006880UL
#define EPWM4_BASE                  0x000068C0UL
#define EPWM5_BASE                  0x00006900UL
#define EPWM6_BASE                  0x00006940UL
#define EPWM7_BASE                  0x00006980UL
#define GPIOCTRL_BASE               0x00006F80UL
#define GPIODATA_BASE               0x00006FC0UL
#define CLKCFG_BASE                 0x00007010UL
#define NMI_BASE                    0x00007060UL
#define ADCA_BASE                   0x00007100UL
#define I2CA_BASE                   0x00007900UL
#define PARTID_BASE                 0x003D7E80UL
#define SPIA_BASE                   0x00007040UL
#define SPIB_BASE                   0x00007740UL

#endif /* HW_MEMMAP_H */
