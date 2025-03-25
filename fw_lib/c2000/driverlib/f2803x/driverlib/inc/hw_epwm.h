/**************************************************************************************************
 * 
 * \file hw_epwm.h
 * 
 * \brief Definitions for ePWM registers
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef HW_EPWM_H
#define HW_EPWM_H

/**************************************************************************************************
 * 
 * The following are defines for ePWM time-base submodule registers (Sec. 3.2.2)
 * BASE REGISTER    EPWM1_BASE
 * 
 *************************************************************************************************/

#define EPWM_O_TBCTL            0x0U    /* Time-Base Control Register                           */
#define EPWM_O_TBSTS            0x1U    /* Time-Base Status Register                            */
#define EPWM_O_TBPHSHR          0x2U    /* Extension for HRPWM Phase Register                   */
#define EPWM_O_TBPHS            0x3U    /* Time-Base Phase Register                             */
#define EPWM_O_TBCTR            0x4U    /* Time-Base Counter Register                           */
#define EPWM_O_TBPRD            0x5U    /* Time-Base Period Register                            */
#define EPWM_O_TBPRDHR          0x6U    /* Time-Base Period High Resolution Register            */

#define EPWM_S_TBCTL            1
#define EPWM_S_TBSTS            1
#define EPWM_S_TBPHSHR          1
#define EPWM_S_TBPHS            1
#define EPWM_S_TBCTR            1
#define EPWM_S_TBPRD            1
#define EPWM_S_TBPRDHR          1

#define EPWM_TBCTL_CTRMODE_S        0U
#define EPWM_TBCTL_CTRMODE_M        0x0003U
#define EPWM_TBCTL_PHSEN            0x0004U
#define EPWM_TBCTL_PRDLD            0x0008U
#define EPWM_TBCTL_SYNCOSEL_S       4U
#define EPWM_TBCTL_SYNCOSEL_M       0x0030U
#define EPWM_TBCTL_SWFSYNC          0x0040U
#define EPWM_TBCTL_HSPCLKDIV_S      7U
#define EPWM_TBCTL_HSPCLKDIV_M      0x0380U
#define EPWM_TBCTL_CLKDIV_S         10U
#define EPWM_TBCTL_CLKDIV_M         0x1C00U
#define EPWM_TBCTL_PHSDIR           0x2000U
#define EPWM_TBCTL_FREESOFT_S       14U
#define EPWM_TBCTL_FREESOFT_M       0xC000U

#define EPWM_TBSTS_CTRDIR           0x0001U
#define EPWM_TBSTS_SYNCI            0x0002U
#define EPWM_TBSTS_CTRMAX           0x0004U

#define EPWM_TBPHSHR_TBPHSHR_S      8U
#define EPWM_TBPHSHR_TBPHSHR_M      0xFF00U

#define EPWM_TBPHS_TBPHS_S          0U
#define EPWM_TBPHS_TBPHS_M          0xFFFFU

#define EPWM_TBCTR_TBCTR_S          0U
#define EPWM_TBCTR_TBCTR_M          0xFFFFU

#define EPWM_TBPRD_TBPRD_S          0U
#define EPWM_TBPRD_TBPRD_M          0xFFFFU

#define EPWM_TBPRDHR_TBPRDHR_S      8U
#define EPWM_TBPRDHR_TBPRDHR_M      0xFF00U

/**************************************************************************************************
 * 
 * The following are defines for ePWM counter-compare submodule registers (Sec. 3.2.3)
 * BASE REGISTER    EPWM1_BASE
 * 
 *************************************************************************************************/

#define EPWM_O_CMPCTL           0x7U    /* Counter-Compare Control Register                     */
#define EPWM_O_CMPAHR           0x8U    /* Extension for HRPWM Counter-Compare A Register       */
#define EPWM_O_CMPA             0x9U    /* Counter-Compare A Register                           */
#define EPWM_O_CMPB             0xAU    /* Counter-Compare B Register                           */

