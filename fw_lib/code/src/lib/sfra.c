/**************************************************************************************************
 *
 * \file sfra.c
 *
 * \brief sfra abstraction implementation 
 *
 * \author Balaje Karthikeyan
 *
 *************************************************************************************************/

#include "inc/lib/sfra.h"

#include "app/tlo.h"
#include "app/user.h"

#include "inc/api/db.h"

#include "sfra_f_db.h"
#include <math.h>

#if defined(F2803X)
/* For f28035 MCU there are issues with the RAM size and will result in an overshooting of the RAM. So inorder to adapt the sfra module we have to make 
some changes to the RAM allocation in the linker file(flash.ld) and reduce the stack size in the cmake file(toolchain_c2000.cmake)*/

#define C_SFRA_FREQ_LENGTH 100

/* This value is to convert from IQ to FLOAT. According to the manual, the values are stored in IQ24, but this is WRONG
(https://e2e.ti.com/support/microcontrollers/c2000-microcontrollers-group/c2000/f/c2000-microcontrollers-forum/657355/linux-tms320f28035-how-to-get-floating-values-in-a-fixed-point-processor-in-memory-browser-for-sfra)
They are actually stored in IQ16 format (so the online manual is wrong)*/
#define VEC_SLOPE ((float)(0.000015259))
static SFRA_IQ sfra1;

#elif defined(F28004X)
#define C_SFRA_FREQ_LENGTH 100
static SFRA_F32 sfra1;

#endif

#if defined(F2803X)
struct sfra_priv {
    // Vectors
    int32_t *ol_mag_v;
    int32_t *ol_phase_v;
    int32_t *plant_mag_v;
    int32_t *plant_phase_v;
    float *freq_v;

    // SFRA API
    SFRA_IQ *sfra_api;
    float start;
    float step;
    float amplitude;
};

#elif defined(F28004X)
struct sfra_priv {
    // Vectors
    float *ol_mag_v;
    float *ol_phase_v;
    float *plant_mag_v;
    float *plant_phase_v;
    float *freq_v;

    // SFRA API
    SFRA_F32 *sfra_api;
    float start;
    float step;
    float amplitude;
};
#endif
// Vectors

#if defined(F2803X)

static int32_t plantMagVect[C_SFRA_FREQ_LENGTH]   = {[C_SFRA_FREQ_LENGTH - 1] = 0.0};
static int32_t plantPhaseVect[C_SFRA_FREQ_LENGTH] = {[C_SFRA_FREQ_LENGTH - 1] = 0.0};
static int32_t olMagVect[C_SFRA_FREQ_LENGTH]      = {[C_SFRA_FREQ_LENGTH - 1] = 0.0};
static int32_t olPhaseVect[C_SFRA_FREQ_LENGTH]    = {[C_SFRA_FREQ_LENGTH - 1] = 0.0};
static float freqVect[C_SFRA_FREQ_LENGTH]       = {[C_SFRA_FREQ_LENGTH - 1] = 0.0};

#elif defined(F28004X)

static float plantMagVect[C_SFRA_FREQ_LENGTH]   = {[C_SFRA_FREQ_LENGTH - 1] = 0.0};
static float plantPhaseVect[C_SFRA_FREQ_LENGTH] = {[C_SFRA_FREQ_LENGTH - 1] = 0.0};
static float olMagVect[C_SFRA_FREQ_LENGTH]      = {[C_SFRA_FREQ_LENGTH - 1] = 0.0};
static float olPhaseVect[C_SFRA_FREQ_LENGTH]    = {[C_SFRA_FREQ_LENGTH - 1] = 0.0};
static float freqVect[C_SFRA_FREQ_LENGTH]       = {[C_SFRA_FREQ_LENGTH - 1] = 0.0};
static float clMagVect[C_SFRA_FREQ_LENGTH]      = {[C_SFRA_FREQ_LENGTH - 1] = 0.0};
static float clPhaseVect[C_SFRA_FREQ_LENGTH]    = {[C_SFRA_FREQ_LENGTH - 1] = 0.0};
#endif

#ifdef SFRA_F
#pragma DATA_SECTION(plantMagVect,"dlog_data")
#pragma DATA_SECTION(plantPhaseVect,"dlog_data")
#pragma DATA_SECTION(olMagVect,"dlog_data")
#pragma DATA_SECTION(olPhaseVect,"dlog_data")
#pragma DATA_SECTION(freqVect,"dlog_data")
#if defined(F28004X)

#pragma DATA_SECTION(clMagVect,"dlog_data")
#pragma DATA_SECTION(clPhaseVect,"dlog_data")
#endif
#pragma DATA_SECTION(sfra1,"dlog_data")
#endif




/**************************************************************************************************
 * 
 * sfra_init()
 * 
 *************************************************************************************************/
