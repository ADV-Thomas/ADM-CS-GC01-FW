/**************************************************************************************************
 * 
 * \file hapi.c
 * 
 * \brief Hardware application interface abstraction
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "inc/hal/hapi.h"

#include "inc/hal/hal.h"
#include "inc/drv/clk.h"
#include "inc/drv/dio.h"
#include "inc/drv/pie.h"
#include "inc/lib/debug.h"
#include "inc/lib/nfo.h"

#include "driverlib/cpu.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**************************************************************************************************
 * 
 * Hardware application interface object
 * 
 *************************************************************************************************/
static struct _hapi_map map = {
    .trip     = IOX,
    .clear    = IOX,
    .internal = IOX,
    .combined = IOX
};

static struct _hapi_isr isr = {
    .run = NULL,
    .tlo = NULL,
    .isr_status = false,
};

static struct _hapi_isr isr_2 = {
    .run = NULL,
    .tlo = NULL,
    .isr_status = false,
};

static struct _hapi _hapi = {
    .map        = &map,
    .isr        = &isr,
    .isr_2      = &isr_2,
    .n_int      = 1,
    .isr_cpu_tics = 0,
    .hapi       = NULL,
    .setup      = NULL,
    .isr_clear  = NULL,
    .isr_2_clear= NULL,
    .adc_data   = NULL,
    .adc_read   = NULL,
    .wch_data   = NULL,
    .wch_get    = NULL,
    .wch_set    = NULL,
    .wch_clear  = NULL,
    .wch_calib_mode = NULL,
    .wcs_data   = NULL,
    .pwm_update = NULL,
};

/**************************************************************************************************
 * 
 * hapi_setup()
 * 
 *************************************************************************************************/
int
hapi_setup(const struct nfo *mod)
{
    int ret = 0;

    /* Resolve application-specific parameters and functions */
    hapi_resolve(&_hapi, mod);

    pie_init();

    io_connect(map.trip, IO_DOUT);
    io_connect(map.clear, IO_DOUT);
    io_connect(map.internal, IO_DIN);
    io_connect(map.combined, IO_DIN);

    /* Setup hardware application interface in application layer */
    ret = _hapi.setup ? _hapi.setup() : -1;

    if (ret < 0) {
        /**
         * Do not allow interlock clear if application layer has not been completely configured.
         * This will reconfigure the interlock clear line as digital input, which is safe since
         * that line has pull-down resistor on all boards.
         */
        io_disconnect(map.clear);
    }

    return (ret < 0) ? -1 : 0;
}

/**************************************************************************************************
 * 
 * hapi_isr_register()
 * 
 *************************************************************************************************/
int
hapi_isr_register(void (*run)(const struct tlo *tlo), const struct tlo *tlo)
{
    hapi_isr_disable();

    isr.run = run;
    isr.tlo = tlo;

    return 0;
}

/**************************************************************************************************
 * 
 * hapi_isr_register()
 * 
 *************************************************************************************************/
int
hapi_isr_2_register(void (*run)(const struct tlo *tlo), const struct tlo *tlo)
{
    hapi_isr_disable();

    isr_2.run = run;
    isr_2.tlo = tlo;

    return 0;
}

/**************************************************************************************************
 * 
 * hapi_isr_enable()
 * 
 *************************************************************************************************/
int
hapi_isr_enable(void)
{
    if (_hapi.n_int == 1){
        if (isr.run && _hapi.isr_clear) {
            EINT;
            isr.isr_status = true;
            return 0;
        } else {
            hapi_isr_disable();
            return -1;
        }
    }
    else if(_hapi.n_int == 2){
        if (isr.run && isr_2.run && _hapi.isr_2_clear && _hapi.isr_clear) {
            EINT;
            isr.isr_status = true;
            return 0;
        } else {
            hapi_isr_disable();
            return -1;
        }
    }
    return -1;
}

/**************************************************************************************************
 * 
 * hapi_isr_enable()
 * 
 *************************************************************************************************/
int
hapi_isr_2_enable(void)
{
    if (isr_2.run && _hapi.isr_2_clear) {
        EINT;
        return 0;
    } else {
        hapi_isr_disable();
        return -1;
    }
}

/**************************************************************************************************
 * 
 * hapi_isr_disable()
 * 
 *************************************************************************************************/
int
hapi_isr_disable(void)
{
    DINT;

    return 0;
}

