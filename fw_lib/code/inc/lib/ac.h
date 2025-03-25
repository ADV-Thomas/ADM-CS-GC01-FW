/**************************************************************************************************
 * 
 * \file ac.h
 * 
 * \brief  AC functions interface (park, clark, phase2line, line2phase, etc.)
 * 
 * \author Jorge Sola
 * 
 *************************************************************************************************/
#ifndef _LIB_FB_AC_H
#define _LIB_FB_AC_H

#include <stdint.h>
#include <stdbool.h>

/**************************************************************************************************
 * 
 * Public enum definition
 * 
 *************************************************************************************************/
/* Clarke transformation types */
enum ac_clarke {
    AC_CLARKE_AMPLITUDE_INVARIANT = 1,  /* Preserve amplitude between alpha-beta and abc frames */
    AC_CLARKE_POWER_INVARIANT = 2       /* Preserve power     between alpha-beta and abc frames */
};

/* Park transformation types */
enum ac_park {
    AC_PARK_POSITIVE_ROTATION = 1,      /* Positive rotation between alpha-beta and dq frames   */
    AC_PARK_NEGATIVE_ROTATION = 2       /* Negative rotation between alpha-beta and dq frames   */
};

/* Modulation types */
enum ac_mod {
    AC_MOD_NONE = 0,
    AC_MOD_SINUSOIDAL,                  /* Sinusoidal modulation                                */
    AC_MOD_SINUSOIDAL_3,                /* Sinusoidal modulation with third harmonic injection  */
    AC_MOD_SVM_SYMMETRICAL              /* Symmetrical space-vector modulation                  */
};

/* Voltage vector rotation direction */
enum ac_dir {
    AC_DIR_NONE = 0,
    AC_DIR_CCW = 1,                     /* CCW rotation direction, frequency is positive        */
    AC_DIR_CW = 2                       /*  CW rotation direction, frequency is negative        */
};

/**************************************************************************************************
 * 
 * Public structures definition
 * 
 *************************************************************************************************/
/* Phase variables */
struct ac_abcn {
    float an;                   /* Phase AN                                                     */
    float bn;                   /* Phase BN                                                     */
    float cn;                   /* Phase CN                                                     */
};

/* Line variables */
struct ac_abc {
    float ab;                   /* Line AB                                                      */
    float bc;                   /* Line BC                                                      */
    float ca;                   /* Line CA                                                      */
};

/* Stationary reference frame (SRF) */
struct ac_ab0 {
    enum ac_clarke type;        /* Clarke transformation type                                   */
    float alpha;                /* Alpha component                                              */
    float beta;                 /* Beta  component                                              */
    float zero;                 /* Zero  component                                              */
};

/* Rotating reference frame (RRF) */
struct ac_dq0 {
    enum ac_park type;          /* Park transformation type                                     */
    float d;                    /* d component                                                  */
    float q;                    /* q component                                                  */
    float o;                    /* 0 component                                                  */
};

/* Phasor */
struct ac_rot {
    float rms;                  /* Phasor magnitude (RMS value by convention)                   */
    float freq;                 /* Phasor frequency (rad/s)                                     */
    float phase;                /* Phasor phase (rad)                                           */
};

/* Trigonometric values */
struct ac_trig {
    float _sin;                 /* Sine of the grid phase angle                                 */
    float _cos;                 /* Cosine of the grid phase angle                               */
};

/* Grid variables */
struct ac_grid {
    float ts;                   /* Sample time to generate artifical rotation (s)               */
    struct ac_rot rot;          /* Grid phasor: magnitude, frequency (rad/s), and phase (rad)   */
    struct ac_trig trig;        /* Trigonometric values of the grid phase angle                 */
};

/* Frequency detection */
struct ac_freq {
    float ts;                   /* Sample time (s)                                         *in* */
    float t_max;                /* Timeout protection (s)                                  *in* */
    uint32_t n_max;             /* Number of frequency samples                             *in* */
    float f_min;                /* Minimum expected frequency (Hz)                         *in* */
    float f_max;                /* Maximum expected frequency (Hz)                         *in* */
    float f_est;                /* Estimated frequency (Hz)                                     */
    float f_sum;                /* Accumulator for frequency estimation (Hz)                    */
    float v1;                   /* AC voltage state v[k-1]                                      */
    uint32_t counter;           /* Number of collected frequency samples                        */
    uint64_t timer;             /* Software timer                                               */
    uint64_t timer_0;           /* Software timer value when last zero-crossing has occured     */
    bool timeout;               /* Timeout error flag                                           */
};