struct sfra *sfra_init()
{

#if defined(F2803X)
    sfra1.H_MagVect = plantMagVect;
    sfra1.H_PhaseVect = plantPhaseVect;
    sfra1.GH_MagVect = olMagVect;
    sfra1.GH_PhaseVect = olPhaseVect;
    sfra1.FreqVect = freqVect;
    sfra1.amplitude = C_SFRA_AMPLITUDE;
    sfra1.Vec_Length = C_SFRA_FREQ_LENGTH;
    sfra1.ISR_Freq = SFRA_ISR_FREQ;
    sfra1.Freq_Start = SFRA_FREQ_START;
    sfra1.Freq_Step = C_SFRA_FREQ_STEP_MULTIPLY;

    static struct sfra_priv priv = {
        .plant_mag_v   = plantMagVect,
        .plant_phase_v = plantPhaseVect,
        .ol_mag_v      = olMagVect,
        .ol_phase_v    = olPhaseVect,
        .freq_v        = freqVect,
        .sfra_api      = &sfra1,
    };

#elif defined(F28004X)

    static struct sfra_priv priv = {
                .plant_mag_v   = plantMagVect,
                .plant_phase_v = plantPhaseVect,
                .ol_mag_v      = olMagVect,
                .ol_phase_v    = olPhaseVect,
                .freq_v        = freqVect,
                .sfra_api      = &sfra1,
            };

#endif

    static struct sfra sfra = {
        .enabled = false,
        .priv    = &priv,
    };

    sfra_strat_sweep(&sfra);

    return &sfra;
}


/**************************************************************************************************
 * 
 * sfra_periodic()
 * 
 *************************************************************************************************/

void sfra_periodic(const struct sfra *self)
{

#if defined(F2803X)
    SFRA_IQ *sfra_api      = self->priv->sfra_api;
    SFRA_IQ_BACKGROUND(sfra_api);
#elif defined(F28004X)
    SFRA_F32 *sfra_api       = self->priv->sfra_api;
    SFRA_F32_runBackgroundTask(sfra_api);
#endif

}

/**************************************************************************************************
 * 
 * sfra_strat_sweep()
 * 
 *************************************************************************************************/
void sfra_strat_sweep(const struct sfra *sfra)
{

#if defined(F2803X)
    SFRA_IQ *sfra_api     = sfra->priv->sfra_api;
    SFRA_IQ_INIT(sfra_api);

#elif defined(F28004X)
    SFRA_F32 *sfra_api     = sfra->priv->sfra_api;
    //Resets the internal data of sfra module to zero
    //
    SFRA_F32_reset(sfra_api);

    //
    //Configures the SFRA module
    //
    SFRA_F32_config(sfra_api,
                    C_SFRA_ISR_FREQ,
                    sfra_api->amplitude,
                    C_SFRA_FREQ_LENGTH,
                    sfra_api->freqStart,
                    sfra_api->freqStep,
                    plantMagVect,
                    plantPhaseVect,
                    olMagVect,
                    olPhaseVect,
                    clMagVect,
                    clPhaseVect,
                    freqVect,
                    1);

    //
    //Resets the response arrays to all zeroes
    //
    SFRA_F32_resetFreqRespArray(sfra_api);

    //
    //Initializes the frequency response array ,
    //The first element is SFRA_FREQ_START
    //The subsequent elements are freqVect[n-1]*C_SFRA_FREQ_STEP_MULTIPLY
    //This enables placing a fixed number of frequency points
    //between a decade of frequency.
    // The below routine can be substituted by a routine that sets
    // the frequency points arbitrarily as needed.
    //
    SFRA_F32_initFreqArrayWithLogSteps(sfra_api,
                                       sfra_api->freqStart,
                                       sfra_api->freqStep);
#endif

}


/**************************************************************************************************
 *
 * sfra_f_db_init()
 *
 *************************************************************************************************/
DB_PROTOTYPE_INIT(sfra_f_db)
{

    /* Periodic callbacks */

    DB_SET_PERIODIC_CALLBACK(sfra_f_sfra_status);
    DB_SET_PERIODIC_CALLBACK(sfra_f_sfra_frequency_index);
    DB_SET_PERIODIC_CALLBACK(sfra_f_sfra_freq_vector);
    DB_SET_PERIODIC_CALLBACK(sfra_f_sfra_ol_mag_vector);
    DB_SET_PERIODIC_CALLBACK(sfra_f_sfra_ol_phase_vector);
    DB_SET_PERIODIC_CALLBACK(sfra_f_sfra_plant_mag_vector);
    DB_SET_PERIODIC_CALLBACK(sfra_f_sfra_plant_phase_vector);

    /* NOT Periodic callbacks (called just once now) */
    return 0;
}


/**************************************************************************************************
 *
 * sfra_f_sfra_command_callback()
 *
 *************************************************************************************************/
DB_PROTOTYPE_CALLBACK(sfra_f_sfra_command)
{

    if (msg->start) {

        sfra_strat_sweep(tlo->sfra);
        sfra1.start      = 1;
    }

}


/**************************************************************************************************
 *
 * sfra_f_sfra_frequency_start_callback()
 *
 *************************************************************************************************/
