/**************************************************************************************************
 * 
 * \file hapi.h
 * 
 * \brief Hardware application interface abstraction
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_HAL_HAPI_H
#define _INC_HAL_HAPI_H

#include "cgen/hal/hapi.h"

#include "inc/drv/io.h"

#include <stdint.h>
#include <stdbool.h>

/**************************************************************************************************
 * 
 * Forward declarations
 * 
 *************************************************************************************************/

struct hapi;

struct nfo;
struct tlo;

struct adc;
struct wch;
struct wcs;
struct pwm;

/**************************************************************************************************
 * 
 * Definition of pin map common to all ADVANTICS power converters
 * 
 *************************************************************************************************/
struct _hapi_map {
    enum io trip;                           /* Interlock trip control line                      */
    enum io clear;                          /* Interlock clear control line                     */
    enum io internal;                       /* Internal interlock status line                   */
    enum io combined;                       /* Combined interlock status line                   */
};

/**************************************************************************************************
 * 
 * Definition for interrupt service routine data
 * 
 *************************************************************************************************/
struct _hapi_isr {
    void (*run)(const struct tlo *tlo);     /* Callback for interrupt service routine           */
    const struct tlo *tlo;                  /* Top-level object handler                         */
    bool isr_status;
};

/**************************************************************************************************
 * 
 * Hardware application interface object definition
 * 
 *************************************************************************************************/
struct _hapi {
    struct _hapi_map * const map;
    struct _hapi_isr * const isr;
    struct _hapi_isr * const isr_2;
    int n_int;      /* This is the number of interrupts to be used by the appliation (1 or 2)   */
    uint32_t isr_cpu_tics;  /* This is the number of CPU clock tics spent inside the interrupt  */
    struct hapi *hapi;
    int (*setup)(void);
    int (*isr_clear)(void);
    int (*isr_2_clear)(void);
    int (*adc_data)(const struct adc *adc);
    int (*adc_read)(const struct adc *adc);
    int (*wch_data)(const struct wch *wch);
    int (*wch_set)(const struct wch *wch);
    int (*wch_get)(const struct wch *wch);
    int (*wch_clear)(const struct wch *wch);
    int (*wch_calib_mode)(const struct wch *wch, bool enable);
    int (*wcs_data)(const struct wcs *wcs);
    int (*pwm_update)(const struct pwm *pwm);
};

/**************************************************************************************************
 * 
 * Prototypes for private functions used in dynamic linkage. These functions are implemented in
 * the application layer.
 * 
 *************************************************************************************************/

static int _hapi_setup(void);
static int _hapi_isr_clear(void);
static int _hapi_isr_2_clear(void);
static int _hapi_adc_data(const struct adc *adc);
static int _hapi_adc_read(const struct adc *adc);
static int _hapi_wch_data(const struct wch *wch);
static int _hapi_wch_get(const struct wch *wch);
static int _hapi_wch_set(const struct wch *wch);
static int _hapi_wch_clear(const struct wch *wch);
static int _hapi_wch_calib_mode(const struct wch *wch, bool enable);
static int _hapi_wcs_data(const struct wcs *wcs);
static int _hapi_pwm_update(const struct pwm *pwm);

/**************************************************************************************************
 * 
 * \brief Setups hardware application interface
 * 
 * \param mod module data object handler
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
hapi_setup(const struct nfo *mod);

/**************************************************************************************************
 * 
 * \brief Resolves hardware application interface in application layer
 * 
 * \param _hapi HAPI object handler
 * \param mod module data object handler
 * 
 * \note This function is implemented in the application layer.
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
hapi_resolve(struct _hapi *_hapi, const struct nfo *mod);

/**************************************************************************************************
 * 
 * \brief Registers interrupt service routine callback
 * 
 * \param run run function handler
 * \param tlo top-level object handler
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
hapi_isr_register(void (*run)(const struct tlo *tlo), const struct tlo *tlo);

/**************************************************************************************************
 * 
 * \brief Registers interrupt service routine callback
 * 
 * \param run run function handler
 * \param tlo top-level object handler
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
hapi_isr_2_register(void (*run)(const struct tlo *tlo), const struct tlo *tlo);

/**************************************************************************************************
 * 
 * \brief Enables interrupt service routine
 * 
 * \param None
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
hapi_isr_enable(void);

/**************************************************************************************************
 * 
 * \brief Disables interrupt service routine
 * 
 * \param None
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
hapi_isr_disable(void);

/**************************************************************************************************
 * 
 * \brief Calls main interrupt service routine
 * 
 * \param None
 * 
 * \return None
 * 
 *************************************************************************************************/
