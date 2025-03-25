/**************************************************************************************************
 * 
 * \file hapi.c
 * 
 * \brief Hardware application interface abstraction
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "app/hapi.h"

#include "inc/lib/nfo.h"
#include "inc/drv/dio.h"
#include <stddef.h>


/**************************************************************************************************
 * 
 * Application layer pin map
 * 
 *************************************************************************************************/
struct hapi_map map = {
    
    /*
    .mcu_led_2      = IO8_DOUT,
    .mcu_led_1      = IO28_DOUT,
    .coding_a       = IO10_DIN,
    .coding_b       = IO29_DIN,
    .screen_rst_n  = IO3_DOUT,
    .screen_d_c    = IO6_DOUT,

    .spi_simo      = IO16_SPIA_SIMO,
    .spi_somi      = IO17_SPIA_SOMI,
    .spi_cs        = IO11_SPIA_STE,  
    .spi_sck       = IO9_SPIA_CLK,  

    .interlock_write  = IO24_DOUT,  
    .interlock_read   = IO7_DIN,  
    */

    .mcu_led_2      = IO72_DOUT,
    .mcu_led_1      = IO84_DOUT,
    .coding_a       = IO10_DIN,
    .coding_b       = IO11_DIN,

    .screen_rst_n  = IO62_DOUT,
    .screen_d_c    = IO63_DOUT,

    .spi_simo      = IO58_SPIA_SIMO,
    .spi_somi      = IO59_SPIA_SOMI,
    .spi_cs        = IO61_SPIA_STE,  
    .spi_sck       = IO60_SPIA_CLK,  

    .interlock_write  = IO43_DOUT,  
    .interlock_read   = IO69_DIN,  


    .button0  = IO15_DIN,
    .button1  = IO14_DIN,
    .button2  = IO13_DIN, 
    .button3  = IO12_DIN,
    .button_cw  = IO100_DIN,

    .led_b0    = IO207_DOUT,
    .led_b1    = IO209_DOUT,
    .led_b2    = IO213_DOUT,
    .led_b3_g  = IO205_DOUT,
    .led_b3_r  = IO203_DOUT




};


/**************************************************************************************************
 * 
 * ADC object (driver)
 * 
 *************************************************************************************************/
_ADC_VAR_NEW(button0);     /* Input-side port DC bus voltage                        */
_ADC_VAR_NEW(button1);     /* Input-side port DC bus voltage                        */
_ADC_VAR_NEW(button2);     /* Input-side port DC bus voltage                        */
_ADC_VAR_NEW(button3);            /* Bar temperature                              *rev.0* */
_ADC_VAR_NEW(button_cw);


_ADC_OBJ_NEW(
    _ADC_MEMBER_SET(button0),
    _ADC_MEMBER_SET(button1),
    _ADC_MEMBER_SET(button2),
    _ADC_MEMBER_SET(button3),
    _ADC_MEMBER_SET(button_cw),
);


/**************************************************************************************************
 * 
 * PWM object (driver)
 * 
 *************************************************************************************************/

_PWM_VAR_NEW(pwm_adc);

_PWM_OBJ_NEW(
    _PWM_MEMBER_SET(pwm_adc),
);

/**************************************************************************************************
 * 
 * Hardware application interface - application object
 * 
 *************************************************************************************************/
static struct hapi hapi = {
    .map       = &map,
    .adc       = &adc,
    .pwm      =  &pwm,
    .spi_net    = NULL,
};



/**************************************************************************************************
 * 
 * hapi_resolve()
 * 
 *************************************************************************************************/