/**************************************************************************************************
 * 
 * hapi_isr_run()
 * 
 *************************************************************************************************/
__attribute__((ramfunc)) __interrupt void
hapi_isr_run(void)
{
    hal_tic();

    if (isr.run) {
        isr.run(isr.tlo);
        hapi_isr_clear();
    } else {
        hapi_isr_disable();
    }

    hal_toc();
}

/**************************************************************************************************
 * 
 * hapi_isr_2_run()
 * 
 *************************************************************************************************/
__attribute__((ramfunc)) __interrupt void
hapi_isr_2_run(void)
{
    if (isr_2.run) {
        isr_2.run(isr_2.tlo);
        hapi_isr_2_clear();
    } else {
        hapi_isr_disable();
    }
}

/**************************************************************************************************
 * 
 * hapi_isr_clear()
 * 
 *************************************************************************************************/
__attribute__((ramfunc)) int
hapi_isr_clear(void)
{
    if (_hapi.isr_clear) {
        return _hapi.isr_clear();
    } else {
        hapi_isr_disable();
        return -1;
    }
}

/**************************************************************************************************
 * 
 * hapi_isr_clear()
 * 
 *************************************************************************************************/
__attribute__((ramfunc)) int
hapi_isr_2_clear(void)
{
    if (_hapi.isr_2_clear) {
        return _hapi.isr_2_clear();
    } else {
        hapi_isr_disable();
        return -1;
    }
}

/**************************************************************************************************
 * 
 * hapi_interlock_trip()
 * 
 *************************************************************************************************/
int
hapi_interlock_trip(void)
{
    int ret = 0;

    ret |= dio_write(map.trip, true);

    clk_delay(5U);

    ret |= dio_write(map.trip, false);

    return (ret < 0) ? -1 : 0;
}

/**************************************************************************************************
 * 
 * hapi_interlock_clear()
 * 
 *************************************************************************************************/
int
hapi_interlock_clear(void)
{
    int ret = 0;

    ret |= dio_write(map.clear, false);

    clk_delay(5U);

    ret |= dio_write(map.clear, true);

    clk_delay(5U);

    ret |= dio_write(map.clear, false);

    return (ret < 0) ? -1 : 0;
}



/**************************************************************************************************
 * 
 * hapi_interlock_internal_read()
 * 
 *************************************************************************************************/
__attribute__((ramfunc))  int
hapi_interlock_internal_read(bool *internal)
{

    ASSERT(internal);
    static bool use_internal = false; 
    static bool use_combined = false; 
    static uint32_t pin_internal;
    static uint32_t pin_combined;

 
    if(!use_internal && !use_combined) {
        //check if we can use internal 
        enum io_fun fun_internal;
        bool status_internal = io_status(map.internal, &pin_internal, &fun_internal, NULL);

        if (status_internal && (fun_internal == IO_DIN || fun_internal == IO_DOUT)) {
            //status is ok so we can use internal 
            use_internal = true;

        } 
        else{
            //check if we can use combined intead
            enum io_fun fun_combined;
            bool status_combined = io_status(map.combined, &pin_combined, &fun_combined, NULL);
                
            if (status_combined && (fun_combined == IO_DIN || fun_combined == IO_DOUT)) {
                //status is ok so we can use internal 
                use_combined = true;
            } 
        } 
    } 


    if(use_internal){
        *internal = GPIO_readPin(pin_internal) != 1;
        /* Invert hardware logic so that high level means active fault state */

    } 


    else if(use_combined){
        *internal = GPIO_readPin(pin_combined) != 1;
        /* Invert hardware logic so that high level means active fault state */
    }
    
    else{
        *internal = true; 
        return -1; 
    }

    return 0;
}


/**************************************************************************************************
 * 
 * hapi_interlock_read()
 * 
 *************************************************************************************************/
