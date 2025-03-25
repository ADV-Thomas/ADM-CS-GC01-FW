/**************************************************************************************************
 * 
 * \file wcs.h
 * 
 * \brief Window comparators in software, applilcation interface
 * 
 * \author Jorge Sola
 * 
 *************************************************************************************************/

#ifndef _APP_WCS_H
#define _APP_WCS_H

#include "inc/api/wcs.h"

/**************************************************************************************************
 * 
 * WCS object definition
 * 
 *************************************************************************************************/

WCS_OBJ_STRUCT(
    WCS_OBJ_STRUCT_MEMBER(fan1_voltage);        /* Input-side port L1 voltage                       */
    WCS_OBJ_STRUCT_MEMBER(fan2_voltage);        /* Input-side port L2 voltage                       */
    WCS_OBJ_STRUCT_MEMBER(fan3_voltage);        /* Input-side port L3 voltage                       */
    WCS_OBJ_STRUCT_MEMBER(temp1);               /* Bar temperature                                  */
    WCS_OBJ_STRUCT_MEMBER(temp2);               /* Inductor temperature                             */
    WCS_OBJ_STRUCT_MEMBER(temp3);               /*                 */
    WCS_OBJ_STRUCT_MEMBER(fan1_current);        /* Input-side port L1 current                       */
    WCS_OBJ_STRUCT_MEMBER(fan2_current);        /* Input-side port L2 current                       */
    WCS_OBJ_STRUCT_MEMBER(fan3_current);        /* Input-side port L3 current                       */
    
    WCS_OBJ_STRUCT_MEMBER(analog_in1);          /* Analog input                      */
    WCS_OBJ_STRUCT_MEMBER(analog_in2);          /* Analog input                      */
    WCS_OBJ_STRUCT_MEMBER(analog_in3);          /* Analog input                      */
    WCS_OBJ_STRUCT_MEMBER(analog_in4);          /* Analog input                      */
);

const struct wcs *
wcs_new(const struct adc *adc, const struct nfo *mod);

#endif /* _APP_WCS_H */
