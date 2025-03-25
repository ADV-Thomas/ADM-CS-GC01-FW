/**************************************************************************************************
 * 
 * \file hw_pie.h
 * 
 * \brief Definitions for PIE registers
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef HW_PIE_H
#define HW_PIE_H

/**************************************************************************************************
 * 
 * The following are defines for PIE registers (Sec. 1.6.4)
 * BASE REGISTER    PIECTRL_BASE
 * 
 *************************************************************************************************/

#define PIE_O_CTRL      0x0U        /* PIE Control Register                                     */
#define PIE_O_ACK       0x1U        /* PIE Acknowledge Register                                 */
#define PIE_O_IERX      0x2U        /* PIE INTx Group Enable Register                           */
#define PIE_O_IFRX      0x3U        /* PIE INTx Group Flag Register                             */

#define PIE_S_CTRL      1
#define PIE_S_ACK       1
#define PIE_S_IERX      1
#define PIE_S_IFRX      1

#define PIE_CTRL_ENPIE          0x0001U
#define PIE_CTRL_PIEVECT_S      1U
#define PIE_CTRL_PIEVECT_M      0xFFFEU

#define PIE_ACK_PIEACK_S        1U
#define PIE_ACK_PIEACK_M        0x0FFFU

#endif /* HW_PIE_H */
