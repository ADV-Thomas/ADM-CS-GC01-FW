/**************************************************************************************************
 * 
 * \file hapi.c
 * 
 * \brief Hardware application interface (rev. 0)
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "app/hapi.h"
#include "app/user.h"
#include "app/adc.h"
#include "app/ctl.h"
#include "app/wcs.h"

#include "inc/drv/pie.h"
#include "inc/drv/pwm.h"
#include "inc/drv/io.h"
#include "inc/drv/dio.h"
#include "inc/drv/ecap.h"
#include "inc/lib/debug.h"
#include "inc/hal/hal.h"

#include "inc/drv/spi.h"
#include "inc/net/spi.h"

#include "inc/drv/clk.h"

#include <stdint.h>
#include <stddef.h>

static int
_hapi_adc_data_var0(const struct adc *adc);
static int
_hapi_wcs_data_var0(const struct wcs *wcs);
static int
_hapi_delay(uint16_t microsec);
static void 
_hapi_toggle_led_1(void);
static void
_hapi_toggle_led_2(void);
static void
_hapi_enable_led_2(bool status);
static bool
_hapi_read_coding_a(void);
static bool
_hapi_read_coding_b(void);
static bool
_hapi_read_interlock(void);
static int
_hapi_enable_spi_interface(bool enable);
static void
_hapi_enable_screen_d_c(bool status);

static bool _hapi_read_button0(void);
static bool _hapi_read_button1(void);
static bool _hapi_read_button2(void);
static bool _hapi_read_button3(void);
static bool _hapi_read_button_cw(void);

static void  _hapi_enable_led_b0(bool status);
static void  _hapi_enable_led_b1(bool status);
static void  _hapi_enable_led_b2(bool status);
static void  _hapi_enable_led_b3_r(bool status);
static void  _hapi_enable_led_b3_g(bool status);



/**************************************************************************************************
 * 
 * Hardware application interface object handler
 * 
 *************************************************************************************************/

static struct hapi *hapi = NULL;

/**************************************************************************************************
 * 
 * hapi_resolve_rev0()
 * 
 *************************************************************************************************/
int
hapi_resolve_rev0(struct _hapi *_hapi, unsigned variant)
{
    ASSERT(_hapi);

    hapi = _hapi->hapi;

    // Interlock signals are not used in this board
    _HAPI_MAP_INIT(trip, IOX);
    _HAPI_MAP_INIT(clear, IOX);
    _HAPI_MAP_INIT(internal, IOX);
    _HAPI_MAP_INIT(combined, IOX);
    
  
    
    if (variant > 2U) {
        return -1;
    }


    else if(variant == 0U){
        _PWM_VAR_INIT(pwm_adc, 1U, IOX, IOX, false, C_FS, false, 0.0f, 0.0f);

        //TRIG_EPWM1_SOCA_N0

        _ADC_VAR_INIT(button0, 3U, ADC_CH_ADCIN0,  ADC_TRIGGER_EPWM1_SOCA, 300U, 0u, 0u);
        _ADC_VAR_INIT(button1, 3U, ADC_CH_ADCIN2,  ADC_TRIGGER_EPWM1_SOCA, 300U, 0U, 1u);
        _ADC_VAR_INIT(button2, 1U, ADC_CH_ADCIN1,  ADC_TRIGGER_EPWM1_SOCA, 300U, 0U, 2u);
        _ADC_VAR_INIT(button3, 1U, ADC_CH_ADCIN0,  ADC_TRIGGER_EPWM1_SOCA, 300U, 1u, 3u);
        _ADC_VAR_INIT(button_cw, 2U, ADC_CH_ADCIN4,  ADC_TRIGGER_EPWM1_SOCA, 300U, 0U, 4u);

    }


    else if(variant == 1){


    }



    
    _hapi->setup      = _hapi_setup;
    _hapi->isr_clear  = _hapi_isr_clear;
    _hapi->adc_data   = _hapi_adc_data_var0;
    _hapi->adc_read   = _hapi_adc_read;
    _hapi->wch_data   = NULL;
    _hapi->wcs_data   = _hapi_wcs_data_var0;
    _hapi->pwm_update = _hapi_pwm_update;
    
    hapi->delay       = _hapi_delay;
    
    hapi->toggle_led_1 = _hapi_toggle_led_1;
    hapi->toggle_led_2 = _hapi_toggle_led_2;
    hapi->enable_led_2 = _hapi_enable_led_2;

    hapi->enable_screen_d_c = _hapi_enable_screen_d_c;
    hapi->read_coding_a = _hapi_read_coding_a;
    hapi->read_coding_b = _hapi_read_coding_b;
    hapi->read_interlock = _hapi_read_interlock;

    hapi->enable_spi_interface = _hapi_enable_spi_interface;

    hapi->read_button0 =   _hapi_read_button0;
    hapi->read_button1 =   _hapi_read_button1;
    hapi->read_button2 =   _hapi_read_button2;
    hapi->read_button3 =   _hapi_read_button3;
    hapi->read_button_cw =   _hapi_read_button_cw;


    hapi->enable_led_b0 = _hapi_enable_led_b0;
    hapi->enable_led_b1 = _hapi_enable_led_b1;
    hapi->enable_led_b2 = _hapi_enable_led_b2;
    hapi->enable_led_b3_r = _hapi_enable_led_b3_r;
    hapi->enable_led_b3_g = _hapi_enable_led_b3_g;


    

    
    return 0;
}

