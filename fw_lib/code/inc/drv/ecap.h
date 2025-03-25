/**************************************************************************************************
 *
 * \file ecap.h
 *
 * \brief ECAP driver interface
 *
 * \author Jorge Sola
 *
 *************************************************************************************************/

#ifndef _INC_DRV_ECAP_H
#define _INC_DRV_ECAP_H

#include "driverlib/ecap.h"

#include "cgen/drv/ecap.h"
#include "inc/drv/io.h"
#include "inc/api/obj.h"

#include <stdint.h>

/**************************************************************************************************
 *
 * Forward declarations
 *
 *************************************************************************************************/

struct _ecap;

/**************************************************************************************************
 *
 * Private variables definition
 *
 *************************************************************************************************/
OBJ_PRIV_STRUCT(_ecap_priv,
                uint16_t init; /**< Bit mask which indicates if ECAP core has been setup  */
);

/**************************************************************************************************
 *
 * ECAP variable definition
 *
 *************************************************************************************************/
struct _ecap_var {
    bool enable;   /**< Enables ECAP variable if true                        */
    uint16_t core; /**< ECAP core index (1 is ECAP1, 2 is ECAP2 etc.)        */
    enum io io;    /**< Connected gpio */
    uint32_t base; /**< ECAP module base address                             */
    uint16_t div;  /**< Event divider */
};

/**************************************************************************************************
 *
 * \brief Configures all ECAP variables in the ECAP object
 *
 * \param ecap ECAP object handler
 *
 * \return 0 if operations is successful; -1 otherwise
 *
 *************************************************************************************************/
extern int ecap_setup(const struct _ecap *ecap);

#endif /* _INC_DRV_ECAP_H */
