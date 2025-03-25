/**************************************************************************************************
 * 
 * \file ac.c
 * 
 * \brief AC functions interface (park, clark, phase2line, line2phase, etc.) 
 * 
 * \author Jorge Sola
 * 
 *************************************************************************************************/
#include "inc/lib/ac.h"

#include "inc/lib/utils.h"

#include <math.h>


/**************************************************************************************************
 * 
 * Three phase transformations
 * 
 *************************************************************************************************/
__attribute__((ramfunc))
void ac_phase_to_line(struct ac_abc *abc, const struct ac_abcn *abcn)
{
    abc->ab = abcn->an - abcn->bn;
    abc->bc = abcn->bn - abcn->cn;
    abc->ca = abcn->cn - abcn->an;
}

__attribute__((ramfunc))
void ac_line_to_phase(struct ac_abcn *abcn, const struct ac_abc *abc)
{
    abcn->an = C_1BY3 * (abc->ab - abc->ca);
    abcn->bn = C_1BY3 * (abc->bc - abc->ab);
    abcn->cn = C_1BY3 * (abc->ca - abc->bc);
}

__attribute__((ramfunc))
void ac_fwd_clarke(struct ac_ab0 *ab0, const struct ac_abcn *abcn)
{
    switch (ab0->type) {
    case AC_CLARKE_AMPLITUDE_INVARIANT:
        /* Intentional fall through */
    default:
        ab0->alpha = (C_2BY3 * abcn->an) - (C_1BY3 * (abcn->bn + abcn->cn));
        ab0->beta  = C_SQRT3BY3 * (abcn->bn - abcn->cn);
        ab0->zero  = C_1BY3 * (abcn->an + abcn->bn + abcn->cn);
        break;
    case AC_CLARKE_POWER_INVARIANT:
        ab0->alpha = (C_SQRT2BYSQRT3 * abcn->an) - (C_SQRT6BY6 * (abcn->bn + abcn->cn));
        ab0->beta  = C_SQRT2BY2 * (abcn->bn - abcn->cn);
        ab0->zero  = C_SQRT3BY3 * (abcn->an + abcn->bn + abcn->cn);
        break;
    }
}

__attribute__((ramfunc))
void ac_inv_clarke(struct ac_abcn *abcn, const struct ac_ab0 *ab0)
{
    switch (ab0->type) {
    case AC_CLARKE_AMPLITUDE_INVARIANT:
        /* Intentional fall through */
    default:
        abcn->an = ab0->alpha + ab0->zero;
        abcn->bn = -(C_1BY2 * ab0->alpha) + (C_SQRT3BY2 * ab0->beta) + ab0->zero;
        abcn->cn = -(C_1BY2 * ab0->alpha) - (C_SQRT3BY2 * ab0->beta) + ab0->zero;
        break;
    case AC_CLARKE_POWER_INVARIANT:
        abcn->an = (C_SQRT2BYSQRT3 * ab0->alpha) + (C_SQRT3BY3 * ab0->zero);
        abcn->bn = -(C_SQRT6BY6 * ab0->alpha) + (C_SQRT2BY2 * ab0->beta) + (C_SQRT3BY3 * ab0->zero);
        abcn->cn = -(C_SQRT6BY6 * ab0->alpha) - (C_SQRT2BY2 * ab0->beta) + (C_SQRT3BY3 * ab0->zero);
        break;
    }
}

__attribute__((ramfunc))
void ac_fwd_park(struct ac_dq0 *dq0, const struct ac_ab0 *ab0, const struct ac_trig *trig)
{
    switch (dq0->type) {
    case AC_PARK_POSITIVE_ROTATION:
        /* Intentional fall through */
    default:
        dq0->d = +(ab0->alpha * trig->_cos) + (ab0->beta * trig->_sin);
        dq0->q = -(ab0->alpha * trig->_sin) + (ab0->beta * trig->_cos);
        dq0->o = ab0->zero;
        break;
    case AC_PARK_NEGATIVE_ROTATION:
        dq0->d = +(ab0->alpha * trig->_cos) - (ab0->beta * trig->_sin);
        dq0->q = +(ab0->alpha * trig->_sin) + (ab0->beta * trig->_cos);
        dq0->o = ab0->zero;
        break;
    }
}

