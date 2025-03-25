/**************************************************************************************************
 * 
 * \file hw_ints.h
 * 
 * \brief Definitions for interrupts
 * 
 * \details The data is encoded as follows:
 * - 0x000000FF     PIE table row number
 * - 0x0000FF00     PIE table column number
 * - 0xFFFF0000     PIE vector ID
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef HW_INTS_H
#define HW_INTS_H

#define INT_ADCA1_1         0x00200101UL        /* 1.1 ADCA Interrupt 1                         */
#define INT_ADCA2_1         0x00210102UL        /* 1.2 ADCA Interrupt 2                         */
#define INT_XINT1           0x00230104UL        /* 1.3 XINT1 Interrupt                          */
#define INT_XINT2           0x00240105UL        /* 1.4 XINT2 Interrupt                          */
#define INT_ADCA9_1         0x00250106UL        /* 1.5 ADCA Interrupt 9                         */
#define INT_TIMER0          0x00260107UL        /* 1.6 CPU Timer 0 Interrupt                    */
#define INT_WAKE            0x00270108UL        /* 1.7 Halt Wakeup/Watchdog Interrupt           */

#define INT_EPWM1_TZ        0x00280201UL        /* 2.1 ePWM1 Trip Zone Interrupt                */
#define INT_EPWM2_TZ        0x00290202UL        /* 2.2 ePWM2 Trip Zone Interrupt                */
#define INT_EPWM3_TZ        0x002A0203UL        /* 2.3 ePWM3 Trip Zone Interrupt                */
#define INT_EPWM4_TZ        0x002B0204UL        /* 2.4 ePWM4 Trip Zone Interrupt                */
#define INT_EPWM5_TZ        0x002C0205UL        /* 2.5 ePWM5 Trip Zone Interrupt                */
#define INT_EPWM6_TZ        0x002D0206UL        /* 2.6 ePWM6 Trip Zone Interrupt                */
#define INT_EPWM7_TZ        0x002E0207UL        /* 2.7 ePWM7 Trip Zone Interrupt                */

#define INT_EPWM1           0x00300301UL        /* 3.1 ePWM1 Interrupt                          */
#define INT_EPWM2           0x00310302UL        /* 3.2 ePWM2 Interrupt                          */
#define INT_EPWM3           0x00320303UL        /* 3.3 ePWM3 Interrupt                          */
#define INT_EPWM4           0x00330304UL        /* 3.4 ePWM4 Interrupt                          */
#define INT_EPWM5           0x00340305UL        /* 3.5 ePWM5 Interrupt                          */
#define INT_EPWM6           0x00350306UL        /* 3.6 ePWM6 Interrupt                          */
#define INT_EPWM7           0x00360307UL        /* 3.7 ePWM7 Interrupt                          */

#define INT_ECAP1           0x00380401UL        /* 4.1 eCAP1 Interrupt                          */

#define INT_EQEP1           0x00400501UL        /* 5.1 eQEP1 Interrupt                          */

#define INT_SPIA_RX         0x00480601UL        /* 6.1 SPIA Receive Interrupt                   */
#define INT_SPIA_TX         0x00490602UL        /* 6.2 SPIA Transmit Interrupt                  */
#define INT_SPIB_RX         0x004A0603UL        /* 6.3 SPIB Receive Interrupt                   */
#define INT_SPIB_TX         0x004B0604UL        /* 6.4 SPIB Transmit Interrupt                  */

#define INT_I2CA            0x00580801UL        /* 8.1 I2CA Interrupt 1                         */
#define INT_I2CA_FIFO       0x00590802UL        /* 8.2 I2CA Interrupt 2                         */

#define INT_SCIA_RX         0x00600901UL        /* 9.1 SCIA Receive Interrupt                   */
#define INT_SCIA_TX         0x00610902UL        /* 9.2 SCIB Transmit Interrupt                  */
#define INT_LINA_0          0x00620903UL        /* 9.3 LINA Interrupt 0                         */
#define INT_LINA_1          0x00630904UL        /* 9.4 LINA Interrupt 1                         */
#define INT_CANA0           0x00640905UL        /* 9.5 CANA Interrupt 0                         */
#define INT_CANA1           0x00650906UL        /* 9.6 CANA Interrupt 1                         */