/**************************************************************************************************
 * 
 * _hapi_setup()
 * 
 *************************************************************************************************/
static int
_hapi_setup(void)
{
    int ret;

    
    ret = pwm_setup(hapi->pwm);
    if (ret < 0) {
        return -1;
    }


    ret = adc_setup(hapi->adc);
    if (ret < 0) {
        return -1;
    }
    /*
    ret = ecap_setup(hapi->ecap);
    if (ret < 0) {
        return -1;
    }*/


    pwm_trigger(EPWM1_BASE, EPWM_SOC_A, EPWM_SOC_TBCTR_ZERO, C_ISR_DIVIDER);

     pie_register(INT_ADCA1, hapi_isr_run);


    return 0;
}


/**************************************************************************************************
 * 
 * _hapi_isr_clear()
 * 
 *************************************************************************************************/
__attribute__((ramfunc)) static int
_hapi_isr_clear(void)
{


    pie_clear(INT_ADCA1);
    ADC_clearInterruptStatus(ADCA_BASE,ADC_INT_NUMBER1);

    

    return 0;
}

/**************************************************************************************************
 * 
 * _hapi_adc_data()
 * 
 *************************************************************************************************/
static int
_hapi_adc_data_var0(const struct adc *adc)
{
    ADC_SET_CONV(button0, 4096U, 3.3f, 0u);
    ADC_SET_CONV(button1, 4096U, 3.3f, 0u);
    ADC_SET_CONV(button2, 4096U, 3.3f, 0u);
    ADC_SET_CONV(button3, 4096U, 3.3f, 0u);
    ADC_SET_CONV(button_cw, 4096U, 3.3f, 0u);

    return 0;
}

/**************************************************************************************************
 * 
 * _hapi_adc_read()
 * 
 *************************************************************************************************/
__attribute__((ramfunc)) static int
_hapi_adc_read(const struct adc *adc)
{

    _ADC_READ_1(button0);
    _ADC_READ_1(button1);
    _ADC_READ_1(button2);
    _ADC_READ_1(button3);
    _ADC_READ_1(button_cw);
    return 0;
}

/**************************************************************************************************
 * 
 * _hapi_adc_read()
 * 
 *************************************************************************************************/
__attribute__((ramfunc)) static int
_hapi_wcs_data_var0(const struct wcs *wcs)
{
    return 0;
}





/**************************************************************************************************
 * 
 * _hapi_adc_read()
 * 
 *************************************************************************************************/
__attribute__((ramfunc)) static int
 _hapi_pwm_update(const struct pwm *pwm)
 {
    uint16_t c_period = EPWM_getTimeBasePeriod(EPWM1_BASE);
    return 0;
 }



