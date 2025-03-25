/**************************************************************************************************
 * 
 * \file hw_flash.h
 * 
 * \brief Definitions for Flash registers
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef HW_FLASH_H
#define HW_FLASH_H

/**************************************************************************************************
 * 
 * The following are defines for Flash and OTP configuration registers (Sec. 1.1.4).
 * BASE REGISTER    FLASHCFG_BASE
 * 
 *************************************************************************************************/

#define FLASH_O_FOPT            0x0U    /* Flash Option Register                            *E* */
#define FLASH_O_FPWR            0x2U    /* Flash Power Modes Register                       *E* */
#define FLASH_O_FSTATUS         0x3U    /* Status Register                                  *E* */
#define FLASH_O_FSTDBYWAIT      0x4U    /* Flash Sleep to Standby Wait Register             *E* */
#define FLASH_O_FACTIVEWAIT     0x5U    /* Flash Standby to Active Wait Register            *E* */
#define FLASH_O_FBANKWAIT       0x6U    /* Flash Read Access Wait State Register            *E* */
#define FLASH_O_FOTPWAIT        0x7U    /* OTP Read Access Wait State Register              *E* */

#define FLASH_S_FOPT            1
#define FLASH_S_FPWR            1
#define FLASH_S_FSTATUS         1
#define FLASH_S_FSTDBYWAIT      1
#define FLASH_S_FACTIVEWAIT     1
#define FLASH_S_FBANKWAIT       1
#define FLASH_S_FOTPWAIT        1

#define FLASH_FOPT_ENPIPE                   0x0001U

#define FLASH_FPWR_PWR_S                    0U
#define FLASH_FPWR_PWR_M                    0x0003U

#define FLASH_FSTATUS_STDBYWAITS            0x0004U
#define FLASH_FSTATUS_ACTIVEWAITS           0x0008U
#define FLASH_FSTATUS_3VSTAT                0x0100U

#define FLASH_FSTATUS_PWRS_S                0U
#define FLASH_FSTATUS_PWRS_M                0x0003U

#define FLASH_FSTDBYWAIT_STDBYWAIT_S        0U
#define FLASH_FSTDBYWAIT_STDBYWAIT_M        0x01FFU

#define FLASH_FACTIVEWAIT_ACTIVEWAIT_S      0U
#define FLASH_FACTIVEWAIT_ACTIVEWAIT_M      0x01FFU

#define FLASH_FBANKWAIT_RANDWAIT_S          0U
#define FLASH_FBANKWAIT_RANDWAIT_M          0x000FU

#define FLASH_FBANKWAIT_PAGEWAIT_S          8U
#define FLASH_FBANKWAIT_PAGEWAIT_M          0x0F00U

#define FLASH_FOTPWAIT_OTPWAIT_S            0U
#define FLASH_FOTPWAIT_OTPWAIT_M            0x001FU

#endif /* HW_FLASH_H */
