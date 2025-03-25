/**************************************************************************************************
 * 
 * \file ghm.c
 * 
 * \brief Grid health monitor implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "inc/lib/ghm.h"

#include "inc/api/adc.h"
#include "inc/lib/conv.h"
#include "inc/lib/debug.h"
#include "inc/lib/rms.h"

/**************************************************************************************************
 * 
 * Defines for the algorithm
 * 
 *************************************************************************************************/

/* Minimum number of consecutive negative samples for zero-crossing detection */
#define DEBOUNCING      10U

/* Number of fundamental periods for mean frequency and phase calculation */
#define PERIODS         10U

#define RMS_MIN         50.0f           /* Minimum allowed grid RMS voltage (V)                 */

#define FREQ_MIN        45.0f           /* Minimum allowed grid frequency (Hz)                  */
#define FREQ_MAX        65.0f           /* Maximum allowed grid frequency (Hz)                  */

#define PHASE_120       120.0f          /* Allowed grid phase 2PI/3 (degrees)                   */
#define PHASE_240       240.0f          /* Allowed grid phase 4PI/3 (degrees)                   */

#define PHASE_TOL       20.0f           /* Grid phase tolerance (degrees)                       */

#define PHASE_120_MIN   (PHASE_120 - PHASE_TOL)
#define PHASE_120_MAX   (PHASE_120 + PHASE_TOL)

#define PHASE_240_MIN   (PHASE_240 - PHASE_TOL)
#define PHASE_240_MAX   (PHASE_240 + PHASE_TOL)

/**************************************************************************************************
 * 
 * \brief Resets grid health monitor
 * 
 * \param ghm monitor object handler
 * 
 * \return None
 * 
 *************************************************************************************************/
__attribute__((ramfunc)) static void
ghm_reset(struct ghm *ghm)
{
    ASSERT(ghm);

    struct ghm_priv *priv = &ghm->priv;
    struct ghm_out *out = &ghm->out;
    struct ghm_var *var = priv->var;

#if 0
    /* RMS flag should not be reset */
    out->sf.rms = false;
#endif
    out->sf.init = false;
    out->sf.freq = false;
    out->sf.phase = false;

    unsigned i;
    for (i = 0U; i < priv->n_phases; i++) {
        var[i].negative = 0U;
        var[i].time = 0U;

        var[i].freq = 0U;
        var[i].phase = 0U;
        var[i].counter = 0U;

#if 0
        /* RMS flag should not be reset */
        var[i].sf.rms = false;
#endif
        var[i].sf.init = false;
        var[i].sf.freq = false;
        var[i].sf.phase = false;

        out->freq[i] = 0.0f;
        out->phase[i] = 0.0f;
    }
}

/**************************************************************************************************
 * 
 * ghm_init()
 * 
 *************************************************************************************************/
int
ghm_init(struct ghm *ghm)
{
    ASSERT(ghm);

    struct ghm_priv *priv = &ghm->priv;
    struct ghm_usr *usr = &ghm->usr;
    struct ghm_var *var = priv->var;

    ASSERT(usr->TS > 0.0f);

    if(usr->n_phases == 1){
        priv->n_phases = 1;
    }
    else{
        priv->n_phases = 3; // Number of phases is 3 by default
    }

    unsigned i;
    for (i = 0U; i < priv->n_phases; i++) {
        var[i].rms = usr->rms[i];
        if (!var[i].rms) {
            return -1;
        }

        /** TODO: Reset RMS flag */
    }

    priv->TS = usr->TS;

    priv->min = conv_p2n(var[0].rms->adc->conv, RMS_MIN);

    ghm_reset(ghm);

    return 0;
}

/**************************************************************************************************
 * 
 * ghm_run()
 * 
 *************************************************************************************************/
