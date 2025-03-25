/**************************************************************************************************
 * 
 * \file ghm.h
 * 
 * \brief Grid health monitor interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_LIB_GHM_H
#define _INC_LIB_GHM_H

#include <stdint.h>
#include <stdbool.h>

#include "IQmathLib.h"

/**************************************************************************************************
 * 
 * Forward declarations
 * 
 *************************************************************************************************/

struct rms;
struct rms_var;

/**************************************************************************************************
 * 
 * Monitor status flags
 * 
 *************************************************************************************************/
struct ghm_sf {
    bool init;                      /* True when all status flags have been initialized         */
    bool rms;                       /* True when RMS voltage is within limits                   */
    bool freq;                      /* True when frequency is within limits                     */
    bool phase;                     /* True when phase shift is within limits                   */
};

/**************************************************************************************************
 * 
 * Monitored variable definition
 * 
 *************************************************************************************************/
struct ghm_var {
    const struct rms_var *rms;      /* RMS measurement variable                                 */
    struct ghm_sf sf;               /* Monitor status flags                                     */
    uint32_t negative;              /* Counts number of consecutive negative samples            */
    uint32_t time;                  /* Timestamp of the last detected zero-crossing             */
    uint32_t freq;                  /* Accumulator for frequency estimated via zero-crossing    */
    uint32_t phase;                 /* Accumulator for phase estimated via zero-crossing        */
    uint32_t counter;               /* Number of accumulated frequency and phase samples        */
};

/**************************************************************************************************
 * 
 * Private variables definition
 * 
 *************************************************************************************************/
struct ghm_priv {
    float TS;                       /* Latched sample time (s)                                  */
    _iq min;                        /* Minimum allowed RMS value                                */
    uint32_t timer;                 /* Free-running counter                                     */
    struct ghm_var var[3];          /* Monitored variables (one for each grid phase)            */
    uint16_t n_phases;
};

/**************************************************************************************************
 * 
 * User-defined parameters
 * 
 *************************************************************************************************/
struct ghm_usr {
    float TS;                       /* Sample time (s)                                          */
    const struct rms_var *rms[3];   /* RMS measurement variables (one for each grid phase)      */
    uint16_t n_phases;
};

/**************************************************************************************************
 * 
 * Output variables definition
 * 
 *************************************************************************************************/
struct ghm_out {
    struct ghm_sf sf;               /* Monitor status flags for all three phases combined       */
    float freq[3];                  /* Estimated frequency for each grid phase                  */
    float phase[3];                 /* Estimated phase for each grid phase                      */
};

/**************************************************************************************************
 * 
 * Grid health monitor object definition
 * 
 *************************************************************************************************/
struct ghm {
    struct ghm_priv priv;           /* Private variables                                        */
    struct ghm_usr usr;             /* User-defined parameters                                  */
    struct ghm_out out;             /* Output variables                                         */
};

/**************************************************************************************************
 * 
 * \brief Initializes grid health monitor
 * 
 * \param ghm monitor object handler
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
ghm_init(struct ghm *ghm);

/**************************************************************************************************
 * 
 * \brief Runs grid health monitor
 * 
 * \param ghm monitor object handler
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
ghm_run(struct ghm *ghm);

/**************************************************************************************************
 * 
 * \brief Creates new grid health monitor object
 * 
 * \param rms RMS object handler
 * 
 * \note This function is implemented in the application layer.
 * 
 * \return Monitor object handler
 * 
 *************************************************************************************************/
extern struct ghm *
ghm_new(const struct rms *rms);

#endif /* _INC_LIB_GHM_H */