/* Rotation direction detection */
struct ac_rod {
    float ts;                   /* Sample time (s)                                         *in* */
    float t_max;                /* Timeout protection (s)                                  *in* */
    uint32_t n_max;             /* Number of consecutive rotation direction samples        *in* */
    enum ac_dir dir;            /* CCW or CW rotation direction                                 */
    float vd_min;               /* Direct component minimum value since last zero crossing      */
    float vd_max;               /* Direct component maximum value since last zero crossing      */
    float vd1;                  /* AC voltage state vd[k-1]                                     */
    int32_t counter;            /* Counter for rotation direction samples                       */
    uint64_t timer;             /* Software timer                                               */
    bool timeout;               /* Timeout error flag                                           */
};

/* Amplitude initialization */
struct ac_amp {
    uint32_t n_max;             /* Number of amplitude samples                             *in* */
    uint32_t counter;           /* Samples counter                                              */
    float a_est;                /* Estimated amplitude                                          */
    float sum;                  /* Accumulator for amplitude estimation                         */
};

/* Orthogonal signal generator */
struct ac_osg {
    float ts;                   /* Sample time (s)                                         *in* */
    float fb;                   /* 3-dB frequency bandwidth (Hz)                           *in* */
    float fc;                   /* Fundamental frequency (Hz)                              *in* */
    float U01;                  /* Recursive equation parameter                                 */
    float U21;                  /* Recursive equation parameter                                 */
    float UY1;                  /* Recursive equation parameter                                 */
    float Y12;                  /* Recursive equation parameter                                 */
    float Y1;                   /* Recursive equation parameter                                 */
    float Q01;                  /* Recursive equation parameter                                 */
    float u1;                   /* Input signal u[k-1]                                          */
    float u2;                   /* Input signal u[k-2]                                          */
    float d1;                   /* Direct component d[k-1]                                      */
    float q1;                   /* Quadrature component q[k-1]                                  */
    float q2;                   /* Quadrature component q[k-2]                                  */
    bool init;                  /* Indicates if OSG has been initialized                        */
};

/**
 *      PUBLIC FUNCTION PROTOTYPE
 */

void ac_phase_to_line(struct ac_abc *abc, const struct ac_abcn *abcn);
void ac_line_to_phase(struct ac_abcn *abcn, const struct ac_abc *abc);

void ac_fwd_clarke(struct ac_ab0 *ab0, const struct ac_abcn *abcn);
void ac_inv_clarke(struct ac_abcn *abcn, const struct ac_ab0 *ab0);

void ac_fwd_park(struct ac_dq0 *dq0, const struct ac_ab0 *ab0, const struct ac_trig *trig);
void ac_inv_park(struct ac_ab0 *ab0, const struct ac_dq0 *dq0, const struct ac_trig *trig);

void ac_modulation(struct ac_abcn *m_abcn, const struct ac_dq0 *v_ref_dq0, float v_dc, const struct ac_trig *trig, enum ac_mod type);

void ac_grid_init(struct ac_grid *self, float amp, float freq, const struct ac_ab0 *ab0);
void ac_grid_reset(struct ac_grid *self);
void ac_grid_rotation(struct ac_grid *self);
void ac_grid_trigonometry(struct ac_grid *self);

void ac_freq_reset(struct ac_freq *self);
int ac_freq_run(struct ac_freq *self, float v);

void ac_rod_reset(struct ac_rod *self);
int ac_rod_run(struct ac_rod *self, const struct ac_ab0 *ab0);

void ac_amp_reset(struct ac_amp *self);
int ac_amp_run(struct ac_amp *self, const struct ac_ab0 *ab0);

int ac_osg_init(struct ac_osg *self);
void ac_osg_reset(struct ac_osg *self);
void ac_osg_run(struct ac_osg *self, float u0, struct ac_ab0 *ab0);

#endif /* _LIB_FB_AC_H */

