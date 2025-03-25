/**************************************************************************************************
 * 
 * \file hw_cputimer.h
 * 
 * \brief Definitions for CPU timer registers
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef HW_CPUTIMER_H
#define HW_CPUTIMER_H

/**************************************************************************************************
 * 
 * The following are defines for CPU timer registers (Sec. 1.3.5).
 * BASE REGISTER    CPUTIMER0_BASE,CPUTIMER1_BASE,CPUTIMER2_BASE
 * 
 *************************************************************************************************/

#define CPUTIMER_O_TIM      0x0U        /* Counter Register                                     */
#define CPUTIMER_O_PRD      0x2U        /* Period Register                                      */
#define CPUTIMER_O_TCR      0x4U        /* Control Register                                     */
#define CPUTIMER_O_TPR      0x6U        /* Prescale Register                                    */
#define CPUTIMER_O_TPRH     0x7U        /* Prescale Register High                               */

#define CPUTIMER_S_TIM      2
#define CPUTIMER_S_PRD      2
#define CPUTIMER_S_TCR      1
#define CPUTIMER_S_TPR      1
#define CPUTIMER_S_TPRH     1

#define CPUTIMER_TIM_TIM_S          0U
#define CPUTIMER_TIM_TIM_M          0xFFFFFFFFUL

#define CPUTIMER_PRD_PRD_S          0U
#define CPUTIMER_PRD_PRD_M          0xFFFFFFFFUL

#define CPUTIMER_TCR_TSS            0x0010U
#define CPUTIMER_TCR_TRB            0x0020U
#define CPUTIMER_TCR_FREESOFT_S     10U
#define CPUTIMER_TCR_FREESOFT_M     0x0C00U
#define CPUTIMER_TCR_TIE            0x4000U
#define CPUTIMER_TCR_TIF            0x8000U

#define CPUTIMER_TPR_TDDR_S         0U
#define CPUTIMER_TPR_TDDR_M         0x00FFU
#define CPUTIMER_TPR_PSC_S          8U
#define CPUTIMER_TPR_PSC_M          0xFF00U

#define CPUTIMER_TPRH_TDDRH_S       0U
#define CPUTIMER_TPRH_TDDRH_M       0x00FFU
#define CPUTIMER_TPRH_PSCH_S        8U
#define CPUTIMER_TPRH_PSCH_M        0xFF00U

#endif /* HW_CPUTIMER_H */
