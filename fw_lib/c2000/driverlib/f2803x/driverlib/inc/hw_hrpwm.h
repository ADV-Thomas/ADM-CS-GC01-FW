/**************************************************************************************************
 * 
 * \file hw_hrpwm.h
 * 
 * \brief Definitions for HRPWM registers
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef HW_HRPWM_H
#define HW_HRPWM_H

/**************************************************************************************************
 * 
 * The following are defines for ePWM high-resolution PWM extension registers (Ch. 4)
 * BASE REGISTER    EPWM1_BASE
 * 
 *************************************************************************************************/

#define HRPWM_O_HRCNFG          0x20U   /* HRPWM Configuration Register                     *E* */
#define HRPWM_O_HRPWR           0x21U   /* HRPWM Power Register                             *E* */
#define HRPWM_O_HRMSTEP         0x26U   /* HRPWM MEP Step Register                          *E* */
#define HRPWM_O_HRPCTL          0x28U   /* High Resolution Period Control Register          *E* */
#define HRPWM_O_TBPRDHRM        0x2AU   /* Time-Base Period High Resolution Register Mirror     */
#define HRPWM_O_TBPRDM          0x2BU   /* Time-Base Period Register Mirror                     */
#define HRPWM_O_CMPAHRM         0x2CU   /* Compare A High Resolution Register Mirror            */
#define HRPWM_O_CMPAM           0x2DU   /* Compare A Register Mirror                            */

#define HRPWM_S_HRCNFG          1
#define HRPWM_S_HRPWR           1
#define HRPWM_S_HRMSTEP         1
#define HRPWM_S_HRPCTL          1
#define HRPWM_S_TBPRDHRM        1
#define HRPWM_S_TBPRDM          1
#define HRPWM_S_CMPAHRM         1
#define HRPWM_S_CMPAM           1

#define HRPWM_HRCNFG_EDGMODE_S      0U
#define HRPWM_HRCNFG_EDGMODE_M      0x0003U
#define HRPWM_HRCNFG_CTLMODE        0x0004U
#define HRPWM_HRCNFG_HRLOAD_S       3U
#define HRPWM_HRCNFG_HRLOAD_M       0x0018U
#define HRPWM_HRCNFG_SELOUTB        0x0020U
#define HRPWM_HRCNFG_AUTOCONV       0x0040U
#define HRPWM_HRCNFG_SWAPAB         0x0080U

#define HRPWM_HRPWR_MEPOFF_S        6U
#define HRPWM_HRPWR_MEPOFF_M        0x03C0U

#define HRPWM_HRMSTEP_HRMSTEP_S     0U
#define HRPWM_HRMSTEP_HRMSTEP_M     0x00FFU

#define HRPWM_HRPCTL_HRPE           0x0001U
#define HRPWM_HRPCTL_PWMSYNCSEL     0x0002U
#define HRPWM_HRPCTL_TBPHSHRLOADE   0x0004U

#define HRPWM_TBPRDHRM_TBPRDHR_S    8U
#define HRPWM_TBPRDHRM_TBPRDHR_M    0xFF00U

#define HRPWM_TBPRDM_TBPRD_S        0U
#define HRPWM_TBPRDM_TBPRD_M        0xFFFFU

#define HRPWM_CMPAHRM_CMPAHR_S      8U
#define HRPWM_CMPAHRM_CMPAHR_M      0xFF00U

#define HRPWM_CMPAM_CMPA_S          0U
#define HRPWM_CMPAM_CMPA_M          0xFFFFU

#endif /* HW_HRPWM_H */
