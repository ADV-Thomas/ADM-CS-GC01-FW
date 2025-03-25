/**************************************************************************************************
 * 
 * \file wch.h
 * 
 * \brief Window Hardware Comparator driver interface
 * 
 * \author Jorge Sola
 * 
 *************************************************************************************************/

#ifndef _INC_DRV_WCH_H
#define _INC_DRV_WCH_H

#include "driverlib/cmpss.h"
#include "driverlib/xbar.h"

#include "cgen/drv/wch.h"
#include "inc/api/obj.h"

#include <stdint.h>

/**************************************************************************************************
 * 
 * Forward declarations
 * 
 *************************************************************************************************/

struct _wch;

/**************************************************************************************************
 * 
 * Private variables definition
 * 
 *************************************************************************************************/
OBJ_PRIV_STRUCT(_wch_priv,
    uint16_t init;                      /* Bit mask which indicates if ADC core has been setup  */
);

/**************************************************************************************************
 * 
 * WCH variable definition
 * 
 *************************************************************************************************/
struct _wch_var {
    bool enable;                        /* Enables WCH variable if true                         */
    uint16_t core;
    uint32_t base;
    uint16_t adc_core;
    uint16_t adc_channel;
    uint16_t xbar_trip_h;
    uint16_t xbar_trip_l;
    uint16_t pwm_sync;
};

/**************************************************************************************************
 * 
 * \brief Configures all WCH variables in the WCH object
 * 
 * \param wch WCH object handler
 * 
 * \return 0 if operations is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
wch_setup(const struct _wch *wch);

#endif /* _INC_DRV_ADC_H */