__attribute__((ramfunc))
void ac_inv_park(struct ac_ab0 *ab0, const struct ac_dq0 *dq0, const struct ac_trig *trig)
{
    switch (dq0->type) {
    case AC_PARK_POSITIVE_ROTATION:
        /* Intentional fall through */
    default:
        ab0->alpha = +(dq0->d * trig->_cos) - (dq0->q * trig->_sin);
        ab0->beta  = +(dq0->d * trig->_sin) + (dq0->q * trig->_cos);
        ab0->zero  = dq0->o;
        break;
    case AC_PARK_NEGATIVE_ROTATION:
        ab0->alpha = +(dq0->d * trig->_cos) + (dq0->q * trig->_sin);
        ab0->beta  = -(dq0->d * trig->_sin) + (dq0->q * trig->_cos);
        ab0->zero  = dq0->o;
        break;
    }
}


/**************************************************************************************************
 * 
 * PWM modulation
 * 
 *************************************************************************************************/
__attribute__((ramfunc))
void ac_modulation(struct ac_abcn *m_abcn, const struct ac_dq0 *v_ref_dq0, float v_dc, const struct ac_trig *trig, enum ac_mod type)
{
    /**
     *  Limit inverter voltage amplitude
     *  NOTE: Requested inverter voltage might be higher than what DC link voltage can provide.
     *  Inverter voltage amplitude is limited here not to have duty cycles outside of 0..1 range.
     *  TODO: Implement and verify!
     */

    /**
     *  Calculate duty cycles in dq frame
     *  NOTE: Duty cycles calculated in this way will be in range -0.5..0.5.
     *  TODO: Should we add protection against zero DC link voltage?
     */
    float v_dc_inv = 1.0f / v_dc;
    struct ac_dq0 m_dq0 = (struct ac_dq0) {
        .type = AC_PARK_POSITIVE_ROTATION,
        .d = v_ref_dq0->d * v_dc_inv,
        .q = v_ref_dq0->q * v_dc_inv,
        .o = 0.0f,
    };

    /**
     *  TODO: Grid voltage phase angle could be corrected in order to account for delay
     *  introduced by control algorithm and analog filters. Results will not be much
     *  improved for grid-connected applications, but it might be crucial in case of
     *  electric machines that operate at high frequencies.
     */

    /* Inverse Park transformation */
    struct ac_ab0 m_ab0 = (struct ac_ab0) {
        .type = AC_CLARKE_AMPLITUDE_INVARIANT,
    };
    ac_inv_park(&m_ab0, &m_dq0, trig);

    /* Inverse Clarke transformation */
    ac_inv_clarke(m_abcn, &m_ab0);

    /* Temporary variables */
    float m_med, m_zsv;

    /* Modulation type */
    switch (type) {
        case AC_MOD_SINUSOIDAL:
            /* Intentional fall through */
        case AC_MOD_SINUSOIDAL_3:
            break;
        
        case AC_MOD_SVM_SYMMETRICAL:
            /**
             *  Calculate zero-voltage sequence
             *  NOTE: Some research papers suggest calculating it as -0.5f*(m_max+m_min), where
             *  m_max and m_min are maximum and minimum duty cylces in the m_abcn structure.
             *  Median value is used instead as it requires less operations, 2 or 3 comparisons!
             */
            if (m_abcn->an > m_abcn->bn)
                if (m_abcn->bn > m_abcn->cn)            /* c < b < a */
                    m_med = m_abcn->bn;
                else if (m_abcn->an > m_abcn->cn)       /* b < c < a */
                    m_med = m_abcn->cn;
                else                                    /* b < a < c */
                    m_med = m_abcn->an;
            else
                if (m_abcn->an > m_abcn->cn)            /* c < a < b */
                    m_med = m_abcn->an;
                else if (m_abcn->bn > m_abcn->cn)       /* a < c < b */
                    m_med = m_abcn->cn;
                else                                    /* a < b < c */
                    m_med = m_abcn->bn;

            /* Zero-voltage sequence */
            m_zsv = 0.5f * m_med;

            /* Add zero-voltage sequence to duty cycles */
            m_abcn->an += m_zsv;
            m_abcn->bn += m_zsv;
            m_abcn->cn += m_zsv;

            break;

        case AC_MOD_NONE:
            /* Intentional fall through */
        default:
            break;
    }

    /* Transform range from -0.5..0.5 to 0..1 */
    m_abcn->an += 0.5f;
    m_abcn->bn += 0.5f;
    m_abcn->cn += 0.5f;
}