#define INT_ADCA1_10        0x00680A01UL        /* 10.1 ADCA Interrupt 1                        */
#define INT_ADCA2_10        0x00690A02UL        /* 10.2 ADCA Interrupt 2                        */
#define INT_ADCA3_10        0x006A0A03UL        /* 10.3 ADCA Interrupt 3                        */
#define INT_ADCA4_10        0x006B0A04UL        /* 10.4 ADCA Interrupt 4                        */
#define INT_ADCA5_10        0x006C0A05UL        /* 10.5 ADCA Interrupt 5                        */
#define INT_ADCA6_10        0x006D0A06UL        /* 10.6 ADCA Interrupt 6                        */
#define INT_ADCA7_10        0x006E0A07UL        /* 10.7 ADCA Interrupt 7                        */
#define INT_ADCA8_10        0x006F0A08UL        /* 10.8 ADCA Interrupt 8                        */

#define INT_CLA1_1          0x00700B01UL        /* 11.1 CLA1 Interrupt 1                        */
#define INT_CLA1_2          0x00710B02UL        /* 11.2 CLA1 Interrupt 2                        */
#define INT_CLA1_3          0x00720B03UL        /* 11.3 CLA1 Interrupt 3                        */
#define INT_CLA1_4          0x00730B04UL        /* 11.4 CLA1 Interrupt 4                        */
#define INT_CLA1_5          0x00740B05UL        /* 11.5 CLA1 Interrupt 5                        */
#define INT_CLA1_6          0x00750B06UL        /* 11.6 CLA1 Interrupt 6                        */
#define INT_CLA1_7          0x00760B07UL        /* 11.7 CLA1 Interrupt 7                        */
#define INT_CLA1_8          0x00770B08UL        /* 11.8 CLA1 Interrupt 8                        */

#define INT_XINT3           0x00780C01UL        /* 12.1 XINT3 Interrupt                         */
#define INT_CLA1_LVF        0x007E0C07UL        /* 12.7 CLA Latched Overflow Interrupt          */
#define INT_CLA1_LUF        0x007F0C08UL        /* 12.8 CLA Latched Underflow Interrupt         */

#define INT_TIMER1          0x000D0000UL        /* CPU Timer 1 Interrupt                        */
#define INT_TIMER2          0x000E0000UL        /* CPU Timer 2 Interrupt                        */
#define INT_DATALOG         0x000F0000UL        /* Data Logging Interrupt                       */
#define INT_RTOS            0x00100000UL        /* RTOS Interrupt                               */
#define INT_EMU             0x00110000UL        /* Emulation Interrupt                          */
#define INT_NMI             0x00120000UL        /* Non-Maskable Interrupt                       */
#define INT_ILLEGAL         0x00130000UL        /* Illegal Operation Trap                       */
#define INT_USER1           0x00140000UL        /* User Defined Trap 1                          */
#define INT_USER2           0x00150000UL        /* User Defined Trap 2                          */
#define INT_USER3           0x00160000UL        /* User Defined Trap 3                          */
#define INT_USER4           0x00170000UL        /* User Defined Trap 4                          */
#define INT_USER5           0x00180000UL        /* User Defined Trap 5                          */
#define INT_USER6           0x00190000UL        /* User Defined Trap 6                          */
#define INT_USER7           0x001A0000UL        /* User Defined Trap 7                          */
#define INT_USER8           0x001B0000UL        /* User Defined Trap 8                          */
#define INT_USER9           0x001C0000UL        /* User Defined Trap 9                          */
#define INT_USER10          0x001D0000UL        /* User Defined Trap 10                         */
#define INT_USER11          0x001E0000UL        /* User Defined Trap 11                         */
#define INT_USER12          0x001F0000UL        /* User Defined Trap 12                         */

#endif /* HW_INTS_H */
