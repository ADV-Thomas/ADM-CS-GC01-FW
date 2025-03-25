/**************************************************************************************************
 * 
 * \file cla.h
 * 
 * \brief CLA driver interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_DRV_CLA_H
#define _INC_DRV_CLA_H

#include "driverlib/cla.h"

#include "cgen/drv/cla.h"
#include "inc/api/obj.h"

#include <stdint.h>

/**************************************************************************************************
 * 
 * Forward declarations
 * 
 *************************************************************************************************/

struct _cla;

/**************************************************************************************************
 * 
 * Private variables definition
 * 
 *************************************************************************************************/
OBJ_PRIV_STRUCT(_cla_priv,
    uint16_t init;                      /* Bit mask which indicates if ADC core has been setup  */
);

/**************************************************************************************************
 * 
 * ADC variable definition
 * 
 *************************************************************************************************/
struct _cla_var {
    bool enable;                        /* Enables ADC variable if true                         */
    uint16_t task_num;
    uint16_t trigger;
    void (*task_fun)(void);
};


extern int cla_setup(const struct _cla *cla);

#endif /* _INC_DRV_CLA_H */
