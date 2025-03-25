/**************************************************************************************************
 * 
 * \file hw_can.h
 * 
 * \brief Definitions for system control registers
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef HW_SYSCTL_H
#define HW_SYSCTL_H

/**************************************************************************************************
 * 
 * The following are defines for the PARTID register (Sec. 1.5.3).
 * BASE REGISTER    PARTID_BASE
 * 
 *************************************************************************************************/

#define SYSCTL_O_PARTID         0x0U        /* Part ID Register                                 */

#define SYSCTL_S_PARTID         1

#define SYSCTL_PARTID_PARTNO_S              0U
#define SYSCTL_PARTID_PARTNO_M              0x00FFU
#define SYSCTL_PARTID_PARTTYPE_S            8U
#define SYSCTL_PARTID_PARTTYPE_M            0xFF00U

/**************************************************************************************************
 * 
 * The following are defines for the device emulation registers (Sec. 1.5.3).
 * BASE REGISTER    DEVCFG_BASE
 * 
 *************************************************************************************************/

#define SYSCTL_O_DEVICECNF      0x0U        /* Device Configuration Register                    */
#define SYSCTL_O_CLASSID        0x2U        /* Class ID Register                                */
#define SYSCTL_O_REVID          0x3U        /* Revision ID Register                             */

#define SYSCTL_S_DEVICECNF      2
#define SYSCTL_S_CLASSID        1
#define SYSCTL_S_REVID          1

#define SYSCTL_DEVICECNF_TRST               0x08000000UL
#define SYSCTL_DEVICECNF_ENPROT             0x00080000UL
#define SYSCTL_DEVICECNF_XRS                0x00000020UL
#define SYSCTL_DEVICECNF_VMAPS              0x00000008UL

#define SYSCTL_CLASSID_CLASSID_S            0U
#define SYSCTL_CLASSID_CLASSID_M            0x00FFU

#define SYSCTL_REVID_REVID_S                0U
#define SYSCTL_REVID_REVID_M                0xFFFFU

/**************************************************************************************************
 * 
 * The following are defines for the clocking and system control registers (Sec. 1.3.1).
 * BASE REGISTER    CLKCFG_BASE
 * 
 *************************************************************************************************/

#define SYSCTL_O_XCLK           0x0U        /* XCLKOUT/XCLKIN Control                       *E* */
#define SYSCTL_O_PLLSTS         0x1U        /* PLL Status Register                          *E* */
#define SYSCTL_O_CLKCTL         0x2U        /* Clock Control Register                       *E* */
#define SYSCTL_O_PLLLOCKPRD     0x3U        /* PLL Lock Period Register                     *E* */
#define SYSCTL_O_INTOSC1TRIM    0x4U        /* Internal Oscillator 1 Trim Register          *E* */
#define SYSCTL_O_INTOSC2TRIM    0x6U        /* Internal Oscillator 2 Trim Register          *E* */
#define SYSCTL_O_PCLKCR2        0x9U        /* Peripheral Clock Control Register 2              */
#define SYSCTL_O_LOSPCP         0xBU        /* Low-Speed Peripheral Clock Pre-Scaler        *E* */
#define SYSCTL_O_PCLKCR0        0xCU        /* Peripheral Clock Control Register 0          *E* */
#define SYSCTL_O_PCLKCR1        0xDU        /* Peripheral Clock Control Register 1          *E* */
#define SYSCTL_O_LPMCR0         0xEU        /* Low Power Mode Control Register 0            *E* */
#define SYSCTL_O_PCLKCR3        0x10U       /* Peripheral Clock Control Register 3          *E* */
#define SYSCTL_O_PLLCR          0x11U       /* PLL Control Register                         *E* */
#define SYSCTL_O_SCSR           0x12U       /* System Control and Status Register           *E* */
#define SYSCTL_O_WDCNTR         0x13U       /* Watchdog Counter Register                    *E* */
#define SYSCTL_O_WDKEY          0x15U       /* Watchdog Reset Key Register                  *E* */
#define SYSCTL_O_WDCR           0x19U       /* Watchdog Control Register                    *E* */

#define SYSCTL_S_XCLK           1
#define SYSCTL_S_PLLSTS         1
#define SYSCTL_S_CLKCTL         1
#define SYSCTL_S_PLLLOCKPRD     1
#define SYSCTL_S_INTOSC1TRIM    1
#define SYSCTL_S_INTOSC2TRIM    1
#define SYSCTL_S_PCLKCR2        1
#define SYSCTL_S_LOSPCP         1
#define SYSCTL_S_PCLKCR0        1
#define SYSCTL_S_PCLKCR1        1
#define SYSCTL_S_LPMCR0         1
#define SYSCTL_S_PCLKCR3        1
#define SYSCTL_S_PLLCR          1
#define SYSCTL_S_SCSR           1
#define SYSCTL_S_WDCNTR         1
#define SYSCTL_S_WDKEY          1
#define SYSCTL_S_WDCR           1

