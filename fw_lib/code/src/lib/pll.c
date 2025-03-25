/**************************************************************************************************
 * 
 * \file pll.c
 * 
 * \brief Phase Locked Loop functions interface 
 * 
 * \author Jorge Sola
 * 
 *************************************************************************************************/
/**
 *  FILE:   pll.c
 *  AUTHOR: Marko Gulin, 2020
 */

#include "inc/lib/pll.h"

#include "inc/lib/utils.h"

/**************************************************************************************************
 * 
 * Algorithm constants
 * 
 *************************************************************************************************/
#define C_TMR_OSG       (0.15f)             /* Time to wait for OSG to settle (s)           */

#define C_F_ERR         (35.0f)             /* Frequency correction limit (Hz)              */
#define C_DDSRF_FC      (30.0f)             /* Cut-off frequency for DDSRF extension (Hz)   */
#define C_FNXX_FB       (60.0f)            /* 3-dB bandwidth for resonant filters (Hz)     */

#define C_FREQ_T_MAX    (0.5f)              /* Frequency initialization timeout (s)         */
#define C_FREQ_N_MAX    (8u)                /* Number of frequency samples (-)              */
#define C_FREQ_F_MIN    (38.0f)             /* Minimum allowed frequency (Hz)               */
#define C_FREQ_F_MAX    (72.0f)             /* Maximum allowed frequency (Hz)               */

#define C_ROD_T_MAX     (0.5f)              /* Rotation direction detection timeout (s)     */
#define C_ROD_N_MAX     (5u)                /* Number of consecutive direction samples (-)  */

#define C_AMP_N_MAX     (64u)               /* Number of amplitude samples (-)              */


/**************************************************************************************************
 * 
 * Public function definitions
 * 
 *************************************************************************************************/
int pll_setup(struct pll *self)
{
    /* Pointers for shorter notation */
    struct pll_priv *priv = &self->priv;
    struct pll_in *in = &self->in;
    struct pll_out *out = &self->out;
    struct pll_in *_in = &priv->in;
    struct pll_iir *_iir = &priv->iir;

    /* Temporary variable */
    int ret = 0;

    /* Lock state machine by default */
    priv->state = PLL_STATE_ERROR;
    priv->error = PLL_ERROR_SETUP;

    /**
     *  PARAMETERS VALIDITY CHECK
     * 
     *      Example of parameters that work well with 50 Hz and 60 Hz electric grids:
     * 
     *          WN        ZETA        Q_LOCK    T_LOCK
     *          2PI*25    SQRT2BY2    0.05      0.1
     */

    /* Parameters validity check */
    if (in->ts <= 0.0f)
        return -1;
    if ((in->wn <= 0.0f) || (in->zeta <= 0.0f))
        return -1;
    if ((in->q_lock <= 0.0f) || (in->t_lock <= 0.0f))
        return -1;
    
    /* Latch input parameters */
    *_in = *in;

    /* Calculate software timer constants */
    priv->t_osg = (uint64_t) (C_TMR_OSG / _in->ts);
    priv->t_lock = (uint64_t) (_in->t_lock / _in->ts);

    /**
     *      INITIALIZE OUTPUT VARIABLES
     */

#define PLL_CONNECT_OUTPUT(VAR)     out->VAR = &priv->VAR

    /* Connect output variables */
    PLL_CONNECT_OUTPUT(state);
    PLL_CONNECT_OUTPUT(error);
    PLL_CONNECT_OUTPUT(grid);
    PLL_CONNECT_OUTPUT(v_grid_ab0);
    PLL_CONNECT_OUTPUT(v_grid_dq0_p);
    PLL_CONNECT_OUTPUT(v_grid_dq0_n);
    PLL_CONNECT_OUTPUT(in_lock);
    
    /**
     *      OBJECT INITIALIZATION
     */

    /* Grid phasor */
    priv->grid.ts = _in->ts;

    /* Frequency initialization */
    priv->freq.ts = _in->ts;
    priv->freq.t_max = C_FREQ_T_MAX;
    priv->freq.n_max = C_FREQ_N_MAX;
    priv->freq.f_min = C_FREQ_F_MIN;
    priv->freq.f_max = C_FREQ_F_MAX;

    /* Rotation direction detection */
    priv->rod.ts = _in->ts;
    priv->rod.t_max = C_ROD_T_MAX;
    priv->rod.n_max = C_ROD_N_MAX;

    /* Amplitude initialization */
    priv->amp.n_max = C_AMP_N_MAX;

    /* Orthogonal signal generator */
    priv->osg.ts = _in->ts;

    /* PI controller for frequency estimation */
    priv->pi_w_err.usr = (struct pid_usr) {
        .ts = _in->ts,
        .kp = 2.0f * (_in->zeta * _in->wn),
        .ti = 2.0f * (_in->zeta / _in->wn),
        .low = -(C_2PI * C_F_ERR),
        .high = +(C_2PI * C_F_ERR),
    };

    /* Setup PI controller */
    ret |= pid_setup(&priv->pi_w_err, PID_TYPE_PI);

    /* Decoupled double SRF PLL extension */
    struct iir_usr ddsrf_in = (struct iir_usr) {
        .TS = _in->ts,
        .FC = C_DDSRF_FC,
    };
    _iir->fdp_d.usr = ddsrf_in;
    _iir->fdp_q.usr = ddsrf_in;
    _iir->fdn_d.usr = ddsrf_in;
    _iir->fdn_q.usr = ddsrf_in;

    /* Setup low-pass filters */
    ret |= iir_setup(&_iir->fdp_d, IIR_LOW_PASS);
    ret |= iir_setup(&_iir->fdp_q, IIR_LOW_PASS);
    ret |= iir_setup(&_iir->fdn_d, IIR_LOW_PASS);
    ret |= iir_setup(&_iir->fdn_q, IIR_LOW_PASS);

    /* Q component 6th and 12th harmonic rejection */
    struct iir_usr fnxx_in = (struct iir_usr) {
        .TS = _in->ts,
        .FC = 0.0f, /* Initialized after frequency has been initialized */
        .FB = C_FNXX_FB,
    };
    _iir->fn6_q .usr = fnxx_in;
    _iir->fn12_q.usr = fnxx_in;

    /**
     *      RETURN
     */

    /* Reset object */
    pll_reset(self);

    /* Check initialization error */
    if (ret < 0)
        return -1;

    /* Unlock state machine */
    priv->state = PLL_STATE_NONE;
    priv->error = PLL_ERROR_NONE;

    /* Return */
    return 0;
}

