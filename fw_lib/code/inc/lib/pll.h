/**************************************************************************************************
 * 
 * \file pll.h
 * 
 * \brief Phase Locked Loop functions interface
 * 
 * \author Jorge Sola
 * 
 *************************************************************************************************/

#ifndef _LIB_FB_PLL_H
#define _LIB_FB_PLL_H

#include "inc/lib/ac.h"
#include "inc/lib/iir.h"
#include "inc/lib/pid.h"

#include <stdint.h>
#include <stdbool.h>


/**************************************************************************************************
 * 
 * Public enum definition
 * 
 *************************************************************************************************/
/* PLL routine type */
enum pll_type {
    PLL_TYPE_3P = 0,            /* Three-phase PLL                                              */
    PLL_TYPE_1P                 /* Single-phase PLL with SOGI-based OSG                         */
};

/* State machine state */
enum pll_state {
    PLL_STATE_NONE = 0,
    PLL_STATE_ERROR,            /* Error state                                                  */
    PLL_STATE_FREQUENCY,        /* Frequency initialization                                     */
    PLL_STATE_ROTATION,         /* Rotation direction initialization                            */
    PLL_STATE_OSG_WAIT,         /* Wait for OSG to settle                                       */
    PLL_STATE_AMPLITUDE,        /* Amplitude initialization                                     */
    PLL_STATE_INIT_3P,          /* Three-phase PLL initialization                               */
    PLL_STATE_INIT,             /* Three-phase and single-phase common PLL initialization       */
    PLL_STATE_DDSRF,            /* Decoupled double SRF PLL extension                           */
    PLL_STATE_RUN               /* PLL routine                                                  */
};

/* Error codes */
enum pll_error {
    PLL_ERROR_NONE = 0,
    PLL_ERROR_SETUP,            /* PLL object initialization                                    */
    PLL_ERROR_FREQUENCY,        /* Grid voltage frequency initialization                        */
    PLL_ERROR_OSG,              /* Orthogonal signal generator initialization                   */
    PLL_ERROR_ROTATION,         /* Voltage vector rotation direction                            */
    PLL_ERROR_AMPLITUDE,        /* Grid voltage amplitude initialization                        */
    PLL_ERROR_FNXX,             /* Resonant filter initialization                               */
    PLL_ERROR_UNEXPECTED_STATE  /* Unexpected state machine state                               */
};


/**************************************************************************************************
 * 
 *      PRIVATE STRUCTURE DEFINITION        [DO NOT ACCESS]
 *  
 *      NOTE:   Although these structure definitions are made publicly available,
 *              do not try to read or write them. They are private to the object!
 * 
 *************************************************************************************************/
/* Input variables */
struct pll_in {
    enum pll_type type;         /* Three-phase or single-phase PLL routine type                 */
    float ts;                   /* Sample time (s)                                              */
    float wn;                   /* Natural frequency of linearized PLL (rad/s)                  */
    float zeta;                 /* Damping of linearized PLL (-)                                */
    float q_lock;               /* Q component limit to detect in-lock condition (-)            */
    float t_lock;               /* Software timer constant for in-lock condition (s)            */
    bool ddsrf_en;              /* Decoupled double SRF PLL extension                           */
    bool fn6_en;                /* Q component  6th harmonic rejection                          */
    bool fn12_en;               /* Q component 12th harmonic rejection                          */
};

/* IIR filters */
struct pll_iir {
    struct iir fdp_d;       /* Filter for decoupled variable in DDSRF extension       (LP1) */
    struct iir fdp_q;       /* Filter for decoupled variable in DDSRF extension       (LP1) */
    struct iir fdn_d;       /* Filter for decoupled variable in DDSRF extension       (LP1) */
    struct iir fdn_q;       /* Filter for decoupled variable in DDSRF extension       (LP1) */
    struct iir fn6_q;       /* Filter for q component  6th harmonic                   (BS2) */
    struct iir fn12_q;      /* Filter for q component 12th harmonic                   (BS2) */
};

/* Private variables */
struct pll_priv {
    enum pll_state state;       /* PLL state machine state                                      */
    enum pll_error error;       /* PLL error code                                               */
    struct pll_in in;           /* Latched input parameters                                     */
    struct pll_iir iir;         /* IIR filters                                                  */
    struct pid pi_w_err;    /* PI controller for frequency estimation                       */
    struct ac_grid grid;        /* Grid phasor: magnitude, frequency (rad/s), and phase (rad)   */
    struct ac_freq freq;        /* Frequency initialization                                     */
    struct ac_rod rod;          /* Rotation direction detection                                 */
    struct ac_amp amp;          /* Amplitude initialization for normalization                   */
    struct ac_osg osg;          /* Orthogonal signal generator for single-phase PLL             */
    struct ac_ab0 v_grid_ab0;   /* Grid voltage in SRF                                          */
    struct ac_dq0 v_grid_dq0_p; /* Grid voltage in RRF with positive rotation                   */
    struct ac_dq0 v_grid_dq0_n; /* Grid voltage in RRF with negative rotation                   */
    float f_nom;                /* Nominal grid frequency - absolute (Hz)                       */
    float w_nom;                /* Nominal grid frequency (rad/s)                               */
    bool osg_en;                /* Enable flag for OSG                                          */
    uint64_t timer;             /* Software timer                                               */
    uint64_t t_osg;             /* Wait OSG to settle software timer constant (-)               */
    uint64_t t_lock;            /* In-lock debouncing software timer constant (-)               */
    bool in_lock;               /* In-lock condition flag                                       */
};

/**
 *      OBJECT DEFINITION
 */

/* Output variables */
struct pll_out {
    const enum pll_state *state;        /* PLL state machine state                              */
    const enum pll_error *error;        /* PLL error code                                       */
    const struct ac_grid *grid;         /* Grid phasor: frequency (rad/s) and phase (rad)       */
    const struct ac_ab0 *v_grid_ab0;    /* Grid voltage in SRF                                  */
    const struct ac_dq0 *v_grid_dq0_p;  /* Grid voltage in RRF with positive rotation           */
    const struct ac_dq0 *v_grid_dq0_n;  /* Grid voltage in RRF with negative rotation           */
    const bool *in_lock;                /* In-lock condition flag                               */
};

/* Object definition */
struct pll {
    struct pll_priv priv;       /* Private variables                            [do not access] */
    struct pll_in in;           /* Input variables                                              */
    struct pll_out out;         /* Output variables                                 [read-only] */
};

/**
 *      PUBLIC FUNCTION PROTOTYPE
 */

int pll_setup(struct pll *self);
void pll_reset(struct pll *self);
void pll_run(struct pll *self, const struct ac_abcn *v_grid_abcn);

#endif /* _LIB_FB_PLL_H */