/**************************************************************************************************
 * 
 * AC grid phasor
 * 
 *************************************************************************************************/
__attribute__((ramfunc))
void ac_grid_init(struct ac_grid *self, float amp, float freq, const struct ac_ab0 *ab0)
{
    /* Calculate phase */
    float phase = atan2f(ab0->beta, ab0->alpha);

    /* Limit phase to 0..2PI interval */
    if (phase < 0)
        phase += C_2PI;
    
    /* Initialize phasor */
    self->rot = (struct ac_rot) {
        .rms = amp / C_SQRT2,
        .freq = freq,
        .phase = phase
    };
}

void ac_grid_reset(struct ac_grid *self)
{
    /* Grid phasor */
    self->rot = (struct ac_rot) {
        .rms = 0.0f,
        .freq = 0.0f,
        .phase = 0.0f
    };

    /* Trigonometric values */
    self->trig = (struct ac_trig) {
        ._sin = 0.0f,
        ._cos = 0.0f
    };
}

__attribute__((ramfunc))
void ac_grid_rotation(struct ac_grid *self)
{
    /* Pointers for shorter notation */
    struct ac_rot *rot = &self->rot;

    /* Update phase angle by rectangular numerical integration method (rad) */
    rot->phase += (rot->freq * self->ts);

    /**
     * Limit phase angle to 0..2PI interval
     *  . What if frequency is that large to cause more than 2.PI jump within 1 sample time?
     *    In that case the frequency is greater than 1/TS which implies aliasing phenomena.
     *    This is not expected to happen with 50 Hz or 60 Hz electric grids.
     */
    if (rot->phase > C_2PI)
        rot->phase -= C_2PI;
    else if (rot->phase < 0.0f)
        rot->phase += C_2PI;
}

__attribute__((ramfunc))
void ac_grid_trigonometry(struct ac_grid *self)
{
    /* Pointers for shorter notation */
    struct ac_rot *rot = &self->rot;
    struct ac_trig *trig = &self->trig;

    /* Calculate sine and cosine of the grid phase */
    trig->_sin = sinf(rot->phase);
    trig->_cos = cosf(rot->phase);
}


/**************************************************************************************************
 * 
 * Frequency initialization
 * 
 *************************************************************************************************/
void ac_freq_reset(struct ac_freq *self)
{
    self->f_est = 0.0f;
    self->f_sum = 0.0f;
    self->v1 = 0.0f;
    self->counter = 0u;
    self->timer = 0u;
    self->timer_0 = 0u;
    self->timeout = false;
}

int ac_freq_run(struct ac_freq *self, float v)
{
    /* Error check */
    if ((self->timeout == true) || (self->n_max == 0u))
        return -1;
    
    /* Check if frequency has been initialized */
    if (self->counter >= self->n_max)
        return 1;

    /**
     * Zero crossing detection
     *  . "timer > 0" condition ensures that v1 is initialized before zero crossing detection,
     *    which is necessary condition.
     */
    if ((self->timer > 0u) && ((v * self->v1) <= 0.0f)) {
        /* First detected zero crossing event is used only to initialize timer_0 */
        if (self->timer_0 > 0u) {
            /* Zero crossing occurs two times per cycle */
            float f_est = 0.5f / ((self->timer - self->timer_0) * self->ts);

            /* Estimated frequency must be within defined limits */
            if ((f_est >= self->f_min) && (f_est <= self->f_max)) {
                /* Accumulate estimated frequency samples */
                self->f_sum += f_est;

                /* Check if all samples have been collected */
                if (++self->counter >= self->n_max) {
                    /* Estimate frequency */
                    self->f_est = self->f_sum / self->counter;

                    /* Frequency has been initialized */
                    return 1;
                }
            }
        }

        /* Update zero-crossing time instant */
        self->timer_0 = self->timer;
    }

    /* Update voltage state */
    self->v1 = v;

    /* Update software timer with timeout protection */
    self->timeout = (bool) (++self->timer > (self->t_max / self->ts));
    
    /* Frequency initialization is still running */
    return 0;
}


