/**************************************************************************************************
 * 
 * \file hw_adc.h
 * 
 * \brief Definitions for ADC registers
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef HW_ADC_H
#define HW_ADC_H

/**************************************************************************************************
 * 
 * The following are defines for ADC control registers (Sec. 8.12.1-8.12.2)
 * BASE REGISTER    ADCA_BASE
 * 
 *************************************************************************************************/

#define ADC_O_CTL1          0x0U        /* ADC Control 1 Register                           *E* */
#define ADC_O_CTL2          0x1U        /* ADC Control 2 Register                           *E* */

#define ADC_S_CTL1          1
#define ADC_S_CTL2          1

#define ADC_CTL1_TEMPCONV           0x0001U
#define ADC_CTL1_VREFLOCONV         0x0002U
#define ADC_CTL1_INTPULSEPOS        0x0004U
#define ADC_CTL1_ADCREFSEL          0x0008U
#define ADC_CTL1_ADCREFPWD          0x0020U
#define ADC_CTL1_ADCBGPWD           0x0040U
#define ADC_CTL1_ADCPWDN            0x0080U
#define ADC_CTL1_ADCBSYCHN_S        8U
#define ADC_CTL1_ADCBSYCHN_M        0x1F00U
#define ADC_CTL1_ADCBSY             0x2000U
#define ADC_CTL1_ADCENABLE          0x4000U
#define ADC_CTL1_RESET              0x8000U

#define ADC_CTL2_CLKDIV2EN          0x0001U
#define ADC_CTL2_ADCNONOVERLAP      0x0002U

/**************************************************************************************************
 * 
 * The following are defines for ADC interrupt registers (Sec. 8.12.3)
 * BASE REGISTER    ADCA_BASE
 * 
 *************************************************************************************************/

#define ADC_O_INTFLG        0x4U        /* ADC Interrupt Flag Register                          */
#define ADC_O_INTFLGCLR     0x5U        /* ADC Interrupt Flag Clear Register                    */
#define ADC_O_INTOVF        0x6U        /* ADC Interrupt Overflow Register                      */
#define ADC_O_INTOVFCLR     0x7U        /* ADC Interrupt Overflow Clear Register                */
#define ADC_O_INTSELxNy     0x8U        /* ADC Interrupt X and Y Selection Register         *E* */

#define ADC_S_INTFLG        1
#define ADC_S_INTFLGCLR     1
#define ADC_S_INTOVF        1
#define ADC_S_INTOVFCLR     1
#define ADC_S_INTSELxNy     1

#define ADC_INTSELxNy_INTxSEL_S     0U
#define ADC_INTSELxNy_INTxSEL_M     0x001FU
#define ADC_INTSELxNy_INTxE         0x0020U
#define ADC_INTSELxNy_INTxCONT      0x0040U
#define ADC_INTSELxNy_INTySEL_S     8U
#define ADC_INTSELxNy_INTySEL_M     0x1F00U
#define ADC_INTSELxNy_INTyE         0x2000U
#define ADC_INTSELxNy_INTyCONT      0x4000U

/**************************************************************************************************
 * 
 * The following are defines for ADC priority registers (Sec. 8.12.4)
 * BASE REGISTER    ADCA_BASE
 * 
 *************************************************************************************************/

#define ADC_O_SOCPRICTL     0x10U       /* ADC SOC Priority Control Register                *E* */

#define ADC_S_SOCPRICTL     1

#define ADC_SOCPRICTL_SOCPRIORITY_S     0U
#define ADC_SOCPRICTL_SOCPRIORITY_M     0x001FU
#define ADC_SOCPRICTL_RRPOINTER_S       5U
#define ADC_SOCPRICTL_RRPOINTER_M       0x07E0U
#define ADC_SOCPRICTL_ONESHOT           0x8000U

/**************************************************************************************************
 * 
 * The following are defines for ADC SOC registers (Sec. 8.12.5)
 * BASE REGISTER    ADCA_BASE
 * 
 *************************************************************************************************/