#define EPWM_S_CMPCTL           1
#define EPWM_S_CMPAHR           1
#define EPWM_S_CMPA             1
#define EPWM_S_CMPB             1

#define EPWM_CMPCTL_LOADAMODE_S     0U
#define EPWM_CMPCTL_LOADAMODE_M     0x0003U
#define EPWM_CMPCTL_LOADBMODE_S     2U
#define EPWM_CMPCTL_LOADBMODE_M     0x000CU
#define EPWM_CMPCTL_SHDWAMODE       0x0010U
#define EPWM_CMPCTL_SHDWBMODE       0x0040U
#define EPWM_CMPCTL_SHDWAFULL       0x0100U
#define EPWM_CMPCTL_SHDWBFULL       0x0200U

#define EPWM_CMPAHR_CMPAHR_S        8U
#define EPWM_CMPAHR_CMPAHR_M        0xFF00U

#define EPWM_CMPA_CMPA_S            0U
#define EPWM_CMPA_CMPA_M            0xFFFFU

#define EPWM_CMPB_CMPB_S            0U
#define EPWM_CMPB_CMPB_M            0xFFFFU

/**************************************************************************************************
 * 
 * The following are defines for ePWM action-qualifier submodule registers (Sec. 3.2.4)
 * BASE REGISTER    EPWM1_BASE
 * 
 *************************************************************************************************/

#define EPWM_O_AQCTLA           0xBU    /* Action-Qualifier Control Register for Output A       */
#define EPWM_O_AQCTLB           0xCU    /* Action-Qualifier Control Register for Output B       */
#define EPWM_O_AQSFRC           0xDU    /* Action-Qualifier Software Force Register             */
#define EPWM_O_AQCSFRC          0xEU    /* Action-Qualifier Continuous S/W Force Register Set   */

#define EPWM_S_AQCTLA           1
#define EPWM_S_AQCTLB           1
#define EPWM_S_AQSFRC           1
#define EPWM_S_AQCSFRC          1

#define EPWM_AQCTLA_ZRO_S           0U
#define EPWM_AQCTLA_ZRO_M           0x0003U
#define EPWM_AQCTLA_PRD_S           2U
#define EPWM_AQCTLA_PRD_M           0x000CU
#define EPWM_AQCTLA_CAU_S           4U
#define EPWM_AQCTLA_CAU_M           0x0030U
#define EPWM_AQCTLA_CAD_S           6U
#define EPWM_AQCTLA_CAD_M           0x00C0U
#define EPWM_AQCTLA_CBU_S           8U
#define EPWM_AQCTLA_CBU_M           0x0300U
#define EPWM_AQCTLA_CBD_S           10U
#define EPWM_AQCTLA_CBD_M           0x0C00U

#define EPWM_AQCTLB_ZRO_S           0U
#define EPWM_AQCTLB_ZRO_M           0x0003U
#define EPWM_AQCTLB_PRD_S           2U
#define EPWM_AQCTLB_PRD_M           0x000CU
#define EPWM_AQCTLB_CAU_S           4U
#define EPWM_AQCTLB_CAU_M           0x0030U
#define EPWM_AQCTLB_CAD_S           6U
#define EPWM_AQCTLB_CAD_M           0x00C0U
#define EPWM_AQCTLB_CBU_S           8U
#define EPWM_AQCTLB_CBU_M           0x0300U
#define EPWM_AQCTLB_CBD_S           10U
#define EPWM_AQCTLB_CBD_M           0x0C00U

#define EPWM_AQSFRC_ACTSFA_S        0U
#define EPWM_AQSFRC_ACTSFA_M        0x0003U
#define EPWM_AQSFRC_OTSFA           0x0004U
#define EPWM_AQSFRC_ACTSFB_S        3U
#define EPWM_AQSFRC_ACTSFB_M        0x0018U
#define EPWM_AQSFRC_OTSFB           0x0020U
#define EPWM_AQSFRC_RLDCSF_S        6U
#define EPWM_AQSFRC_RLDCSF_M        0x00C0U