DB_PROTOTYPE_CALLBACK(sfra_f_sfra_frequency_start)
{
#if defined(F2803X)
    sfra1.Freq_Start = msg->start_frequency;
#elif defined(F28004X)
    sfra1.freqStart = msg->start_frequency;
#endif
}

/**************************************************************************************************
 *
 * sfra_f_sfra_frequency_step_callback()
 *
 *************************************************************************************************/
DB_PROTOTYPE_CALLBACK(sfra_f_sfra_frequency_step)
{
#if defined(F2803X)
    sfra1.Freq_Step = msg->frequency_step;
#elif defined(F28004X)
    sfra1.freqStep = msg->frequency_step;
#endif
}

/**************************************************************************************************
 *
 * sfra_f_sfra_injection_amplitude_callback()
 *
 *************************************************************************************************/
DB_PROTOTYPE_CALLBACK(sfra_f_sfra_injection_amplitude)
{
#if defined(F2803X)  
    sfra1.amplitude = _IQ26(msg->injection_amplitude);
#elif defined(F28004X)
    sfra1.amplitude = msg->injection_amplitude;
#endif
}


/**************************************************************************************************
 *
 * sfra_f_sfra_status_callback()
 *
 *************************************************************************************************/
DB_PROTOTYPE_CALLBACK(sfra_f_sfra_status)
{
    msg->status    = sfra1.status;
    msg->state     =  sfra1.state;

#if defined(F2803X)
    msg->vector_length =  sfra1.Vec_Length;
#elif defined(F28004X)
    msg->vector_length =  sfra1.vecLength;
#endif

}
/**************************************************************************************************
 *
 * sfra_f_sfra_status_callback()
 *
 *************************************************************************************************/
DB_PROTOTYPE_CALLBACK(sfra_f_sfra_frequency_index)
{

#if defined(F2803X)
    msg->frequency_index =  sfra1.FreqIndex;
#elif defined(F28004X)
    msg->frequency_index =  sfra1.freqIndex;
#endif


}
/**************************************************************************************************
 *
 * sfra_f_sfra_status_callback()
 *
 *************************************************************************************************/
DB_PROTOTYPE_CALLBACK(sfra_f_sfra_freq_vector)
{
    struct sfra_priv *priv = tlo->sfra->priv;

    msg->vector_index = (msg->vector_index + 1) % C_SFRA_FREQ_LENGTH;
    msg->frequency    = priv->freq_v[msg->vector_index];

}
/**************************************************************************************************
 *
 * sfra_f_sfra_status_callback()
 *
 *************************************************************************************************/
DB_PROTOTYPE_CALLBACK(sfra_f_sfra_ol_mag_vector)
{

    struct sfra_priv *priv = tlo->sfra->priv;
    msg->vector_index = (msg->vector_index + 1) % C_SFRA_FREQ_LENGTH;
    
#if defined(F2803X)
    msg->magnitude    = VEC_SLOPE * ((float)priv->ol_mag_v[msg->vector_index]);
    
#elif defined(F28004X)
    msg->magnitude    = priv->ol_mag_v[msg->vector_index];
#endif
}
/**************************************************************************************************
 *
 * sfra_f_sfra_status_callback()
 *
 *************************************************************************************************/
DB_PROTOTYPE_CALLBACK(sfra_f_sfra_ol_phase_vector)
{
    
struct sfra_priv *priv = tlo->sfra->priv;
    msg->vector_index = (msg->vector_index + 1) % C_SFRA_FREQ_LENGTH;
#if defined(F2803X)
    msg->phase        = _IQtoF( * ((float)priv->ol_phase_v[msg->vector_index]));
    msg->phase        = VEC_SLOPE * ((float)priv->ol_phase_v[msg->vector_index]);
#elif defined(F28004X)
    msg->phase        = priv->ol_phase_v[msg->vector_index];
#endif

}
/**************************************************************************************************
 *
 * sfra_f_sfra_status_callback()
 *
 *************************************************************************************************/
DB_PROTOTYPE_CALLBACK(sfra_f_sfra_plant_mag_vector)
{
    struct sfra_priv *priv = tlo->sfra->priv;

    msg->vector_index = (msg->vector_index + 1) % C_SFRA_FREQ_LENGTH;
#if defined(F2803X)
    msg->magnitude    = VEC_SLOPE * ((float)priv->plant_mag_v[msg->vector_index]);
#elif defined(F28004X)
    msg->magnitude    = priv->plant_mag_v[msg->vector_index];
#endif
}
/**************************************************************************************************
 *
 * sfra_f_sfra_status_callback()
 *
 *************************************************************************************************/
DB_PROTOTYPE_CALLBACK(sfra_f_sfra_plant_phase_vector)
{
    struct sfra_priv *priv = tlo->sfra->priv;

    msg->vector_index = (msg->vector_index + 1) % C_SFRA_FREQ_LENGTH;
#if defined(F2803X)
    msg->phase        = VEC_SLOPE * ((float)priv->plant_phase_v[msg->vector_index]);
#elif defined(F28004X)
    msg->phase        = priv->plant_phase_v[msg->vector_index];
#endif
}