extern __interrupt void
hapi_isr_run(void);

/**************************************************************************************************
 * 
 * \brief Calls secondary interrupt service routine
 * 
 * \param None
 * 
 * \return None
 * 
 *************************************************************************************************/
extern __interrupt void
hapi_isr_2_run(void);

/**************************************************************************************************
 * 
 * \brief Clears interrupt trigger flag
 * 
 * \param None
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
hapi_isr_clear(void);

/**************************************************************************************************
 * 
 * \brief Clears secondary interrupt trigger flag
 * 
 * \param None
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
hapi_isr_2_clear(void);

/**************************************************************************************************
 * 
 * \brief Triggers interlock trip condition
 * 
 * \param None
 * 
 * \note Trip condition is imposed by high level on the corresponding output pin.
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
hapi_interlock_trip(void);

/**************************************************************************************************
 * 
 * \brief Clears interlock trip condition
 * 
 * \param None
 * 
 * \note Interlock is cleared by rising edge on the corresponding output pin. Interlock cannot be
 * cleared if application layer is not completely configured.
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
hapi_interlock_clear(void);



/**************************************************************************************************
 * 
 * \brief Gets internal interlock flags
 * 
 * \param internal storage for internal interlock flag
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
hapi_interlock_internal_read(bool *internal);

/**************************************************************************************************
 * 
 * \brief Gets internal and external interlock flags
 * 
 * \param internal storage for internal interlock flag
 * \param external storage for external interlock flag
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
hapi_interlock_read(bool *internal, bool *external);

/**************************************************************************************************
 * 
 * \brief Initializes default ADC conversion data
 * 
 * \param adc ADC object handler
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
hapi_adc_data(const struct adc *adc);

/**************************************************************************************************
 * 
 * \brief Updates raw measurements in ADC object
 * 
 * \param adc ADC object handler
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
hapi_adc_read(const struct adc *adc);

/**************************************************************************************************
 * 
 * \brief Initializes default WCH conversion data
 * 
 * \param wch WCH object handler
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
hapi_wch_data(const struct wch *wch);

/**************************************************************************************************
 * 
 * \brief Initializes default WCH conversion data
 * 
 * \param wch WCH object handler
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
hapi_wch_get(const struct wch *wch);

/**************************************************************************************************
 * 
 * \brief Initializes default WCH conversion data
 * 
 * \param wch WCH object handler
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
hapi_wch_set(const struct wch *wch);

/**************************************************************************************************
 * 
 * \brief Initializes default WCH conversion data
 * 
 * \param wch WCH object handler
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
hapi_wch_clear(const struct wch *wch);

/**************************************************************************************************
 * 
 * \brief Initializes default hardware limits in WCS object
 * 
 * \param wcs WCS object handler
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
hapi_wcs_data(const struct wcs *wcs);

extern int
hapi_wch_calibration_mode(const struct wch *wch, bool enable);

/**************************************************************************************************
 * 
 * \brief Updates PWM generators (duty cycle, dead time etc.)
 * 
 * \param pwm PWM object handler
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
hapi_pwm_update(const struct pwm *pwm);

/**************************************************************************************************
 * 
 * \brief Used to get CPU interrupt clock counts
 * 
 * \return unsigned integer with the number of CPU clocks spent in the ISR
 * 
 *************************************************************************************************/
unsigned int
hapi_get_cpu_busy_time(void);

/**************************************************************************************************
 *
 * \brief Used to get status of ISR if its enabled for the first time in application or not
 *
 * \return bool with the status of the ISR
 *
 *************************************************************************************************/
bool
hapi_get_isr_status_flag();

#endif /* _INC_HAL_HAPI_H */