#define EPWM_AQCSFRC_CSFA_S         0U
#define EPWM_AQCSFRC_CSFA_M         0x0003U
#define EPWM_AQCSFRC_CSFB_S         2U
#define EPWM_AQCSFRC_CSFB_M         0x000CU

/**************************************************************************************************
 * 
 * The following are defines for ePWM dead-band generator submodule registers (Sec. 3.2.5)
 * BASE REGISTER    EPWM1_BASE
 * 
 *************************************************************************************************/

#define EPWM_O_DBCTL            0xFU    /* Dead-Band Generator Control Register                 */
#define EPWM_O_DBRED            0x10U   /* Dead-Band Generator Rising Edge Delay Register       */
#define EPWM_O_DBFED            0x11U   /* Dead-Band Generator Falling Edge Delay Register      */

#define EPWM_S_DBCTL            1
#define EPWM_S_DBRED            1
#define EPWM_S_DBFED            1

#define EPWM_DBCTL_OUTMODE_S        0U
#define EPWM_DBCTL_OUTMODE_M        0x0003U
#define EPWM_DBCTL_POLSEL_S         2U
#define EPWM_DBCTL_POLSEL_M         0x000CU
#define EPWM_DBCTL_INMODE_S         4U
#define EPWM_DBCTL_INMODE_M         0x0030U
#define EPWM_DBCTL_HALFCYCLE        0x8000U

#define EPWM_DBRED_DEL_S            0U
#define EPWM_DBRED_DEL_M            0x03FFU

#define EPWM_DBFED_DEL_S            0U
#define EPWM_DBFED_DEL_M            0x03FFU

/**************************************************************************************************
 * 
 * The following are defines for ePWM trip-zone submodule registers (Sec. 3.2.7)
 * BASE REGISTER    EPWM1_BASE
 * 
 *************************************************************************************************/

#define EPWM_O_TZSEL            0x12U   /* Trip-Zone Select Register                        *E* */
#define EPWM_O_TZDCSEL          0x13U   /* Trip-Zone Digital Compare Select Register        *E* */
#define EPWM_O_TZCTL            0x14U   /* Trip-Zone Control Register                       *E* */
#define EPWM_O_TZEINT           0x15U   /* Trip-Zone Enable Interrupt Register              *E* */
#define EPWM_O_TZFLG            0x16U   /* Trip-Zone Flag Register                              */
#define EPWM_O_TZCLR            0x17U   /* Trip-Zone Clear Register                         *E* */
#define EPWM_O_TZFRC            0x18U   /* Trip-Zone Force Register                         *E* */

#define EPWM_S_TZSEL            1
#define EPWM_S_TZDCSEL          1
#define EPWM_S_TZCTL            1
#define EPWM_S_TZEINT           1
#define EPWM_S_TZFLG            1
#define EPWM_S_TZCLR            1
#define EPWM_S_TZFRC            1

#define EPWM_TZSEL_CBC1             0x0001U
#define EPWM_TZSEL_CBC2             0x0002U
#define EPWM_TZSEL_CBC3             0x0004U
#define EPWM_TZSEL_CBC4             0x0008U
#define EPWM_TZSEL_CBC5             0x0010U
#define EPWM_TZSEL_CBC6             0x0020U
#define EPWM_TZSEL_DCAEVT2          0x0040U
#define EPWM_TZSEL_DCBEVT2          0x0080U
#define EPWM_TZSEL_OSHT1            0x0100U
#define EPWM_TZSEL_OSHT2            0x0200U
#define EPWM_TZSEL_OSHT3            0x0400U
#define EPWM_TZSEL_OSHT4            0x0800U
#define EPWM_TZSEL_OSHT5            0x1000U
#define EPWM_TZSEL_OSHT6            0x2000U
#define EPWM_TZSEL_DCAEVT1          0x4000U
#define EPWM_TZSEL_DCBEVT1          0x8000U

