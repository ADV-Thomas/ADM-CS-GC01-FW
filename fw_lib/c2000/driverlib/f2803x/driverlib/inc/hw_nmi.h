/**************************************************************************************************
 * 
 * \file hw_can.h
 * 
 * \brief Definitions for NMI registers
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef HW_NMI_H
#define HW_NMI_H

/**************************************************************************************************
 * 
 * The following are defines for NMI interrupt registers (Sec. 1.3.2).
 * BASE REGISTER    NMI_BASE
 * 
 *************************************************************************************************/

#define NMI_O_CFG           0x0U        /* Configuration Register                           *E* */
#define NMI_O_FLG           0x1U        /* Flag Register                                    *E* */
#define NMI_O_FLGCLR        0x2U        /* Flag Clear Register                              *E* */
#define NMI_O_FLGFRC        0x3U        /* Flag Force Register                              *E* */
#define NMI_O_WDCNT         0x4U        /* Watchdog Counter Register                            */
#define NMI_O_WDPRD         0x5U        /* Watchdog Period Register                         *E* */

#define NMI_S_CFG           1
#define NMI_S_FLG           1
#define NMI_S_FLGCLR        1
#define NMI_S_FLGFRC        1
#define NMI_S_WDCNT         1
#define NMI_S_WDPRD         1

#define NMI_CFG_CLOCKFAIL           0x0002U

#define NMI_FLG_NMIINT              0x0001U
#define NMI_FLG_CLOCKFAIL           0x0002U

#define NMI_FLGCLR_NMIINT           0x0001U
#define NMI_FLGCLR_CLOCKFAIL        0x0002U

#define NMI_FLGFRC_CLOCKFAIL        0x0002U

#define NMI_WDCNT_NMIWDCNT_S        0U
#define NMI_WDCNT_NMIWDCNT_M        0xFFFFU

#define NMI_WDPRD_NMIWDPRD_S        0U
#define NMI_WDPRD_NMIWDPRD_M        0xFFFFU

#endif /* HW_NMI_H */
