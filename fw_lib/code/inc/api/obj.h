/**************************************************************************************************
 * 
 * \file obj.h
 * 
 * \brief Generic API object interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_API_OBJ_H
#define _INC_API_OBJ_H

#include "cgen/api/obj.h"

#include <stdint.h>
#include <stdbool.h>

/**************************************************************************************************
 * 
 * Generic API object identifiers
 * 
 *************************************************************************************************/
enum obj_id {
    OBJ_NONE = 0,               /* Default object identifier                                    */
    OBJ__PWM,                   /* PWM object (driver)                                          */
    OBJ__ADC,                   /* ADC object (driver)                                          */
    OBJ__WCH,                   /* Window comparator in hardware (driver)                       */
    OBJ__SDFM,
    OBJ__ECAP,
    OBJ__DMA,
    OBJ__CLA,
    OBJ__PGA,
    OBJ_DB,                     /* CAN database object                                          */
    OBJ_ADC,                    /* ADC generic object                                           */
    OBJ_RMS,                    /* RMS generic object                                           */
    OBJ_WCH,                    /* Window comparator in hardware generic object                 */
    OBJ_WCS,                    /* Window comparators in software                               */
    OBJ_TASK,                   /* Task scheduler object                                        */
    OBJ_FSI,                    /* FSI database object                                          */
};

/**************************************************************************************************
 * 
 * \brief Check if generic API object is valid
 * 
 * \param obj generic object handler
 * \param id expected object identifier
 * 
 * \details Object is valid if:
 * - object handler given by \e obj parameter is not \b NULL,
 * - private variables handler in object is not \b NULL,
 * - object identifier matches expected object identifier, and
 * - number of object variables is greater than zero.
 * 
 * \return \b true when generic API object is valid; \b false otherwise
 * 
 *************************************************************************************************/
static inline bool
obj_check(const void *obj, enum obj_id id)
{
    OBJ_PRIV_STRUCT(priv);
    OBJ_STRUCT(obj, priv);

    const struct priv *priv = ((const struct obj *) obj)->priv;

    return (obj && priv && (priv->id == id) && (priv->size > 0U));
}

#endif /* _INC_API_OBJ_H */