#define EPWM_TZDCSEL_DCAEVT1_S      0U
#define EPWM_TZDCSEL_DCAEVT1_M      0x0007U
#define EPWM_TZDCSEL_DCAEVT2_S      3U
#define EPWM_TZDCSEL_DCAEVT2_M      0x0038U
#define EPWM_TZDCSEL_DCBEVT1_S      6U
#define EPWM_TZDCSEL_DCBEVT1_M      0x01C0U
#define EPWM_TZDCSEL_DCBEVT2_S      9U
#define EPWM_TZDCSEL_DCBEVT2_M      0x0E00U

#define EPWM_TZCTL_TZA_S            0U
#define EPWM_TZCTL_TZA_M            0x0003U
#define EPWM_TZCTL_TZB_S            2U
#define EPWM_TZCTL_TZB_M            0x000CU
#define EPWM_TZCTL_DCAEVT1_S        4U
#define EPWM_TZCTL_DCAEVT1_M        0x0030U
#define EPWM_TZCTL_DCAEVT2_S        6U
#define EPWM_TZCTL_DCAEVT2_M        0x00C0U
#define EPWM_TZCTL_DCBEVT1_S        8U
#define EPWM_TZCTL_DCBEVT1_M        0x0300U
#define EPWM_TZCTL_DCBEVT2_S        10U
#define EPWM_TZCTL_DCBEVT2_M        0x0C00U

#define EPWM_TZEINT_CBC             0x0002U
#define EPWM_TZEINT_OST             0x0004U
#define EPWM_TZEINT_DCAEVT1         0x0008U
#define EPWM_TZEINT_DCAEVT2         0x0010U
#define EPWM_TZEINT_DCBEVT1         0x0020U
#define EPWM_TZEINT_DCBEVT2         0x0040U

#define EPWM_TZFLG_INT              0x0001U
#define EPWM_TZFLG_CBC              0x0002U
#define EPWM_TZFLG_OST              0x0004U
#define EPWM_TZFLG_DCAEVT1          0x0008U
#define EPWM_TZFLG_DCAEVT2          0x0010U
#define EPWM_TZFLG_DCBEVT1          0x0020U
#define EPWM_TZFLG_DCBEVT2          0x0040U

#define EPWM_TZCLR_INT              0x0001U
#define EPWM_TZCLR_CBC              0x0002U
#define EPWM_TZCLR_OST              0x0004U
#define EPWM_TZCLR_DCAEVT1          0x0008U
#define EPWM_TZCLR_DCAEVT2          0x0010U
#define EPWM_TZCLR_DCBEVT1          0x0020U
#define EPWM_TZCLR_DCBEVT2          0x0040U

#define EPWM_TZFRC_CBC              0x0002U
#define EPWM_TZFRC_OST              0x0004U
#define EPWM_TZFRC_DCAEVT1          0x0008U
#define EPWM_TZFRC_DCAEVT2          0x0010U
#define EPWM_TZFRC_DCBEVT1          0x0020U
#define EPWM_TZFRC_DCBEVT2          0x0040U

/**************************************************************************************************
 * 
 * The following are defines for ePWM event-trigger submodule registers (Sec. 3.2.8)
 * BASE REGISTER    EPWM1_BASE
 * 
 *************************************************************************************************/

#define EPWM_O_ETSEL            0x19U   /* Event-Trigger Selection Register                     */
#define EPWM_O_ETPS             0x1AU   /* Event-Trigger Prescale Register                      */
#define EPWM_O_ETFLG            0x1BU   /* Event-Trigger Flag Register                          */
#define EPWM_O_ETCLR            0x1CU   /* Event-Trigger Clear Register                         */
#define EPWM_O_ETFRC            0x1DU   /* Event-Trigger Force Register                         */