#define ADC_O_SAMPLEMODE    0x12U       /* ADC Sampling Mode Register                       *E* */
#define ADC_O_INTSOCSELx    0x14U       /* ADC Interrupt SOC Selection X Register           *E* */
#define ADC_O_SOCFLG1       0x18U       /* ADC SOC Flag 1 Register                              */
#define ADC_O_SOCFRC1       0x1AU       /* ADC SOC Force 1 Register                             */
#define ADC_O_SOCOVF1       0x1CU       /* ADC SOC Overflow 1 Register                          */
#define ADC_O_SOCOVFCLR1    0x1EU       /* ADC SOC Overflow Clear 1 Register                    */
#define ADC_O_SOCxCTL       0x20U       /* ADC SOCx Control Register                        *E* */

#define ADC_S_SAMPLEMODE    1
#define ADC_S_INTSOCSELx    1
#define ADC_S_SOCFLG1       1
#define ADC_S_SOCFRC1       1
#define ADC_S_SOCOVF1       1
#define ADC_S_SOCOVFCLR1    1
#define ADC_S_SOCxCTL       1

#define ADC_SOCxCTL_ACQPS_S         0U
#define ADC_SOCxCTL_ACQPS_M         0x003FU
#define ADC_SOCxCTL_CHSEL_S         6U
#define ADC_SOCxCTL_CHSEL_M         0x03C0U
#define ADC_SOCxCTL_TRIGSEL_S       11U
#define ADC_SOCxCTL_TRIGSEL_M       0xF800U

/**************************************************************************************************
 * 
 * The following are defines for ADC calibration registers (Sec. 8.12.6)
 * BASE REGISTER    ADCA_BASE
 * 
 *************************************************************************************************/

#define ADC_O_REFTRIM       0x40U       /* ADC Reference Trim Register                      *E* */
#define ADC_O_OFFTRIM       0x41U       /* ADC Offset Trim Register                         *E* */

#define ADC_S_REFTRIM       1
#define ADC_S_OFFTRIM       1

#define ADC_REFTRIM_BGFINETRIM_S            0U
#define ADC_REFTRIM_BGFINETRIM_M            0x001FU
#define ADC_REFTRIM_BGCOARSETRIM_S          5U
#define ADC_REFTRIM_BGCOARSETRIM_M          0x01E0U
#define ADC_REFTRIM_EXTREFFINETRIM_S        9U
#define ADC_REFTRIM_EXTREFFINETRIM_M        0x3E00U

#define ADC_OFFTRIM_OFFTRIM_S               0U
#define ADC_OFFTRIM_OFFTRIM_M               0x01FFU

/**************************************************************************************************
 * 
 * The following are defines for comparator hysteresis control register (Sec. 8.12.7)
 * BASE REGISTER    ADCA_BASE
 * 
 *************************************************************************************************/

#define ADC_O_COMPHYSTCTL   0x4CU       /* Comparator Hysteresis Control Register           *E* */

#define ADC_S_COMPHYSTCTL   1

#define ADC_COMPHYSTCTL_COMP1DISABLE        0x0002U
#define ADC_COMPHYSTCTL_COMP2DISABLE        0x0040U
#define ADC_COMPHYSTCTL_COMP3DISABLE        0x0800U

/**************************************************************************************************
 * 
 * The following are defines for ADC revision register (Sec. 8.12.8)
 * BASE REGISTER    ADCA_BASE
 * 
 *************************************************************************************************/

#define ADC_O_REV           0x4FU       /* Revision Register                                    */

#define ADC_S_REV           1

#define ADC_REV_TYPE_S      0U
#define ADC_REV_TYPE_M      0x00FFU
#define ADC_REV_REV_S       8U
#define ADC_REV_REV_M       0xFF00U

/**************************************************************************************************
 * 
 * The following are defines for ADC result registers (Sec. 8.12.9)
 * BASE REGISTER    ADCARESULT_BASE
 * 
 *************************************************************************************************/

#define ADC_O_RESULTx       0x0U        /* ADC Result X Register                                */

#define ADC_S_RESULTx       1

#define ADC_RESULTx_RESULT_S    0U
#define ADC_RESULTx_RESULT_M    0x0FFFU

#endif /* HW_ADC_H */
