/**************************************************************************************************
 * 
 * \file user.h
 * 
 * \brief Application layer constants
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _APP_USER_H
#define _APP_USER_H

#define C_FS                (25000U)        /* Sample frequency (Hz)                            */
#define C_ISR_DIVIDER       (10U)
#define C_ISR_FREQ          (C_FS/C_ISR_DIVIDER)        /* Sample frequency (Hz)            */                

/**************************************************************************************************
 * 
 * Task frequency constants
 * 
 *************************************************************************************************/
#define C_TASK_FREQ_BLINK   ((float) 2.0f)      /* Blink user LED task frequency (Hz)           */
#define C_TASK_FREQ_MEAS    ((float) 1000.0f)    /* Measurements task frequency (Hz)             */
#define C_TASK_FREQ_CAN     ((float) 1000.0f)   /* CAN task frequency (Hz)                      */
#define C_TASK_FREQ_FB      ((float) 1000.0f)   /* Feedback task frequency (Hz)                 */
#define C_TASK_FREQ_CAL     ((float) 1000.0f)   /* ADC and WCH calibration task frequency (Hz)  */
#define C_TASK_FREQ_MLX_MEAS     ((float) 30.0f)   /* ADC and WCH calibration task frequency (Hz)  */
#define C_TASK_FREQ_MLX     ((float) 2.0f)   /* ADC and WCH calibration task frequency (Hz)  */


/**************************************************************************************************
 * 
 * Derived constants
 * 
 *************************************************************************************************/

#define C_TS                (1.0f / C_FS)   /* Sample time (s)                                  */


#endif /* _APP_USER_H */