#define EPWM_S_ETSEL            1
#define EPWM_S_ETPS             1
#define EPWM_S_ETFLG            1
#define EPWM_S_ETCLR            1
#define EPWM_S_ETFRC            1

#define EPWM_ETSEL_INTSEL_S         0U
#define EPWM_ETSEL_INTSEL_M         0x0007U
#define EPWM_ETSEL_INTEN            0x0008U
#define EPWM_ETSEL_SOCASEL_S        8U
#define EPWM_ETSEL_SOCASEL_M        0x0700U
#define EPWM_ETSEL_SOCAEN           0x0800U
#define EPWM_ETSEL_SOCBSEL_S        12U
#define EPWM_ETSEL_SOCBSEL_M        0x7000U
#define EPWM_ETSEL_SOCBEN           0x8000U

#define EPWM_ETPS_INTPRD_S          0U
#define EPWM_ETPS_INTPRD_M          0x0003U
#define EPWM_ETPS_INTCNT_S          2U
#define EPWM_ETPS_INTCNT_M          0x000CU
#define EPWM_ETPS_SOCAPRD_S         8U
#define EPWM_ETPS_SOCAPRD_M         0x0300U
#define EPWM_ETPS_SOCACNT_S         10U
#define EPWM_ETPS_SOCACNT_M         0x0C00U
#define EPWM_ETPS_SOCBPRD_S         12U
#define EPWM_ETPS_SOCBPRD_M         0x3000U
#define EPWM_ETPS_SOCBCNT_S         14U
#define EPWM_ETPS_SOCBCNT_M         0xC000U

#define EPWM_ETFLG_INT              0x0001U
#define EPWM_ETFLG_SOCA             0x0004U
#define EPWM_ETFLG_SOCB             0x0008U

#define EPWM_ETCLR_INT              0x0001U
#define EPWM_ETCLR_SOCA             0x0004U
#define EPWM_ETCLR_SOCB             0x0008U

#define EPWM_ETFRC_INT              0x0001U
#define EPWM_ETFRC_SOCA             0x0004U
#define EPWM_ETFRC_SOCB             0x0008U

/**************************************************************************************************
 * 
 * The following are defines for ePWM PWM-chopper submodule registers (Sec. 3.2.6)
 * BASE REGISTER    EPWM1_BASE
 * 
 *************************************************************************************************/

#define EPWM_O_PCCTL            0x1EU   /* PWM-Chopper Control Register                         */

#define EPWM_S_PCCTL            1

#define EPWM_PCCTL_CHPEN            0x0001U
#define EPWM_PCCTL_OSHTWTH_S        1U
#define EPWM_PCCTL_OSHTWTH_M        0x001EU
#define EPWM_PCCTL_CHPFREQ_S        5U
#define EPWM_PCCTL_CHPFREQ_M        0x00E0U
#define EPWM_PCCTL_CHPDUTY_S        8U
#define EPWM_PCCTL_CHPDUTY_M        0x0700U

/**************************************************************************************************
 * 
 * The following are defines for ePWM digital compare event registers (Sec. 3.2.9)
 * BASE REGISTER    EPWM1_BASE
 * 
 *************************************************************************************************/

#define EPWM_O_DCTRIPSEL        0x30U   /* Digital Compare Trip Select Register             *E* */
#define EPWM_O_DCACTL           0x31U   /* Digital Compare A Control Register               *E* */
#define EPWM_O_DCBCTL           0x32U   /* Digital Compare B Control Register               *E* */
#define EPWM_O_DCFCTL           0x33U   /* Digital Compare Filter Control Register          *E* */
#define EPWM_O_DCCAPCTL         0x34U   /* Digital Compare Capture Control Register         *E* */
#define EPWM_O_DCFOFFSET        0x35U   /* Digital Compare Filter Offset Register               */
#define EPWM_O_DCFOFFSETCNT     0x36U   /* Digital Compare Filter Offset Counter Register       */
#define EPWM_O_DCFWINDOW        0x37U   /* Digital Compare Filter Window Register               */
#define EPWM_O_DCFWINDOWCNT     0x38U   /* Digital Compare Filter Window Counter Register       */
#define EPWM_O_DCCAP            0x39U   /* Digital Compare Counter Capture Register             */