int
hapi_interlock_read(bool *internal, bool *external)
{
    ASSERT(internal && external);

    int ret;

    bool combined;
    ret = dio_read(map.combined, &combined);
    if (ret < 0) {
        *internal = true;
        *external = true;
        return -1;
    }

    /* Invert hardware logic so that high level means active fault state */
    combined = !combined;

    /**
     * Internal interlock signal is not wired properly on some boards, i.e. instantaneous signal
     * is wired instead of latched signal. The following code snippet will ensure that interlock
     * signals are updated in best way possible considering the described board bug.
     */
    *internal = combined;
    *external = combined;

    ret = dio_read(map.internal, internal);
    if (ret < 0) {
        return 0;
    }

    /* Invert hardware logic so that high level means active fault state */
    *internal = !(*internal);

    /**
     * Combined interlock signal is logical OR between internal and external interlock signals:
     * 
     *      INT EXT | CMB   ->   INT CMB | EXT
     *      --------|----        --------|----
     *       0   0  |  0          0   0  |  0
     *       0   1  |  1          0   1  |  1
     *       1   0  |  1          1   1  |  0
     *       1   1  |  1          1   1  |  1(#)
     * 
     * The logic table above (on the right) shows that internal and combined signals can uniquely
     * determine external signal only when internal signal is false. When internal signal is true,
     * external signal could be either true or false. (#) In all practical scenarios, chances that
     * both internal and external signals become high at the same time are very small.
     */
    *external = *internal ? false : combined;

    return 0;
}

/**************************************************************************************************
 * 
 * hapi_adc_data()
 * 
 *************************************************************************************************/
int
hapi_adc_data(const struct adc *adc)
{
    ASSERT(_hapi.adc && adc);

    return (_hapi.adc_data && adc) ? _hapi.adc_data(adc) : -1;
}

/**************************************************************************************************
 * 
 * hapi_adc_read()
 * 
 *************************************************************************************************/
__attribute__((ramfunc)) int
hapi_adc_read(const struct adc *adc)
{
    ASSERT(_hapi.adc_read && adc);

    return (_hapi.adc_read && adc) ? _hapi.adc_read(adc) : -1;
}

/**************************************************************************************************
 * 
 * hapi_wch_data
 * 
 *************************************************************************************************/
int
hapi_wch_data(const struct wch *wch)
{
    ASSERT(_hapi.wch_data && wch);

    return (_hapi.wch_data && wch) ? _hapi.wch_data(wch) : -1;
}

/**************************************************************************************************
 * 
 * hapi_wch_set
 * 
 *************************************************************************************************/
int
hapi_wch_set(const struct wch *wch)
{
    ASSERT(_hapi.wch_set && wch);

    return (_hapi.wch_set && wch) ? _hapi.wch_set(wch) : -1;
}

/**************************************************************************************************
 * 
 * hapi_wch_get
 * 
 *************************************************************************************************/
int
hapi_wch_get(const struct wch *wch)
{
    ASSERT(_hapi.wch_get && wch);

    return (_hapi.wch_get && wch) ? _hapi.wch_get(wch) : -1;
}

/**************************************************************************************************
 * 
 * hapi_wch_clear
 * 
 *************************************************************************************************/
int
hapi_wch_clear(const struct wch *wch)
{
    ASSERT(_hapi.wch_clear && wch);

    return (_hapi.wch_clear && wch) ? _hapi.wch_clear(wch) : -1;
}

/**************************************************************************************************
 * 
 * hapi_wch_clear
 * 
 *************************************************************************************************/
int
hapi_wch_calibration_mode(const struct wch *wch, bool enable)
{
    ASSERT(_hapi.wch_calib_mode && wch);

    return (_hapi.wch_calib_mode && wch) ? _hapi.wch_calib_mode(wch, enable) : -1;
}

/**************************************************************************************************
 * 
 * hapi_wcs_data()
 * 
 *************************************************************************************************/
int
hapi_wcs_data(const struct wcs *wcs)
{
    ASSERT(_hapi.wcs_data && wcs);

    return (_hapi.wcs_data && wcs) ? _hapi.wcs_data(wcs) : -1;
}

/**************************************************************************************************
 * 
 * hapi_pwm_update()
 * 
 *************************************************************************************************/
__attribute__((ramfunc)) int
hapi_pwm_update(const struct pwm *pwm)
{
    ASSERT(_hapi.pwm_update && pwm);

    return (_hapi.pwm_update && pwm) ? _hapi.pwm_update(pwm) : -1;
}

/**************************************************************************************************
 * 
 * hapi_get_cpu_busy_time()
 * 
 *************************************************************************************************/
unsigned int
hapi_get_cpu_busy_time(void)
{
    return _hapi.isr_cpu_tics;
}

/**************************************************************************************************
 * 
 * hapi_get_isr_status_flag()
 * 
 *************************************************************************************************/
bool
hapi_get_isr_status_flag()
{
    return isr.isr_status;
}