/**************************************************************************************************
 * 
 * _hapi_delay()
 * 
 *************************************************************************************************/
__attribute__((ramfunc)) static int
_hapi_delay(uint16_t microsec)
{
    clk_delay(microsec);

    return 0;
}

void _hapi_toggle_led_1(void)
{
    dio_toggle(hapi->map->mcu_led_1);
}

void _hapi_toggle_led_2(void)
{
    dio_toggle(hapi->map->mcu_led_2);
}



void _hapi_enable_led_2(bool status)
{    
    dio_write(hapi->map->mcu_led_2,status);
}



void _hapi_enable_led_b0(bool status)
{    
    dio_write(hapi->map->led_b0,status);
}


void _hapi_enable_led_b1(bool status)
{    
    dio_write(hapi->map->led_b1,status);
}




void _hapi_enable_led_b2(bool status)
{    
    dio_write(hapi->map->led_b2,status);
}


void _hapi_enable_led_b3_r(bool status)
{    
    dio_write(hapi->map->led_b3_r,status);
}


void _hapi_enable_led_b3_g(bool status)
{    
    dio_write(hapi->map->led_b3_g,status);
}





void _hapi_enable_screen_d_c(bool status)
{    
    dio_write(hapi->map->screen_d_c,status);
}




bool _hapi_read_button0(void)
{   
    bool state;
    dio_read(hapi->map->button0, &state);
    return state;
}


bool _hapi_read_button1(void)
{   
    bool state;
    dio_read(hapi->map->button1, &state);
    return state;
}




bool _hapi_read_button2(void)
{   
    bool state;
    dio_read(hapi->map->button2, &state);
    return state;
}


bool _hapi_read_button3(void)
{   
    bool state;
    dio_read(hapi->map->button3, &state);
    return state;
}



bool _hapi_read_button_cw(void)
{   
    bool state;
    dio_read(hapi->map->button_cw, &state);
    return state;
}



bool _hapi_read_coding_a(void)
{   
    bool state;
    dio_read(hapi->map->coding_a, &state);
    return state;
}



bool _hapi_read_coding_b(void)
{    
    bool state;
    dio_read(hapi->map->coding_b, &state);
    return state;
}


bool _hapi_read_interlock(void)
{    
    bool state;
    dio_read(hapi->map->interlock_read, &state);
    return state;
}


int _hapi_enable_spi_interface(bool enable)
{
 
    int static has_been_set = 0;
    if(has_been_set == 1){
        return 1;
    }
    if(enable){
        
        volatile int ret = 0;

        
        io_disconnect(hapi->map->spi_sck);
        io_disconnect(hapi->map->spi_simo);
        io_disconnect(hapi->map->spi_somi);
        io_disconnect(hapi->map->spi_cs);


        hapi->spi_net = hal_get_spi();
        spi_disconnect(hapi->spi_net);
        hapi->spi_net = spi_a_new();

        
        //SPI_enableTriWire(SPIA_BASE);
        SPI_enableTalk(SPIA_BASE);
        SPI_enableHighSpeedMode(SPIA_BASE);

    

        
        ret = spi_connect(hapi->spi_net, hapi->map->spi_sck, hapi->map->spi_simo , hapi->map->spi_somi,  hapi->map->spi_cs );
        if( ret < 0 ){
            return -1;
        } 


        struct spi_usr spi_usr = {
            .bitrate  = 1000000*50, //*16
            .bitcount = 8,
        };


        ret =  spi_open(hapi->spi_net, &spi_usr);
        if( ret < 0 ){
            return -2;
        } 


        dio_write(hapi->map->screen_rst_n, false); //0
        hapi_delay_ms(500);   
        dio_write(hapi->map->screen_rst_n, true); //1
        hapi_delay_ms(500);
        has_been_set = 1;


    }
    else {
        dio_write(hapi->map->screen_rst_n, false); //0    
        spi_disconnect(hapi->spi_net);
        has_been_set = 0;
    }

    return 0;


}

