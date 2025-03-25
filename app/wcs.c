/**************************************************************************************************
 * 
 * \file wcs.h
 * 
 * \brief WCS implementation
 * 
 * \author Jorge Sola
 * 
 *************************************************************************************************/

#include "app/wcs.h"
#include "app/adc.h"

#include "inc/lib/nfo.h"
#include "inc/lib/ntc.h"

#include <stddef.h>

/**************************************************************************************************
 * 
 * wcs_new()
 * 
 *************************************************************************************************/
const struct wcs *
wcs_new(const struct adc *adc, const struct nfo *mod)
{
    ASSERT(mod);

    //int ret;

    /**
     * Create object variables
     *  . Minimum voltage and temperature protection is disabled by default. These signals can be
     *    manually enabled by the user via CAN.
     *  . NTC thermistor is connected to the MCU analog input via a 10k pull-up resistor. NTC has
     *    negative temperature coefficient, ie increase in temperature translates to decrease in
     *    resistance, ie to decrease in voltage on the MCU analog input. For this reason, low and
     *    high comparators output for temperature measurements must be inverted.
     */ 

    /*      --- VARIABLES---    ---- CMPL ----  ---- CMPH ---   */    
    /*                          ENABLE  INVERT  ENABLE  INVERT  */  
   /* WCS_VAR_NEW(button0,   false,  false,  true,   false);
    WCS_VAR_NEW(button1,   false,  false,  true,   false);
    WCS_VAR_NEW(button2,   false,  false,  true,   false);



    switch (mod->revision) {
    case 0U:
        break;
    case 1U:
        break;
    case 2U:
        break;
    default:
        return NULL;
    }


    WCS_OBJ_NEW(
        OBJ_MEMBER_SET(fan1_voltage),
        OBJ_MEMBER_SET(fan2_voltage),
        OBJ_MEMBER_SET(fan3_voltage),

    );

    ret = wcs_init(&wcs);
    if (ret < 0) {
        return NULL;
    }
    
    return &wcs
    */
    return NULL; 
}
