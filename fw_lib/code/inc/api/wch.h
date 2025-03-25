/**************************************************************************************************
 * 
 * \file wch.h
 * 
 * \brief WCH abstraction interface
 * 
 * \author Jorge Sola
 * 
 *************************************************************************************************/

#ifndef _INC_API_WCH_H
#define _INC_API_WCH_H

#include "cgen/api/wch.h"
#include "inc/lib/conv.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "IQmathLib.h"

/**************************************************************************************************
 * 
 * Forward declarations
 * 
 *************************************************************************************************/

struct wch;
struct iir;
struct nfo;
struct mal;

/**************************************************************************************************
 * 
 * Values that can be passed to \e wch_run() as the \e op parameter.
 * 
 *************************************************************************************************/
enum wch_op {
    WCH_OP_UPDATE,                  /* Update raw measurements and do normalization             */
    WCH_OP_RESET,               /* Normalize raw measurements                               */
    WCH_OP_CLEAR,                  /* Filter normalized measurements                           */
    WCH_OP_TEST,                 /* Calculate physical from filtered measurements            */
    WCH_OP_STATUS,
};

/* Comparator type */
enum wch_type {
    WCH_TYPE_LOW = 0,           /* Low comparator                                               */
    WCH_TYPE_HIGH,              /* High comparator                                              */
};

/* Calibration state */
enum wch_state {
    WCH_STATE_START = 0,        /* Initialize calibration routine                               */
    WCH_STATE_ERROR,            /* Error state                                                  */
    WCH_STATE_CALIBRATE,        /* Calibrate all low and high comparators                       */
};

/* Return codes */
enum wch_ret {
    WCH_RET_CAL_CONT = 1,       /* Calibration point initialized, comparators not calibrated    */
    WCH_RET_SUCCESS = 0,        /* Operation successful                                         */
    WCH_RET_NULL = -1,          /* Null pointer exception                                       */
    WCH_RET_TYPE = -2,          /* Invalid comparator type                                      */
    WCH_RET_INIT_SIZE = -3,     /* Number of variables not initialized in private structure     */
    WCH_RET_INIT_DATA = -4,     /* Conversion data update failed                                */
    WCH_RET_TEST_FAIL = -5,     /* Test failed for low or high comparators                      */
    WCH_RET_CAL_POINT = -6,     /* Calibration point #2 cannot be used in ONE_POINT mode        */
    WCH_RET_CAL_LOCK = -7,      /* Both calibration points have been locked                     */
    WCH_RET_CAL_CONV = -8,      /* Calibration failed in conversion object                      */
    WCH_RET_CAL_STATE = -9,     /* Unexpected state machine state                               */
    WCH_RET_CAL_ERROR = -10,    /* State machine has stuck in the error state                   */
};

/**************************************************************************************************
 * 
 * Private variables definition
 * 
 *************************************************************************************************/
OBJ_PRIV_STRUCT(wch_priv,
    bool init;                      /* Indicates if WCH object has been initialized             */
    enum wch_state state;
);

/**************************************************************************************************
 * 
 * WCH measurement variable
 * 
 *************************************************************************************************/
struct wch_meas {
    uint32_t raw;                   /* Raw (unprocessed) value                                  */
    _iq norm;                       /* Normalized value                                         */
    _iq flt;                        /* Filtered value                                           */
    float phy;                      /* Physical value (filtered)                                */
};


/**************************************************************************************************
 * 
 * WCH trip value test definition
 * 
 *************************************************************************************************/
struct wch_test {
    bool enable;                /* Comparator trip value test enable flag                       */
    uint16_t raw;               /* Raw value                                                    */
    float nrm;                  /* Normalized value                                             */
    float phy;                  /* Physical value                                               */
};

/**************************************************************************************************
 * 
 * WCH reference definition
 * 
 *************************************************************************************************/
struct wch_ref {
    uint16_t raw;               /* Raw value                                                    */
    float nrm;                  /* Normalized value                                             */
    float phy;                  /* Physical value                                               */
};

/**************************************************************************************************
 * 
 * WCH comparator definition
 * 
 *************************************************************************************************/
struct wch_cmp {
    enum wch_type type;         /* Comparator type (low or high)                                */
    bool enable;                /* Comparator enable flag                                       */
    bool fault;                 /* Comparator fault flag                                        */
    struct wch_ref ref;         /* Comparator reference                                         */
    struct wch_test test;       /* Comparator trip value test                                   */
    struct conv *conv;           /* Measurements conversion object                               */
};

/**************************************************************************************************
 * 
 * WCH variable definition
 * 
 *************************************************************************************************/
struct wch_var {
    bool fault;                 /* Combined low and high comparator fault flag                  */
    struct wch_cmp cmpl;        /* Low comparator object                                        */
    struct wch_cmp cmph;        /* High comparator object                                       */
};

/**************************************************************************************************
 * 
 * \brief Initializes WCH object
 * 
 * \param wch WCH object handler
 * \param mod module data object handler
 * \param mal memory handler
 * 
 * \details This function initializes WCH object. If low-pass filter is provided, it also setups
 * the filter as second-order low-pass Bessel with the following parameters:
 * - 1 Hz cut-off frequency,
 * - 1 ms sample time.
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
wch_init(const struct wch *wch, const struct nfo *mod, struct mal *mal);

/**************************************************************************************************
 * 
 * \brief Runs WCH test
 * 
 * \param wch WCH object handler
 * 
 * \return 0 if test succeeded. -1 otherwise
 * 
 *************************************************************************************************/
extern int
wch_test(const struct wch *wch);


/**************************************************************************************************
 * 
 * \brief Runs WCH operation
 * 
 * \param wch WCH object handler
 * \param op run operation
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
wch_run(const struct wch *wch, enum wch_op op);

/**************************************************************************************************
 * 
 * \brief Runs WCH calibration
 * 
 * \param wch WCH object handler
 * 
 * \return 0 if calibration succeeded. -1 otherwise
 * 
 *************************************************************************************************/
extern int
wch_calibrate(const struct wch *wch);

/**************************************************************************************************
 * 
 * \brief Creates new WCH object
 * 
 * \param mod module data object handler
 * \param mal memory handler
 * 
 * \note This function is implemented in the application layer.
 * 
 * \return WCH object handler
 * 
 *************************************************************************************************/
extern const struct wch *
wch_new(const struct nfo *mod, struct mal *mal);

/**************************************************************************************************
 * 
 * \brief Initializes WCH conversion data
 * 
 * \param wch WCH object handler
 * \param src conversion data source
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
wch_conv(const struct wch *wch, enum conv_src src);

/**************************************************************************************************
 * 
 * \brief Updates the WCH reference for the given variable
 * 
 * \param wch_var Pointer to WCH variable to be updated
 * \param upper Upper limit of the hardware comparator
 * \param lower lower limit of the hardware comparator
 * 
 *************************************************************************************************/
extern int
wch_set_reference(struct wch_var *wch_var, float upper, float lower);

#endif /* _INC_API_WCH_H */
