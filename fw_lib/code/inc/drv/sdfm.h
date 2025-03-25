/**************************************************************************************************
 *
 * \file sdfm.h
 *
 * \brief Sigma Delta Modulator driver interface
 *
 * \author Jorge Sola
 *
 *************************************************************************************************/

#ifndef _INC_DRV_SDFM_H
#define _INC_DRV_SDFM_H

#include "driverlib/sdfm.h"

#include "cgen/drv/sdfm.h"
#include "inc/api/obj.h"
#include "inc/drv/io.h"

#include <stdint.h>

/**************************************************************************************************
 *
 * Forward declarations
 *
 *************************************************************************************************/

struct _sdfm;

/**************************************************************************************************
 *
 * Private variables definition
 *
 *************************************************************************************************/
OBJ_PRIV_STRUCT(_sdfm_priv,
                uint16_t init; /* Bit mask which indicates if SDFM core has been setup  */
);

/**************************************************************************************************
 *
 * SDFM variable definition
 *
 *************************************************************************************************/
struct _sdfm_var {
    bool enable;      /* Enables SDFM variable if true                         */
    uint16_t channel; /* SDFM filter number (1..8)                         */
    enum io io;
    enum io io_clk;
    uint16_t oversampling;
    uint16_t shift;
    uint16_t clk_mode;
};

/**************************************************************************************************
 *
 * \brief Configures all SDFM variables in the SDFM object
 *
 * \param sdfm SDFM object handler
 *
 * \return 0 if operations is successful; -1 otherwise
 *
 *************************************************************************************************/
extern int sdfm_setup(const struct _sdfm *sdfm);

#endif /* _INC_DRV_ADC_H */