int
hapi_resolve(struct _hapi *_hapi, const struct nfo *mod)
{
    int ret;

    /**
     * Configure pins. Default pin state
     * after configuration is \b low.
     * CAUTION: These pins should never change between hardware revisions!
     */

    ret = io_connect(map.interlock_write, IO_DOUT);
    if (ret < 0) {
        return -1;
    }
    dio_write(map.interlock_write, false);  // setting it to off on boot


    ret = io_connect(map.interlock_read, IO_DIN);
    if (ret < 0) {
        return -1;
    }

    

    io_disconnect(map.mcu_led_1);
    ret = io_connect(map.mcu_led_1, IO_DOUT);
    if (ret < 0) {
        return -1;
    }
    dio_write(map.mcu_led_1, true);  // setting it to off on boot

    io_disconnect(map.mcu_led_2);
    ret = io_connect(map.mcu_led_2, IO_DOUT);
    if (ret < 0) {
        return -1;
    }
    dio_write(map.mcu_led_2, false);  // setting it to off on boot


    ret = io_connect(map.coding_a, IO_DIN);
    if (ret < 0) {
        return -1;
    }

    ret = io_connect(map.coding_b, IO_DIN);
    if (ret < 0) {
        return -1;
    }


    ret = io_connect(map.button0, IO_DIN);
    if (ret < 0) {
        return -1;
    }
    ret = io_connect(map.button1, IO_DIN);
    if (ret < 0) {
        return -1;
    }
    ret = io_connect(map.button2, IO_DIN);
    if (ret < 0) {
        return -1;
    }
    ret = io_connect(map.button3, IO_DIN);
    if (ret < 0) {
        return -1;
    }
    ret = io_connect(map.button_cw, IO_DIN);
    if (ret < 0) {
        return -1;
    }



    ret = io_connect(map.screen_rst_n, IO_DOUT);
    if (ret < 0) {
        return -1;
    }
    dio_write(map.screen_rst_n, false);  // setting it to off on boot

    ret = io_connect(map.screen_d_c, IO_DOUT);
    if (ret < 0) {
        return -1;
    }
    dio_write(map.screen_d_c, true);  // setting it to off on boot


    ret = io_connect(map.led_b0, IO_DOUT);
     if (ret < 0) {
        return -1;
    }
    ret = dio_write(map.led_b0, true);  // setting it to off on boot


    ret = io_connect(map.led_b1, IO_DOUT);
    if (ret < 0) {
        return -1;
    }
    dio_write(map.led_b1, true);  // setting it to off on boot


    ret = io_connect(map.led_b2, IO_DOUT);
    if (ret < 0) {
        return -1;
    }
    dio_write(map.led_b2, true);  // setting it to off on boot


    ret = io_connect(map.led_b3_g, IO_DOUT);
    if (ret < 0) {
        return -1;
    }
    dio_write(map.led_b3_g, true);  // setting it to off on boot
        
 
    ret = io_connect(map.led_b3_r, IO_DOUT);
    if (ret < 0) {
        return -1;
    }
    dio_write(map.led_b3_r, true);  // setting it to off on boot







    _hapi->hapi = &hapi;

    switch (mod->revision) {

    case 0U:
        ret = hapi_resolve_rev0(_hapi, mod->variant);
        break;
    case 1U:
        ret = hapi_resolve_rev1(_hapi, mod->variant);
        break;
    default:
        return -1;
    }

    return (ret < 0) ? -1 : 0;
}


__attribute__((ramfunc)) 
int hapi_delay(uint16_t microsec)
{
    ASSERT(hapi.delay);
    return hapi.delay ? hapi.delay(microsec) : -1;   
}

__attribute__((ramfunc)) 
int hapi_delay_ms(uint16_t millisec)
{
    int rtn = 0;
    int i  =  0 ;
    for (i =0 ; i <  millisec; i++){
        rtn = hapi_delay(1000) ;
        if(rtn < 0){
            return -1;
        }
    }
    return 0;   
}




bool hapi_read_interlock(void)
{
    return hapi.read_interlock();
}

bool hapi_read_coding_a(void)
{
    return hapi.read_coding_a();
}


bool hapi_read_coding_b(void)
{
    return hapi.read_coding_b();
}


void hapi_toggle_led_1(void)
{
    hapi.toggle_led_1();
}


void hapi_toggle_led_2(void)
{
    hapi.toggle_led_2();
}



void hapi_enable_led_2(bool status)
{
    hapi.enable_led_2(status);
}



void hapi_enable_led_b0(bool status)
{
        hapi.enable_led_b0(status);
}


void hapi_enable_led_b1(bool status)
{
        hapi.enable_led_b1(status);
}


void hapi_enable_led_b2(bool status)
{
        hapi.enable_led_b2(status);
}


void hapi_enable_led_b3_r(bool status)
{
        hapi.enable_led_b3_r(status);
}


void hapi_enable_led_b3_g(bool status)
{
        hapi.enable_led_b3_g(status);
}






void hapi_enable_screen_d_c(bool status)
{
    hapi.enable_screen_d_c(status);
}




int hapi_enable_spi_interface(bool enable)
{
    return hapi.enable_spi_interface(enable);
}



bool hapi_read_button0(void){
      return hapi.read_button0();
}

bool hapi_read_button1(void){
      return hapi.read_button1();
}

bool hapi_read_button2(void){
      return hapi.read_button2();
}

bool hapi_read_button3(void){
      return hapi.read_button3();
}

bool hapi_read_button_cw(void){
      return hapi.read_button_cw();
}


