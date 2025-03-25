/**************************************************************************************************
 * 
 * \file hw_can.h
 * 
 * \brief Definitions for eCAN registers
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef HW_CAN_H
#define HW_CAN_H

/**************************************************************************************************
 * 
 * The following are defines for eCAN control and status registers (Sec. 14.7.1-14.7.18).
 * BASE REGISTER    CANA_BASE
 * 
 *************************************************************************************************/

#define CAN_O_ME        0x0U        /* Mailbox Enable Register                                  */
#define CAN_O_MD        0x2U        /* Mailbox Direction Register                               */
#define CAN_O_TRS       0x4U        /* Transmission-Request Set Register                        */
#define CAN_O_TRR       0x6U        /* Transmission-Request-Reset Register                      */
#define CAN_O_TA        0x8U        /* Transmission-Acknowledge Register                        */
#define CAN_O_AA        0xAU        /* Abort-Acknowledge Register                               */
#define CAN_O_RMP       0xCU        /* Received-Message-Pending Register                        */
#define CAN_O_RML       0xEU        /* Received-Message-Lost Register                           */
#define CAN_O_RFP       0x10U       /* Remote-Frame-Pending Register                            */
#define CAN_O_GAM       0x12U       /* Global Acceptance Mask Register                          */
#define CAN_O_MC        0x14U       /* Master Control Register                              *E* */
#define CAN_O_BTC       0x16U       /* Bit-Timing Configuration Register                    *E* */
#define CAN_O_ES        0x18U       /* Error and Status Register                                */
#define CAN_O_TEC       0x1AU       /* Transmit-Error-Counter Register                          */
#define CAN_O_REC       0x1CU       /* Receive-Error-Counter Register                           */
#define CAN_O_GIF0      0x1EU       /* Global Interrupt Flag 0 Register                         */
#define CAN_O_GIM       0x20U       /* Global Interrupt Mask Register                       *E* */
#define CAN_O_GIF1      0x22U       /* Global Interrupt Flag 1 Register                         */
#define CAN_O_MIM       0x24U       /* Mailbox Interrupt Mask Register                      *E* */
#define CAN_O_MIL       0x26U       /* Mailbox Interrupt Level Register                         */
#define CAN_O_OPC       0x28U       /* Overwrite Protection Control Register                    */
#define CAN_O_TIOC      0x2AU       /* Transmit I/O Control Register                        *E* */
#define CAN_O_RIOC      0x2CU       /* Receive I/O Control Register                         *E* */
#define CAN_O_TSC       0x2EU       /* Time-Stamp Counter Register                          *E* */
#define CAN_O_TOC       0x30U       /* Time-Out Control Register                                */
#define CAN_O_TOS       0x32U       /* Time-Out Status Register                                 */

#define CAN_S_ME        2
#define CAN_S_MD        2
#define CAN_S_TRS       2
#define CAN_S_TRR       2
#define CAN_S_TA        2
#define CAN_S_AA        2
#define CAN_S_RMP       2
#define CAN_S_RML       2
#define CAN_S_RFP       2
#define CAN_S_GAM       2
#define CAN_S_MC        2
#define CAN_S_BTC       2
#define CAN_S_ES        2
#define CAN_S_TEC       2
#define CAN_S_REC       2
#define CAN_S_GIF0      2
#define CAN_S_GIM       2
#define CAN_S_GIF1      2
#define CAN_S_MIM       2
#define CAN_S_MIL       2
#define CAN_S_OPC       2
#define CAN_S_TIOC      2
#define CAN_S_RIOC      2
#define CAN_S_TSC       2
#define CAN_S_TOC       2
#define CAN_S_TOS       2

#define CAN_GAM_GAM_S           0U
#define CAN_GAM_GAM_M           0x1FFFFFFFUL
#define CAN_GAM_AMI             0x80000000UL

#define CAN_MC_MBNR_S           0U
#define CAN_MC_MBNR_M           0x0000001FUL
#define CAN_MC_SRES             0x00000020UL
#define CAN_MC_STM              0x00000040UL
#define CAN_MC_ABO              0x00000080UL
#define CAN_MC_CDR              0x00000100UL
#define CAN_MC_WUBA             0x00000200UL
#define CAN_MC_DBO              0x00000400UL
#define CAN_MC_PDR              0x00000800UL
#define CAN_MC_CCR              0x00001000UL
#define CAN_MC_SCB              0x00002000UL
#define CAN_MC_TCC              0x00004000UL
#define CAN_MC_MBCC             0x00008000UL
#define CAN_MC_SUSP             0x00010000UL

#define CAN_BTC_TSEG2_S         0U
#define CAN_BTC_TSEG2_M         0x00000007UL
#define CAN_BTC_TSEG1_S         3U
#define CAN_BTC_TSEG1_M         0x00000078UL
#define CAN_BTC_SAM             0x00000080UL
#define CAN_BTC_SJW_S           8U
#define CAN_BTC_SJW_M           0x00000300UL
#define CAN_BTC_BRP_S           16U
#define CAN_BTC_BRP_M           0x00FF0000UL