/**************************************************************************************************
 * 
 * Rotation direction detection
 * 
 *************************************************************************************************/
void ac_rod_reset(struct ac_rod *self)
{
    self->dir = AC_DIR_NONE;
    self->vd_min = 0.0f;
    self->vd_max = 0.0f;
    self->vd1 = 0.0f;
    self->counter = 0;
    self->timer = 0u;
    self->timeout = false;
}

int ac_rod_run(struct ac_rod *self, const struct ac_ab0 *ab0)
{
    /* Error check */
    if ((self->timeout == true) || (self->n_max == 0u))
        return -1;
    
    /* Check if rotation direction has been detected */
    if (self->dir != AC_DIR_NONE)
        return 1;

    /**
     * Zero crossing detection
     *  . "timer > 0" condition ensures that vd1, vd_min and vd_max are initialized before zero
     *    crossing detection, which is necessary condition.
     */
    if ((self->timer > 0) && ((ab0->alpha * self->vd1) <= 0.0f)) {
        /**
         * Check conditions for rotation direction:
         *  . CCW direction is when:
         *      - vd transitions from positive to negative and vq is positive, or
         *      - vd transitions from negative to positive and vq is negative.
         *  . CW direction is when:
         *      - vd transitions from positive to negative and vq is negative, or
         *      - vd transitions from negative to positive and vq is positive.
         * vd is direct component (alpha), vq is quadrature component (beta).
         */
        if (    ((self->vd_max > -self->vd_min) && (ab0->beta > 0.0f)) ||
                ((self->vd_max < -self->vd_min) && (ab0->beta < 0.0f))  )
            self->counter++; /* CCW */
        else if (   ((self->vd_max > -self->vd_min) && (ab0->beta < 0.0f)) ||
                    ((self->vd_max < -self->vd_min) && (ab0->beta > 0.0f))  )
            self->counter--; /* CW */
        
        /* Check if rotation direction has been initialized */
        if (+self->counter >= (int) self->n_max)
            self->dir = AC_DIR_CCW;
        if (-self->counter >= (int) self->n_max)
            self->dir = AC_DIR_CW;

        /* Reset minimum and maximum values */
        self->vd_min = 0.0f;
        self->vd_max = 0.0f;

        /* Check if rotation direction has been detected */
        if (self->dir != AC_DIR_NONE)
            return 1;
    }

    /* Update voltage state */
    self->vd1 = ab0->alpha;

    /* Update minimum and maximum values */
    if (ab0->alpha < self->vd_min)
        self->vd_min = ab0->alpha;
    if (ab0->alpha > self->vd_max)
        self->vd_max = ab0->alpha;

    /* Update software timer with timeout protection */
    self->timeout = (bool) (++self->timer > (self->t_max / self->ts));
    
    /* Rotation direction detection is still running */
    return 0;
}


/**************************************************************************************************
 * 
 * Amplitude detection
 * 
 *************************************************************************************************/
void ac_amp_reset(struct ac_amp *self)
{
    self->counter = 0u;
    self->a_est = 0.0f;
    self->sum = 0.0f;
}