void pll_reset(struct pll *self)
{
    /* Pointers for shorter notation */
    struct pll_priv *priv = &self->priv;
    struct pll_iir *_iir = &priv->iir;

    /**
     * Reset state machine and clear error flag
     *  . State machine can recover from PLL_ERROR_SETUP error only in pll_setup() function.
     */
    if (priv->error != PLL_ERROR_SETUP) {
        priv->state = PLL_STATE_NONE;
        priv->error = PLL_ERROR_NONE;
    }

    /* IIR filters */
    iir_reset(&_iir->fdp_d);
    iir_reset(&_iir->fdp_q);
    iir_reset(&_iir->fdn_d);
    iir_reset(&_iir->fdn_q);
    iir_reset(&_iir->fn6_q);
    iir_reset(&_iir->fn12_q);

    /* PI controller */
    pid_reset(&priv->pi_w_err);

    /* Grid variables */
    ac_grid_reset(&priv->grid);
    ac_freq_reset(&priv->freq);
    ac_rod_reset(&priv->rod);
    ac_amp_reset(&priv->amp);
    ac_osg_reset(&priv->osg);

    /* Grid voltages in SRF */
    priv->v_grid_ab0 = (struct ac_ab0) {
        .type = AC_CLARKE_AMPLITUDE_INVARIANT,
        .alpha = 0.0f,
        .beta = 0.0f,
        .zero = 0.0f,
    };

    /* Grid voltage in RRF with positive rotation */
    priv->v_grid_dq0_p = (struct ac_dq0) {
        .type = AC_PARK_POSITIVE_ROTATION,
        .d = 0.0f,
        .q = 0.0f,
        .o = 0.0f,
    };

    /* Grid voltage in RRF with negative rotation */
    priv->v_grid_dq0_n = (struct ac_dq0) {
        .type = AC_PARK_NEGATIVE_ROTATION,
        .d = 0.0f,
        .q = 0.0f,
        .o = 0.0f,
    };

    /* Nominal grid frequency */
    priv->f_nom = 0.0f;
    priv->w_nom = 0.0f;

    /* Orthogonal signal generator */
    priv->osg_en = false;

    /* Software timer */
    priv->timer = 0u;

    /* In-lock flag */
    priv->in_lock = false;
}