#define EPWM_S_DCTRIPSEL        1
#define EPWM_S_DCACTL           1
#define EPWM_S_DCBCTL           1
#define EPWM_S_DCFCTL           1
#define EPWM_S_DCCAPCTL         1
#define EPWM_S_DCFOFFSET        1
#define EPWM_S_DCFOFFSETCNT     1
#define EPWM_S_DCFWINDOW        1
#define EPWM_S_DCFWINDOWCNT     1
#define EPWM_S_DCCAP            1

#define EPWM_DCTRIPSEL_DCAHCOMPSEL_S    0U
#define EPWM_DCTRIPSEL_DCAHCOMPSEL_M    0x000FU
#define EPWM_DCTRIPSEL_DCALCOMPSEL_S    4U
#define EPWM_DCTRIPSEL_DCALCOMPSEL_M    0x00F0U
#define EPWM_DCTRIPSEL_DCBHCOMPSEL_S    8U
#define EPWM_DCTRIPSEL_DCBHCOMPSEL_M    0x0F00U
#define EPWM_DCTRIPSEL_DCBLCOMPSEL_S    12U
#define EPWM_DCTRIPSEL_DCBLCOMPSEL_M    0xF000U

#define EPWM_DCACTL_EVT1SRCSEL          0x0001U
#define EPWM_DCACTL_EVT1FRC             0x0002U
#define EPWM_DCACTL_EVT1SOCE            0x0004U
#define EPWM_DCACTL_EVT1SYNCE           0x0008U
#define EPWM_DCACTL_EVT2SRCSEL          0x0100U
#define EPWM_DCACTL_EVT2FRC             0x0200U

#define EPWM_DCBCTL_EVT1SRCSEL          0x0001U
#define EPWM_DCBCTL_EVT1FRC             0x0002U
#define EPWM_DCBCTL_EVT1SOCE            0x0004U
#define EPWM_DCBCTL_EVT1SYNCE           0x0008U
#define EPWM_DCBCTL_EVT2SRCSEL          0x0100U
#define EPWM_DCBCTL_EVT2FRC             0x0200U

#define EPWM_DCFCTL_SRCSEL_S            0U
#define EPWM_DCFCTL_SRCSEL_M            0x0003U
#define EPWM_DCFCTL_BLANKE              0x0004U
#define EPWM_DCFCTL_BLANKINV            0x0008U
#define EPWM_DCFCTL_PULSESEL_S          4U
#define EPWM_DCFCTL_PULSESEL_M          0x0030U

#define EPWM_DCCAPCTL_CAPE              0x0001U
#define EPWM_DCCAPCTL_SHDWMODE          0x0002U

#define EPWM_DCFOFFSET_OFFSET_S         0U
#define EPWM_DCFOFFSET_OFFSET_M         0xFFFFU

#define EPWM_DCFOFFSETCNT_OFFSETCNT_S   0U
#define EPWM_DCFOFFSETCNT_OFFSETCNT_M   0xFFFFU

#define EPWM_DCFWINDOW_WINDOW_S         0U
#define EPWM_DCFWINDOW_WINDOW_M         0x00FFU

#define EPWM_DCFWINDOWCNT_WINDOWCNT_S   0U
#define EPWM_DCFWINDOWCNT_WINDOWCNT_M   0x00FFU

#define EPWM_DCCAP_DCCAP_S              0U
#define EPWM_DCCAP_DCCAP_M              0xFFFFU

#endif /* HW_EPWM_H */