int ac_amp_run(struct ac_amp *self, const struct ac_ab0 *ab0)
{
    /* Error check */
    if (self->n_max == 0u)
        return -1;
    
    /* Check if amplitude has been initialized */
    if (self->counter >= self->n_max)
        return 1;

    /* Calculate amplitude from SRF */
    float a_est = sqrtf((ab0->alpha * ab0->alpha) + (ab0->beta * ab0->beta));

    /* Gain correction for power-invariant transformation */
    if (ab0->type == AC_CLARKE_POWER_INVARIANT)
        a_est *= (2.0f / 3.0f);

    /* Accumulated calculated amplitude */
    self->sum += a_est;

    /* Update samples counter */
    if (++self->counter >= self->n_max) {
        /* Initialize amplitude */
        self->a_est = self->sum / self->counter;

        if(self->a_est < 0.01){
            return -1; /* amplitude is too low */
        }
        /* Amplitude has been initialized */
        return 1;
    }

    /* Amplitude initialization is still running */
    return 0;
}


/**************************************************************************************************
 * 
 * Orthogonal signal generator (quadrature)
 * 
 *************************************************************************************************/
int ac_osg_init(struct ac_osg *self)
{
    /* Check parameters */
    if ((self->ts <= 0.0f) || (self->fb <= 0.0f) || (self->fc <= 0.0f))
        return -1;

    /* Temporary variables */
    float WC_TSBY2 = (C_2PI * self->fc) * (self->ts / 2.0f);
    float WB_TSBY2 = (C_2PI * self->fb) * (self->ts / 2.0f);
    float WC_TSBY2_POW2 = (WC_TSBY2 * WC_TSBY2);
    float DEN = (1.0f + WB_TSBY2 + WC_TSBY2_POW2);

    /* Derivator gain */
    self->Q01 = 1.0f / WC_TSBY2;

    /**
     * Calculate resonant filter parameters:
     *      G(s) = (WB/WC) / ((s/WC)^2 + (WB/WC)*(s/WC) + 1)
     * 
     * The above transfer function is a band-pass resonant filter with embedded integrator which
     * will produce quadrature component. Derivative is then applied to the quadrature component
     * to extract direct component. Derivative is not a problem since band-pass filter does not
     * let through any high frequency components.
     * 
     * Filter parameters below are calculated using Tustin discretization:
     *      1/s -> (TS/2)*((z+1)/(z-1))
     * 
     * The band-pass resonant filter with transfer function
     *      G(s) = ((WB/WC)*(s/WC)) / ((s/WC)^2 + (WB/WC)*(s/WC) + 1)
     * would produce direct component. Integrator applied to the direct component would produce
     * quadrature component. However, any DC offset in the direct component would soon diverge
     * towards infinity, which might cause numerical instability.
     */
    self->U01 = (WC_TSBY2 * WB_TSBY2) / DEN;
    self->U21 = self->U01;
    self->UY1 = 4.0f * self->U01;
    self->Y12 = (1.0f - WB_TSBY2 + WC_TSBY2_POW2) / DEN;
    self->Y1 = (1.0f + (WB_TSBY2 * (1.0f + 4.0f * WC_TSBY2)) - (3.0f * WC_TSBY2_POW2)) / DEN;

    /* OSG has been initialized */
    self->init = true;

    /* Initialization successful */
    return 0;
}

void ac_osg_reset(struct ac_osg *self)
{
    self->u1 = 0.0f;
    self->u2 = 0.0f;
    self->d1 = 0.0f;
    self->q1 = 0.0f;
    self->q2 = 0.0f;
    self->init = false;
}

__attribute__((ramfunc))
void ac_osg_run(struct ac_osg *self, float u0, struct ac_ab0 *ab0)
{
    /* Check if OSG has been initialized */
    if (self->init == false)
        return;
    
    /* Temporary variables */
    float u01 =       u0 - self->u1;
    float u21 = self->u2 - self->u1;
    float uy1 = self->u1 - self->q1;
    float y12 = self->q1 - self->q2;

    /* Run resonant filter to get quadrature component */
    float q0 = (self->U01 * u01) + (self->U21 * u21) + (self->UY1 * uy1) + (self->Y12 * y12) + (self->Y1 * self->q1);

    /* Derivative to get direct component */
    float d0 = (self->Q01 * (q0 - self->q1)) - self->d1;

    /* Update states */
    self->u2 = self->u1;
    self->u1 = u0;
    self->d1 = d0;
    self->q2 = self->q1;
    self->q1 = q0;

    /* Update return value */
    ab0->alpha = d0;
    ab0->beta  = q0;
}
