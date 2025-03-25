/**************************************************************************************************
 * 
 * \file hapi.h
 * 
 * \brief Hardware application interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _APP_HAPI_H
#define _APP_HAPI_H

#include "inc/drv/adc.h"
#include "inc/drv/io.h"
#include "inc/drv/pwm.h"
#include "inc/drv/ecap.h"
#include "inc/hal/hapi.h"

#include <stdbool.h>


/**************************************************************************************************
 * 
 * Application layer pin map definition for all LF45 revisions
 * 
 *************************************************************************************************/
struct hapi_map {
    const enum io mcu_led_1;             
    const enum io mcu_led_2;

    const enum io coding_a;             
    const enum io coding_b;
    const enum io screen_rst_n;
    const enum io screen_d_c;

    const enum io spi_simo;             
    const enum io spi_somi;
    const enum io spi_cs;
    const enum io spi_sck;            
    const enum io interlock_read;
    const enum io interlock_write;


    const enum io  button0;
    const enum io  button1;
    const enum io  button2;
    const enum io  button3;
    const enum io button_cw;


    const enum io  led_b0;
    const enum io  led_b1;
    const enum io  led_b2;
    const enum io  led_b3_g;
    const enum io  led_b3_r;
};

   


/**************************************************************************************************
 * 
 * ADC object (driver)
 * 
 *************************************************************************************************/
_ADC_OBJ_STRUCT(
    _ADC_OBJ_STRUCT_MEMBER(button0);   /* Input-side port DC bus voltage                        */
    _ADC_OBJ_STRUCT_MEMBER(button1);   /* Input-side port DC bus voltage                        */
    _ADC_OBJ_STRUCT_MEMBER(button2);   /* Input-side port DC bus voltage                        */
    _ADC_OBJ_STRUCT_MEMBER(button3);          /* Bar temperature                            *rev.0* */
    _ADC_OBJ_STRUCT_MEMBER(button_cw);          /* Bar temperature                            *rev.0* */


);

/**************************************************************************************************
 * 
 * PWM object (driver)
 * 
 *************************************************************************************************/
_PWM_OBJ_STRUCT(
    _PWM_OBJ_STRUCT_MEMBER(pwm_adc);        /* ADC start of conversion trigger                      */

);



/**************************************************************************************************
 * 
 * Hardware application interface object definition
 * 
 *************************************************************************************************/
struct hapi {
    struct hapi_map * const map;
    struct _adc * const adc;
    struct _pwm * const pwm;
    struct _ecap * const ecap;
    void (*toggle_led_1)(void);
    void (*toggle_led_2)(void);
    void (*enable_led_2)(bool status);
    const struct net *spi_net;
    int (*enable_spi_interface)(bool enable);
    void (*enable_screen_d_c)(bool enable);

    bool (*read_coding_a)(void);
    bool (*read_coding_b)(void);
    
    bool (*read_button0)(void);
    bool (*read_button1)(void);
    bool (*read_button2)(void);
    bool (*read_button3)(void);
    bool (*read_button_cw)(void);


    void (*enable_led_b0)(bool enable);
    void (*enable_led_b1)(bool enable);
    void (*enable_led_b2)(bool enable);
    void (*enable_led_b3_r)(bool enable);
    void (*enable_led_b3_g)(bool enable);



    bool (*read_interlock)(void);
    int (*delay)(uint16_t microsec);
    int (*delay_ms)(uint16_t millisec);

};

/**************************************************************************************************
 * 
 * Prototypes for public functions used to resolve hardware application interface for different
 * hardware revisions
 * 
 *************************************************************************************************/

extern int hapi_resolve_rev0(struct _hapi *_hapi, unsigned variant);
extern int hapi_resolve_rev1(struct _hapi *_hapi, unsigned variant);


/**************************************************************************************************
 * 
 * \brief Introduce a delay (not super accurate as it will have some overhead).
 * 
 * \param microsec Desired delay in microseconds
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int hapi_delay(uint16_t microsec);
extern int hapi_delay_ms(uint16_t millisec);

extern void hapi_toggle_led_1(void);
extern void hapi_toggle_led_2(void);
extern void hapi_enable_led_2(bool status);
extern void hapi_enable_screen_d_c(bool status);
extern bool hapi_read_coding_a(void);
extern bool hapi_read_interlock(void);
extern bool hapi_read_coding_b(void);
extern int hapi_enable_spi_interface(bool enable);


extern bool hapi_read_button0(void);
extern bool hapi_read_button1(void);
extern bool hapi_read_button2(void);
extern bool hapi_read_button3(void);
extern bool hapi_read_button_cw(void);

extern void hapi_enable_led_b0(bool status);
extern void hapi_enable_led_b1(bool status);
extern void hapi_enable_led_b2(bool status);
extern void hapi_enable_led_b3_r(bool status);
extern void hapi_enable_led_b3_g(bool status);



#endif /* _APP_HAPI_H */
