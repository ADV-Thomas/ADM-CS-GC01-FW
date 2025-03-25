/**************************************************************************************************
 * 
 * \file adc.h
 * 
 * \brief ADC interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _APP_ADC_H
#define _APP_ADC_H

#include "inc/api/adc.h"

/**************************************************************************************************
 * 
 * ADC object definition
 * 
 *************************************************************************************************/
ADC_OBJ_STRUCT(
    ADC_OBJ_STRUCT_MEMBER(button0);        /* Output-side port L2 voltage                      */
    ADC_OBJ_STRUCT_MEMBER(button1);        /* Output-side port L3 voltage                      */
    ADC_OBJ_STRUCT_MEMBER(button2);        /* Output-side port L3 voltage                      */
    ADC_OBJ_STRUCT_MEMBER(button3);           /* Inductor temperature                     *rev.0* */
    ADC_OBJ_STRUCT_MEMBER(button_cw);           /* Inductor temperature                     *rev.0* */
    ADC_OBJ_STRUCT_MEMBER(coding_a);
    ADC_OBJ_STRUCT_MEMBER(coding_b);

);

#endif /* _APP_ADC_H */
