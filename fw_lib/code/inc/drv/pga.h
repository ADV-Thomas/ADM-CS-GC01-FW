/**************************************************************************************************
 *
 * \file pga.h
 *
 * \brief PGA (programable gain amplifier) driver interface
 *
 * \author Jorge Sola
 *
 *************************************************************************************************/

#ifndef _INC_DRV_PGA_H
#define _INC_DRV_PGA_H

#include "driverlib/pga.h"
#include "driverlib/adc.h"

#include "cgen/drv/pga.h"
#include "inc/api/obj.h"
#include "inc/drv/io.h"

#include <stdint.h>

/**************************************************************************************************
 *
 * Forward declarations
 *
 *************************************************************************************************/

struct _pga;

/**************************************************************************************************
 *
 * Private variables definition
 *
 *************************************************************************************************/
OBJ_PRIV_STRUCT(_pga_priv,
                uint16_t init; /* Bit mask which indicates if PGA core has been setup  */
);

/**************************************************************************************************
 *
 * PGA variable definition
 *
 *************************************************************************************************/
struct _pga_var {
    bool enable;      /* Enables PGA variable if true                         */
    PGA_GainValue gain;
    uint16_t core;
    uint16_t base;
};

/**************************************************************************************************
 *
 * \brief Configures all PGA variables in the PGA object
 *
 * \param pga PGA object handler
 *
 * \return 0 if operations is successful; -1 otherwise
 *
 *************************************************************************************************/
extern int pga_setup(const struct _pga *pga);

#endif /* _INC_DRV_ADC_H */