__attribute__((ramfunc))
void pll_run(struct pll *self, const struct ac_abcn *v_grid_abcn)
{
    /* Pointers for shorter notation */
    struct pll_priv *priv = &self->priv;
    struct pll_in *in = &self->in;
    struct pll_in *_in = &priv->in;
    struct pll_iir *_iir = &priv->iir;

    /* Reset in-lock flag by default */
    priv->in_lock = false;

    /* Calculate variables in SRF */
    if (_in->type == PLL_TYPE_3P)
        /* Clarke transformation (amplitude invariant) */
        ac_fwd_clarke(&priv->v_grid_ab0, v_grid_abcn);
    else if (priv->osg_en == true) /* PLL_TYPE_1P */
        /* Orthogonal signal generator */
        ac_osg_run(&priv->osg, v_grid_abcn->an, &priv->v_grid_ab0);

    /* Update sin and cos for grid phase */
    ac_grid_trigonometry(&priv->grid);

    /* Park transformation (positive rotation) */
    ac_fwd_park(&priv->v_grid_dq0_p, &priv->v_grid_ab0, &priv->grid.trig);

    /**
     * Positive rotation q variable is used as input to the frequency estimator
     *  . This variable is re-initialized if DDSRF PLL extension is enabled.
     */
    float v_q = priv->v_grid_dq0_p.q;

    /* Temporary variables */
    int ret = 0;
    float v_norm_q, w_err;
    struct ac_dq0 v_grid_dq0_dp, v_grid_dq0_dn;
    struct ac_trig trig_2;

    /**
     *  PLL ALGORITHM STATE MACHINE
     */
    switch (priv->state) {
    /**
     *  FIRST CALL AFTER RESET
     */
    case PLL_STATE_NONE:
        /* Latch input variables */
        _in->type = in->type;
        _in->ddsrf_en = in->ddsrf_en;
        _in->fn6_en = in->fn6_en;
        _in->fn12_en = in->fn12_en;

        /* Transition to next state */
        priv->state = PLL_STATE_FREQUENCY;
        break;

    /**
     *  ERROR STATE
     */
    case PLL_STATE_ERROR:
        /* State machine is locked */
        break;
    
    /**
     *  FREQUENCY INITIALIZATION BASED ON ZERO CROSSING
     */
    case PLL_STATE_FREQUENCY:
        /**
         * Initialize frequency based on zero crossing
         *  . In three-phase PLL routine, input signal should be direct (alpha) or quadrature
         *    (beta) component, it does not matter which one exactly.
         *  . In single-phase PLL routine, input signal is phase voltage, measured between two
         *    phases in order to eliminate bias.
         * 
         * Since frequency initialization is based on zero crossing detection, input signal must
         * not have bias. Forward Clarke transformation captures bias in zero component, ie alpha
         * and beta components do not contain bias. In single-phase scenario, bias is eliminated
         * when input voltage is measured between two phases, eg active and neutral.
         */
        if (_in->type == PLL_TYPE_3P)
            ret = ac_freq_run(&priv->freq, priv->v_grid_ab0.alpha);
        else /* PLL_TYPE_1P */
            ret = ac_freq_run(&priv->freq, v_grid_abcn->an);
        
        /* Check return value */
        if (ret < 0) {
            /* Frequency initialization error */
            priv->state = PLL_STATE_ERROR;
            priv->error = PLL_ERROR_FREQUENCY;
        }

        /* Frequency initialization is still running */
        else if (ret == 0) {
            /* Do nothing */
        }
        
        /* Frequency has been initialized */
        else { /* ret > 0 */
            /* Initialize nominal frequency */
            priv->f_nom = (priv->freq.f_est < 55.0f) ? 50.0f : 60.0f;
            priv->w_nom = C_2PI * priv->f_nom;

            /* Transition to next state */
            if (_in->type == PLL_TYPE_3P) {
                /* Detect rotation direction */
                priv->state = PLL_STATE_ROTATION;

            } else { /* PLL_TYPE_1P */
                /**
                 * Set OSG parameters
                 *  . When FB = FC, then FL = 0.618 * FC is the low frequency.
                 *    For example: FC = FB = 50.0 Hz -> FL = 30.9 Hz and FH = 80.9 Hz
                 *    Gain at FL and FH is 3 dB. Equations are FC = sqrt(FL*FH) and FB = FH - FL.
                 */
                priv->osg.fc = priv->f_nom;
                priv->osg.fb = 1.0f * priv->osg.fc;

                /* Initialize OSG */
                ret = ac_osg_init(&priv->osg);

                /* Check return value */
                if (ret < 0) {
                    /* OSG initialization error */
                    priv->state = PLL_STATE_ERROR;
                    priv->error = PLL_ERROR_OSG;
                }

                /* Enable OSG */
                priv->osg_en = true;

                /* Wait for OSG to settle */
                priv->timer = 0u;
                priv->state = PLL_STATE_OSG_WAIT;
            }
        }

        break;

    /**
     *  VOLTAGE VECTOR ROTATION DIRECTION DETECTION
     */
    case PLL_STATE_ROTATION:
        /* Run rotation direction detection */
        ret = ac_rod_run(&priv->rod, &priv->v_grid_ab0);

        /* Check return value */
        if (ret < 0) {
            /* Rotation direction detection error */
            priv->state = PLL_STATE_ERROR;
            priv->error = PLL_ERROR_ROTATION;
        }

        /* Rotation direction detection is still running */
        else if (ret == 0) {
            /* Do nothing */
        }
        
        /* Rotation direction has been detected */
        else { /* ret > 0 */
            /* Update nominal grid frequency */
            if (priv->rod.dir == AC_DIR_CW)
                priv->w_nom *= (-1.0f);
            
            /* Transition to next state */
            priv->state = PLL_STATE_AMPLITUDE;
        }

        break;

    /**
     *  WAIT FOR OSG TO SETTLE
     */
    case PLL_STATE_OSG_WAIT:
        /* Update software timer with timeout protection */
        if (++priv->timer > priv->t_osg) {
            /* Transition to next state */
            priv->timer = 0u;
            priv->state = PLL_STATE_AMPLITUDE;
        }

        break;

    /**
     *  AMPLITUDE INITIALIZATION FOR NORMALIZATION
     */
    case PLL_STATE_AMPLITUDE:
        /* Run amplitude initialization routine */
        ret = ac_amp_run(&priv->amp, &priv->v_grid_ab0);

        /* Check return value */
        if (ret < 0) {
            /* Amplitude initialization error */
            priv->state = PLL_STATE_ERROR;
            priv->error = PLL_ERROR_AMPLITUDE;
        }

        /* Amplitude initialization is still running */
        else if (ret == 0) {
            /* Do nothing */
        }
        
        /* Amplitude has been initialized */
        else { /* ret > 0 */
            /* Transition to next state */
            if (_in->type == PLL_TYPE_3P)
                priv->state = PLL_STATE_INIT_3P;
            else /* PLL_TYPE_1P */
                priv->state = PLL_STATE_INIT;
        }

        break;

    /**
     *  RESONANT FILTER INITIALIZATION
     */
    case PLL_STATE_INIT_3P:
        /* Setup band-stop filter for 6th harmonic */
        if (_in->fn6_en == true) {
            _iir->fn6_q.usr.FC = 6.0f * priv->f_nom;
            ret |= iir_setup(&_iir->fn6_q, IIR_BAND_STOP);
        }

        /* Setup band-stop filter for 12th harmonic */
        if (_in->fn12_en == true) {
            _iir->fn12_q.usr.FC = 12.0f * priv->f_nom;
            ret |= iir_setup(&_iir->fn12_q, IIR_BAND_STOP);
        }

        /* Check return value */
        if (ret < 0) {
            priv->state = PLL_STATE_ERROR;
            priv->error = PLL_ERROR_FNXX;
            break;
        }

        /* Intentional fall through */
    
    /**
     *  PLL ROUTINE INITIALIZATION
     */
    case PLL_STATE_INIT:
        /* Initialize grid phasor */
        ac_grid_init(&priv->grid, priv->amp.a_est, priv->freq.f_est, &priv->v_grid_ab0);
        
        /* Transition to next state */
        if ((_in->type == PLL_TYPE_3P) && (_in->ddsrf_en == true))
            priv->state = PLL_STATE_DDSRF;
        else /* PLL_TYPE_1P OR !DDSRF_EN */
            priv->state = PLL_STATE_RUN;

        break;
    
    /**
     *  DECOUPLED DOUBLE SRF PLL EXTENSION
     */
    case PLL_STATE_DDSRF:
        /* Forward Park transformation with negative rotation */
        ac_fwd_park(&priv->v_grid_dq0_n, &priv->v_grid_ab0, &priv->grid.trig);

        /* Trigonometry for double grid phase angle */
        trig_2._sin = (2.0f * priv->grid.trig._sin * priv->grid.trig._cos);
        trig_2._cos = (2.0f * priv->grid.trig._cos * priv->grid.trig._cos) - 1.0f;

        /**
         *  Decoupling network:
         *   'dp' and  'dn' are for          decoupled positive and negative rotation variables
         *  'fdp' and 'fdn' are for filtered decoupled positive and negative rotation variables
         */
        v_grid_dq0_dp.d = priv->v_grid_dq0_p.d -
            (*_iir->fdn_d.out * trig_2._cos) - (*_iir->fdn_q.out * trig_2._sin);
        v_grid_dq0_dp.q = priv->v_grid_dq0_p.q +
            (*_iir->fdn_d.out * trig_2._sin) - (*_iir->fdn_q.out * trig_2._cos);
        v_grid_dq0_dn.d = priv->v_grid_dq0_n.d -
            (*_iir->fdp_d.out * trig_2._cos) + (*_iir->fdp_q.out * trig_2._sin);
        v_grid_dq0_dn.q = priv->v_grid_dq0_n.q -
            (*_iir->fdp_d.out * trig_2._sin) - (*_iir->fdp_q.out * trig_2._cos);
        
        /* Low-pass filter on decoupled variables */
        iir_run(&_iir->fdp_d, v_grid_dq0_dp.d);
        iir_run(&_iir->fdp_q, v_grid_dq0_dp.q);
        iir_run(&_iir->fdn_d, v_grid_dq0_dn.d);
        iir_run(&_iir->fdn_q, v_grid_dq0_dn.q);

        /* Re-initialize input to frequency estimator */
        v_q = v_grid_dq0_dp.q;

        /* Intentional fall through to PLL_STATE_RUN */

    /**
     *  FREQUENCY AND PHASE ESTIMATION
     */
    case PLL_STATE_RUN:
        /**
         * Reject higher frequency harmonics from q component if PLL extension is enabled
         *  . 5th and 7th harmonics appear as 6th harmonic in dq frame, 11th and 13th appear as
         *    12th harmonic etc. There harmonics directly propagate to the estimated frequency,
         *    which makes the frequency oscillate. 3rd, 9th, 15th etc. harmonics are caputred
         *    by zero component in SRF or RRF and do not affect the frequency estimation.
         */
        if (_in->fn6_en == true){
            v_q = iir_run(&_iir->fn6_q, v_q);
        }
        if (_in->fn12_en == true){
            v_q = iir_run(&_iir->fn12_q, v_q);
        }



        /* Normalize grid voltage q component */
        v_norm_q = v_q / priv->amp.a_est;
        
        /* Estimate grid frequency correction (rad/s) */
        w_err = pid_run(&priv->pi_w_err, 0.0f, v_norm_q);

        /* Estimate grid frequency (rad/s) */
        priv->grid.rot.freq = priv->w_nom - w_err;

        /* Update phase angle (rad) */
        ac_grid_rotation(&priv->grid);

        /* Detect in-lock condition */
        if ((v_norm_q < -_in->q_lock) || (v_norm_q > _in->q_lock)) {
            priv->timer = 0u;
            priv->in_lock = false;
        } else if (++priv->timer >= priv->t_lock) {
            priv->timer = priv->t_lock;
            priv->in_lock = true;
        }

        break;

    /**
     *  UNEXPECTED STATE
     */
    default:
        /* Transition to error state */
        priv->state = PLL_STATE_ERROR;
        priv->error = PLL_ERROR_UNEXPECTED_STATE;
        break;
    }
}