#define SYSCTL_XCLK_XCLKOUTDIV_S            0U
#define SYSCTL_XCLK_XCLKOUTDIV_M            0x0003U
#define SYSCTL_XCLK_XCLKINSEL               0x0040U

#define SYSCTL_PLLSTS_PLLLOCKS              0x0001U
#define SYSCTL_PLLSTS_PLLOFF                0x0004U
#define SYSCTL_PLLSTS_MCLKSTS               0x0008U
#define SYSCTL_PLLSTS_MCLKCLR               0x0010U
#define SYSCTL_PLLSTS_OSCOFF                0x0020U
#define SYSCTL_PLLSTS_MCLKOFF               0x0040U
#define SYSCTL_PLLSTS_DIVSEL_S              7U
#define SYSCTL_PLLSTS_DIVSEL_M              0x0180U
#define SYSCTL_PLLSTS_NORMRDYE              0x8000U

#define SYSCTL_CLKCTL_OSCCLKSRCSEL          0x0001U
#define SYSCTL_CLKCTL_OSCCLKSRC2SEL         0x0002U
#define SYSCTL_CLKCTL_WDCLKSRCSEL           0x0004U
#define SYSCTL_CLKCTL_TMR2CLKSRCSEL_S       3U
#define SYSCTL_CLKCTL_TMR2CLKSRCSEL_M       0x0018U
#define SYSCTL_CLKCTL_TMR2CLKPRESCALE_S     5U
#define SYSCTL_CLKCTL_TMR2CLKPRESCALE_M     0x00E0U
#define SYSCTL_CLKCTL_INTOSC1OFF            0x0100U
#define SYSCTL_CLKCTL_INTOSC1HALTI          0x0200U
#define SYSCTL_CLKCTL_INTOSC2OFF            0x0400U
#define SYSCTL_CLKCTL_INTOSC2HALTI          0x0800U
#define SYSCTL_CLKCTL_WDHALTI               0x1000U
#define SYSCTL_CLKCTL_XCLKINOFF             0x2000U
#define SYSCTL_CLKCTL_XTALOSCOFF            0x4000U
#define SYSCTL_CLKCTL_NMIRESETSEL           0x8000U

#define SYSCTL_PLLLOCKPRD_PLLLOCKPRD_S      0U
#define SYSCTL_PLLLOCKPRD_PLLLOCKPRD_M      0xFFFFU

#define SYSCTL_INTOSC1TRIM_COARSETRIM_S     0U
#define SYSCTL_INTOSC1TRIM_COARSETRIM_M     0x00FFU
#define SYSCTL_INTOSC1TRIM_FINETRIM_S       9U
#define SYSCTL_INTOSC1TRIM_FINETRIM_M       0x7E00U

#define SYSCTL_INTOSC2TRIM_COARSETRIM_S     0U
#define SYSCTL_INTOSC2TRIM_COARSETRIM_M     0x00FFU
#define SYSCTL_INTOSC2TRIM_FINETRIM_S       9U
#define SYSCTL_INTOSC2TRIM_FINETRIM_M       0x7E00U

#define SYSCTL_LOSPCP_LSPCLK_S              0U
#define SYSCTL_LOSPCP_LSPCLK_M              0x0007U

#define SYSCTL_LPMCR0_LPM_S                 0U
#define SYSCTL_LPMCR0_LPM_M                 0x0003U
#define SYSCTL_LPMCR0_QUALSTDBY_S           2U
#define SYSCTL_LPMCR0_QUALSTDBY_M           0x00FCU
#define SYSCTL_LPMCR0_WDINTE                0x8000U

#define SYSCTL_PLLCR_DIV_S                  0U
#define SYSCTL_PLLCR_DIV_M                  0x000FU

#define SYSCTL_SCSR_WDOVERRIDE              0x0001U
#define SYSCTL_SCSR_WDENINT                 0x0002U
#define SYSCTL_SCSR_WDINTS                  0x0004U

#define SYSCTL_WDCNTR_WDCNTR_S              0U
#define SYSCTL_WDCNTR_WDCNTR_M              0x00FFU

#define SYSCTL_WDKEY_WDKEY_S                0U
#define SYSCTL_WDKEY_WDKEY_M                0x00FFU

#define SYSCTL_WDCR_WDPS_S                  0U
#define SYSCTL_WDCR_WDPS_M                  0x0007U
#define SYSCTL_WDCR_WDCHK_S                 3U
#define SYSCTL_WDCR_WDCHK_M                 0x0038U
#define SYSCTL_WDCR_WDDIS                   0x0040U
#define SYSCTL_WDCR_WDFLAG                  0x0080U

#endif /* HW_SYSCTL_H */