__attribute__((ramfunc)) void
ghm_run(struct ghm *ghm)
{
    struct ghm_priv *priv = &ghm->priv;
    struct ghm_out *out = &ghm->out;
    struct ghm_var *var = priv->var;

    /**
     * Free-running timestamp counter. It can never be zero since that value is used to indicate
     * zero-crossing timestamp has not been initialized.
     */
    if (++priv->timer == 0U) {
        priv->timer = 1U;
    }

    bool zcd;
    /* True when zero-crossing has been detected at least once for all three phases */
    if(priv->n_phases == 1){
        zcd = var[0].time > 0U;
    }else{
        zcd = (var[0].time > 0U) && (var[1].time > 0U) && (var[2].time > 0U);
    }

    /* Global status flags for all three phases combined */
    struct ghm_sf sf = {
        .init = true,
        .rms = true,
        .freq = true,
        .phase = true
    };

    unsigned i;
    for (i = 0U; i < priv->n_phases; i++) {
        const struct rms_var *rms = var[i].rms;
        const struct adc_var *adc = rms->adc;

        var[i].sf.rms = (rms->meas.norm >= priv->min);
        sf.rms = (sf.rms && var[i].sf.rms);

        /* Algorithm can run only when RMS voltage is within limits */
        if (!sf.rms) {
            continue;
        }

        /* Remove DC component from instantaneous measurement */
        _iq ac = adc->meas.norm - adc->meas.flt;
        
        /**
         * Zero-crossing event occurs when positive sample comes after \b DEBOUNCING number of
         * negative samples. The \e negative variable keeps track how many consecutive negative
         * samples have occurred.
         */
        if ((ac >= _IQ(0.L)) && (var[i].negative > DEBOUNCING)) {
            /**
             * Process zero-crossing event only if zero-crossing has occurred at least once for
             * all three phases. This is important for phase detection.
             */
            if (zcd) {
                /* Frequency estimation (accounts timer overflow) */
                uint32_t freq = priv->timer - var[i].time;
                if (priv->timer <= var[i].time) {
                    freq += 0xFFFFFFFFUL;
                }

                /* Index of next phase for phase detection */
                unsigned j = ((i == 2U) ? 0U : (i + 1U));

                /* Phase detection (accounts timer overflow) */
                uint32_t phase;
                if(priv->n_phases == 1){
                    phase = 0;
                }else{
                    phase = priv->timer - var[j].time;
                }
                if (priv->timer <= var[j].time) {
                    phase += 0xFFFFFFFFUL;
                }

                var[i].freq += freq;
                var[i].phase += phase;

                if (++var[i].counter >= PERIODS) {
                    ASSERT(priv->TS && var[i].freq);

                    float freq = ((float) PERIODS / var[i].freq) / priv->TS;
                    float phase = ((float) var[i].phase / var[i].freq) * 360.0f;

                    var[i].sf.init = true;
                    var[i].sf.freq = (freq >= FREQ_MIN) && (freq <= FREQ_MAX);
                    if(priv->n_phases == 1){
                        var[i].sf.phase = true;
                    }else{
                        var[i].sf.phase = ((phase >= PHASE_120_MIN) && (phase <= PHASE_120_MAX)) ||
                            ((phase >= PHASE_240_MIN) && (phase <= PHASE_240_MAX));
                    }

                    out->freq[i] = freq;
                    out->phase[i] = phase;

                    /* Reset accumulators */
                    var[i].freq = 0U;
                    var[i].phase = 0U;
                    var[i].counter = 0U;
                }
            }

            var[i].time = priv->timer;
        }

        sf.init = (sf.init && var[i].sf.init);
        sf.freq = (sf.freq && var[i].sf.freq);
        sf.phase = (sf.phase && var[i].sf.phase);

        if (ac < _IQ(0.L)) {
            var[i].negative++;
        } else { /* ac >= 0 */
            var[i].negative = 0U;
        }
    }

    /**
     * Grid health monitor should run only when RMS voltage is within limits. Otherwise, the
     * frequency and phase detection would be estimated on measurement noise.
     */
    if (!sf.rms) {
        ghm_reset(ghm);
    }

    /* Copy shadow status flags */
    out->sf = sf;
}