#define CAN_ES_TM               0x00000001UL
#define CAN_ES_RM               0x00000002UL
#define CAN_ES_PDA              0x00000008UL
#define CAN_ES_CCE              0x00000010UL
#define CAN_ES_SMA              0x00000020UL
#define CAN_ES_EW               0x00010000UL
#define CAN_ES_EP               0x00020000UL
#define CAN_ES_BO               0x00040000UL
#define CAN_ES_ACKE             0x00080000UL
#define CAN_ES_SE               0x00100000UL
#define CAN_ES_CRCE             0x00200000UL
#define CAN_ES_SA1              0x00400000UL
#define CAN_ES_BE               0x00800000UL
#define CAN_ES_FE               0x01000000UL

#define CAN_TEC_TEC_S           0U
#define CAN_TEC_TEC_M           0x000000FFUL

#define CAN_REC_REC_S           0U
#define CAN_REC_REC_M           0x000000FFUL

#define CAN_GIF0_MIV_S          0U
#define CAN_GIF0_MIV_M          0x0000001FUL
#define CAN_GIF0_WLIF           0x00000100UL
#define CAN_GIF0_EPIF           0x00000200UL
#define CAN_GIF0_BOIF           0x00000400UL
#define CAN_GIF0_RMLIF          0x00000800UL
#define CAN_GIF0_WUIF           0x00001000UL
#define CAN_GIF0_WDIF           0x00002000UL
#define CAN_GIF0_AAIF           0x00004000UL
#define CAN_GIF0_GMIF           0x00008000UL
#define CAN_GIF0_TCOF           0x00010000UL
#define CAN_GIF0_MTOF           0x00020000UL

#define CAN_GIM_I0EN            0x00000001UL
#define CAN_GIM_I1EN            0x00000002UL
#define CAN_GIM_GIL             0x00000004UL
#define CAN_GIM_WLIM            0x00000100UL
#define CAN_GIM_EPIM            0x00000200UL
#define CAN_GIM_BOIM            0x00000400UL
#define CAN_GIM_RMLIM           0x00000800UL
#define CAN_GIM_WUIM            0x00001000UL
#define CAN_GIM_WDIM            0x00002000UL
#define CAN_GIM_AAIM            0x00004000UL
#define CAN_GIM_TCOM            0x00010000UL
#define CAN_GIM_MTOM            0x00020000UL

#define CAN_GIF1_MIV_S          0U
#define CAN_GIF1_MIV_M          0x0000001FUL
#define CAN_GIF1_WLIF           0x00000100UL
#define CAN_GIF1_EPIF           0x00000200UL
#define CAN_GIF1_BOIF           0x00000400UL
#define CAN_GIF1_RMLIF          0x00000800UL
#define CAN_GIF1_WUIF           0x00001000UL
#define CAN_GIF1_WDIF           0x00002000UL
#define CAN_GIF1_AAIF           0x00004000UL
#define CAN_GIF1_GMIF           0x00008000UL
#define CAN_GIF1_TCOF           0x00010000UL
#define CAN_GIF1_MTOF           0x00020000UL

#define CAN_TIOC_TXFUNC         0x00000008UL

#define CAN_RIOC_RXFUNC         0x00000008UL

/**************************************************************************************************
 * 
 * The following are defines for eCAN mailbox registers (Sec. 14.7.18, 14.9).
 * BASE REGISTER    CANA_BASE
 * 
 *************************************************************************************************/

#define CAN_O_LAM       0x40U       /* Local-Acceptance Mask Register                           */
#define CAN_O_MOTS      0x80U       /* Message-Object Time Stamp Register                       */
#define CAN_O_MOTO      0xC0U       /* Message-Object Time-Out Registers                        */

#define CAN_S_LAM       2
#define CAN_S_MOTS      2
#define CAN_S_MOTO      2

#define CAN_LAM_LAM_S           0U
#define CAN_LAM_LAM_M           0x1FFFFFFFUL
#define CAN_LAM_LAMI            0x80000000UL

/**************************************************************************************************
 * 
 * The following are defines for eCAN mailbox registers (Sec. 14.7.19, 14.8).
 * BASE REGISTER    CANA_BASE
 * 
 *************************************************************************************************/

#define CAN_O_MSGID     0x100U      /* Message Identifier Register                              */
#define CAN_O_MSGCTRL   0x102U      /* Message Control Register                                 */
#define CAN_O_MDL       0x104U      /* Message-Data Low Register                                */
#define CAN_O_MDH       0x106U      /* Message-Data High Register                               */

#define CAN_S_MSGID     2
#define CAN_S_MSGCTRL   2
#define CAN_S_MDL       2
#define CAN_S_MDH       2

#define CAN_MSGID_ID_S          0U
#define CAN_MSGID_ID_M          0x1FFFFFFFUL
#define CAN_MSGID_AAM           0x20000000UL
#define CAN_MSGID_AME           0x40000000UL
#define CAN_MSGID_IDE           0x80000000UL

#define CAN_MSGCTRL_DLC_S       0U
#define CAN_MSGCTRL_DLC_M       0x0000000FUL
#define CAN_MSGCTRL_RTR         0x00000010UL
#define CAN_MSGCTRL_TPL_S       8U
#define CAN_MSGCTRL_TPL_M       0x00001F00UL

#endif /* HW_CAN_H */
