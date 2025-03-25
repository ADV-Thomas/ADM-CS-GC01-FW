/**************************************************************************************************
 * 
 * \file wcs.h
 * 
 * \brief Window comparators in software interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_API_WCS_H
#define _INC_API_WCS_H

#include "cgen/api/wcs.h"
#include "inc/api/obj.h"
#include "inc/lib/conv.h"

#include <stdint.h>
#include <stdbool.h>

#include "IQmathLib.h"

/**************************************************************************************************
 * 
 * Forward declarations
 * 
 *************************************************************************************************/

struct wcs;
struct conv;
struct nfo;
struct adc;

/**************************************************************************************************
 * 
 * Values that can be passed to \e wcs_run() as the \e op parameter.
 * 
 *************************************************************************************************/
enum wcs_op {
    WCS_OP_UPDATE = 0,              /* Update comparator normalized ref. from physical value    */
    WCS_OP_RESET,                   /* Reset and disable user-defined comparator reference      */
    WCS_OP_CLEAR,                   /* Clear all fault flags                                    */
    WCS_OP_STATUS                   /* Update fault flag for all comparators                    */
};

/**************************************************************************************************
 * 
 * Private variables definition
 * 
 *************************************************************************************************/
OBJ_PRIV_STRUCT(wcs_priv,
    bool init;                      /* Indicates if WCS object has been initialized             */
);

/**************************************************************************************************
 * 
 * Comparator reference
 * 
 *************************************************************************************************/
struct wcs_ref {
    _iq norm;                       /* Normalized reference value                               */
    float phy;                      /* Physical reference value                                 */
};

/**************************************************************************************************
 * 
 * Comparator object
 * 
 *************************************************************************************************/
struct wcs_cmp {
    bool enable;                    /* Enable or disable comparator                             */
    bool invert;                    /* Invert comparator output                                 */
    bool fault;                     /* Comparator output (fault flag)                           */
    struct wcs_ref sys;             /* System-defined comparator reference                      */
    struct wcs_ref usr;             /* User-defined comparator reference                        */
    const struct wcs_ref *ref;      /* Active comparator reference (system- or user-defined)    */
};

/**************************************************************************************************
 * 
 * WCS variable definition
 * 
 *************************************************************************************************/
struct wcs_var {
    const _iq *meas;                /* Normalized measurement used as input to comparators      */
    const struct conv * conv;        /* Measurement conversion object                            */
    struct wcs_cmp cmpl;            /* Low-side comparator object                               */
    struct wcs_cmp cmph;            /* High-side comparator object                              */
    bool fault;                     /* Combined low- and high-side comparator output (fault)    */
};


/**************************************************************************************************
 * 
 * \brief Initializes WCS object
 * 
 * \param wcs WCS object handler
 * 
 * \note This function is implemented in the application layer.
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
wcs_init(const struct wcs *wcs);

/**************************************************************************************************
 * 
 * \brief Runs WCS operation
 * 
 * \param wcs WCS object handler
 * \param op run operation
 * 
 * \return None
 * 
 *************************************************************************************************/
extern bool
wcs_run(const struct wcs *wcs, enum wcs_op op);

#endif /* _INC_API_WCS_H */
