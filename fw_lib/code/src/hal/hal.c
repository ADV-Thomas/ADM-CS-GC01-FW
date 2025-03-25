/**************************************************************************************************
 * 
 * \file hal.c
 * 
 * \brief Hardware abstraction layer implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "inc/hal/hal.h"

#include "driverlib/flash.h"
#include "driverlib/sysctl.h"

#include "inc/drv/clk.h"
#include "inc/drv/dio.h"
#include "inc/drv/eeprom.h"
#include "inc/drv/id.h"
#include "inc/drv/io.h"
#include "inc/drv/timer.h"
#include "inc/lib/nfo.h"
#include "inc/drv/flash.h"

#ifndef PLECS
#include "inc/drv/can.h"
#include "inc/net/can.h"
#endif /* PLECS */

#ifndef VIRTUAL
#include "inc/drv/i2c.h"
#include "inc/drv/spi.h"
#include "inc/net/i2c.h"
#include "inc/drv/fsi.h"
#else
#include "inc/drv/file.h"
#endif /* VIRTUAL */

#ifndef BOOTLOADER
#include "inc/hal/hapi.h"
#endif



#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**************************************************************************************************
 * 
 * CPU-specific defines for the hardware abstraction layer
 * 
 *************************************************************************************************/

#if defined(F2803X)
#define HAL_CAN_COUNT       1U              /* Number of CAN modules                            */
#define HAL_CAN_INIT        { NULL }        /* CAN driver array initializer                     */
#elif defined(F28004X)
#define HAL_CAN_COUNT       2U              /* Number of CAN modules                            */
#define HAL_CAN_INIT        { NULL, NULL }  /* CAN driver array initializer                     */
#elif defined(F28P65X)
#define HAL_CAN_COUNT       1U              /* Number of CAN modules                            */
#define HAL_CAN_INIT        { NULL }  /* CAN driver array initializer                     */
#else
#error "hal.c - dsp not defined"
#endif

/**************************************************************************************************
 * 
 * These defines would normally go to the application layer. They are put here since CAN and I2C
 * configuration is always the same. Microcontroller LED pin is the same for all modules except
 * ADM-PC-LF45 in which case it is connected to GPIO4.
 * 
 *************************************************************************************************/


#if defined(F2803X)
#define HAL_CAN_ID          0U              /* Primary CAN identifier (CAN-A)                   */
#define HAL_CAN_TX          IO31_CANA_TX    /* Primary CAN transmit pin                         */
#define HAL_CAN_RX          IO30_CANA_RX    /* Primary CAN receive pin                          */
#define HAL_MCU_LED         IO6_DOUT        /* Microcontroller LED pin                          */
#define HAL_I2C_SDA         IO32_I2CA_SDA   /* I2C serial data pin                              */
#define HAL_I2C_SCL         IO33_I2CA_SCL   /* I2C serial clock pin                             */
#elif defined(F28004X)
#define HAL_CAN_ID          1U              /* Primary CAN identifier (CAN-B)                   */
#define HAL_CAN_TX          IO12_CANB_TX    /* Primary CAN transmit pin                         */
#define HAL_CAN_RX          IO13_CANB_RX    /* Primary CAN receive pin                          */
#define HAL_MCU_LED         IO17_DOUT       /* Microcontroller LED pin                          */
#define HAL_I2C_SDA         IO32_I2CA_SDA   /* I2C serial data pin                              */
#define HAL_I2C_SCL         IO33_I2CA_SCL   /* I2C serial clock pin                             */
#elif defined(F28P65X)
#define HAL_CAN_ID          0U              /* Primary CAN identifier (MCAN-A)                   */
#define HAL_CAN_TX          IO71_CANA_TX    /* Primary CAN transmit pin                         */
#define HAL_CAN_RX          IO70_CANA_RX    /* Primary CAN receive pin                          */
#define HAL_MCU_LED         IO84_DOUT       /* Microcontroller LED pin                          */
#define HAL_I2C_SDA         IO0_I2CA_SDA   /* I2C serial data pin                              */
#define HAL_I2C_SCL         IO1_I2CA_SCL   /* I2C serial clock pin                             */
#else
#error "hal.c - dsp not defined"
#endif

/**************************************************************************************************
 * 
 * Definition of pin map common to all ADVANTICS power converters. Microcontroller LED pin is
 * resolved based on module type.
 * 
 *************************************************************************************************/
struct hal_map {
    const enum io can_tx;                   /* Primary CAN transmitter line                     */
    const enum io can_rx;                   /* Primary CAN receiver line                        */
    const enum io i2c_sda;                  /* I2C serial data line                             */
    const enum io i2c_scl;                  /* I2C serial clock line                            */
    enum io mcu_led;                        /* Microcontroller LED control line                 */
};

static struct hal_map hal_map = {
    .can_tx  = HAL_CAN_TX,
    .can_rx  = HAL_CAN_RX,
    .i2c_sda = HAL_I2C_SDA,
    .i2c_scl = HAL_I2C_SCL,
    .mcu_led = HAL_MCU_LED /* resolved based on module type */
};

/**************************************************************************************************
 * 
 * CAN abstraction data
 * 
 *************************************************************************************************/
struct hal_can {
    const uint16_t count;                   /* Number of available CAN modules                  */
    const uint16_t id;                      /* Primary CAN identifier (0=CAN_A, 1=CAN_B)        */
};

static struct hal_can hal_can = {
    .count = HAL_CAN_COUNT,
    .id    = HAL_CAN_ID
};

/**************************************************************************************************
 * 
 * HAL driver pack definition
 * 
 *************************************************************************************************/
struct hal_drv {
    const struct net *can[HAL_CAN_COUNT];   /* CAN driver handlers                              */
    const struct net *i2c;                  /* I2C driver handler for EEPROM                    */
    const struct net *file_e;               /* File driver handler for EEPROM         *VIRTUAL* */
    const struct net *file_f;               /* File driver handler for Flash          *VIRTUAL* */
    const struct net *eeprom;               /* EEPROM driver handler                            */
    const struct net *flash;                /* Flash driver handler                             */
    const struct net *spi;                  /* SPI driver handler                               */
    #if defined(F28004X) || defined(F28P65X)
    const struct net *fsi;                  /* FSI driver handler                               */
    #endif
};

static struct hal_drv hal_drv = {
    .can    = HAL_CAN_INIT,
    .i2c    = NULL,
    .file_e = NULL,
    .file_f = NULL,
    .eeprom = NULL,
    .flash  = NULL,
    .spi    = NULL,
    #if defined(F28004X) || defined(F28P65X)
    .fsi    = NULL,
    #endif
};


/**************************************************************************************************
 * 
 * CPU timer abstraction for stopwatch and task scheduler
 * 
 *************************************************************************************************/
struct hal_tmr {
    int16_t id;                             /* Identifier of the assigned CPU timer             */
    const uint16_t freq;                    /* Initialized timer frequency (Hz)                 */
    uint32_t state;                         /* Timer counter state                              */
};

static struct hal_tmr hal_swc = {
    .id = -1, /* invalid identifier */
    .freq = 0U, /* 0 Hz means maximum timer period */
    .state = 0U
};

static struct hal_tmr hal_task = {
    .id = -1, /* invalid identifier */
    .freq = 1000U, /* 1000 Hz means 1 ms tick */
    .state = 0U
};

/**************************************************************************************************
 * 
 * HAL object definition
 * 
 *************************************************************************************************/
struct hal {
    struct hal_map *map;
    struct hal_can *can;
    struct hal_drv *drv;
    struct hal_tmr *swc;
    struct hal_tmr *task;
};

static const struct hal hal = {
    .map  = &hal_map,
    .can  = &hal_can,
    .drv  = &hal_drv,
    .swc  = &hal_swc,
    .task = &hal_task,
};

static uint32_t cpu_ticks;

/**************************************************************************************************
 * 
 * hal_init()
 * 
 *************************************************************************************************/
void
hal_init(void)
{

#if defined(F2803X)
    Flash_initModule(2U, 2U, 3U);
#elif defined(F28004X) || defined(F28P65X)
    Flash_initModule(FLASH0CTRL_BASE, FLASH0ECC_BASE, 4U);
#else
#error "hal.c - dsp not defined"
#endif

    /* Setup CPU clock (60 MHz on f2803x, 100 MHz on f28004x , 200Mhz on f28p65x) */
    volatile int ret = -1;
    int trial = 3;
    while(ret < 0 && trial > 0){
        ret = clk_init();
        clk_delay(5000);
        trial --;
    }
    
    ASSERT(ret >= 0);

    id_init();

    io_reset();

#ifndef PLECS
    hal.drv->can[0] = can_a_new();
    ASSERT(hal.drv->can[0]);
#ifdef F28004X
    hal.drv->can[1] = can_b_new();
    ASSERT(hal.drv->can[1]);
#endif /* F28004X */
#endif /* PLECS */

#ifndef VIRTUAL
    hal.drv->i2c = i2c_a_new();
    #ifndef BOOTLOADER 
    hal.drv->spi = spi_a_new();
    #if defined(F28004X) || defined(F28P65X)
    hal.drv->fsi = fsi_a_new();
    #endif /* F28004X */
    #endif /* BOOTLOADER */
    ASSERT(hal.drv->i2c);
#else
    hal.drv->file_e = file_e_new();
    hal.drv->file_f = file_f_new();
    ASSERT(hal.drv->file_e && hal.drv->file_f);
#endif /* VIRTUAL */

#ifndef VIRTUAL
    hal.drv->eeprom = eeprom_new(hal.drv->i2c);
#else
    hal.drv->eeprom = eeprom_new(hal.drv->file);
#endif /* VIRTUAL */
    ASSERT(hal.drv->eeprom);


#ifndef VIRTUAL
    #if defined(BOOTLOADER) || defined(F28004X) || defined(F28P65X)
    hal.drv->flash = flash_new(NULL);
    #endif
#else
    hal.drv->flash = flash_new(hal.drv->file_f);
#endif
    ASSERT(hal.drv->flash);

    /* Connect CAN module to I/O pins */
    const struct net *can = hal_get_can();
    ret = can_connect(can, hal.map->can_tx, hal.map->can_rx);


    /* Connect I2C module to I/O pins */
    ret = i2c_connect(hal.drv->i2c, hal.map->i2c_sda, hal.map->i2c_scl);
    ASSERT(ret >= 0);

    /* Initialize stopwatch timer */
    hal.swc->id = timer_setup(hal.swc->freq);
    ASSERT(hal.swc->id >= 0);

    hal_tic();

    /* Initialize task scheduler timer */
    hal.task->id = timer_setup(hal.task->freq);
    while ( ! timer_status(hal.task->id)){
        continue;
    }
    ASSERT(hal.task->id >= 0);



}

/**************************************************************************************************
 * 
 * \brief Resolves application-specific configuration
 * 
 * \param mod module data object handler
 * 
 * \details This function resolves application-specific configuration. CAN and I2C configuration
 * is the same for all modules and does not need to be resolved. Microcontroller LED needs to be
 * verified based on module type. The configuration resolve function is normally defined in the
 * application layer, but it is done here since bootloader does not include application layer.
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
static int
hal_resolve(const struct nfo *mod)
{
    ASSERT(mod);

    switch (mod->id) {
#if defined(F2803X)
    case NFO_LF45:
        hal.map->mcu_led = IO4_DOUT;
        break;
    case NFO_LL25:
        /* Intentional fall through */
    case NFO_UP25:
        /* Intentional fall through */
    case NFO_BC25:
        /* Pin is defined by HAL_MCU_LED */
        break;
#elif defined(F28004X)
    case NFO_BP25:
        /* Intentional fall through */
    case NFO_BI25:
        /* Pin is defined by HAL_MCU_LED */
    case NFO_VG11_FM01:
        /* Pin is defined by HAL_MCU_LED */
    case NFO_VG11_FM02:
        /* Pin is defined by HAL_MCU_LED */
    case NFO_CB01:
        /* Pin is defined by HAL_MCU_LED */
    case NFO_TBTE:
        /* Pin is defined by HAL_MCU_LED */
        break;
    case NFO_FAN1:
        /* For the moment, this board does not have LED */
        break;
#elif defined(F28P65X)
    case NFO_NONE:
    case NFO_BOOT:
    case NFO_FP:
    case NFO_FAN1:
        /* Intentional fall through */
        break;
#else
#error "hal.c - dsp not defined"
#endif
    /* Unknown device identifier */
    default:
        hal.map->mcu_led = IOX;
        return -1;
    }

    return 0;
}

/**************************************************************************************************
 * 
 * hal_setup()
 * 
 *************************************************************************************************/
int
hal_setup(const struct nfo *mod)
{
    ASSERT(mod);

    int ret;

    ret = hal_resolve(mod);

    /**
     * Microcontroller LED pin is resolved only based on module identifier. If that pin could not
     * be resolved, everything else in this function is guaranteed to fail.
     */
    if (ret < 0) {
        return -1;
    }

    ret = io_connect(hal.map->mcu_led, IO_DOUT);
    ASSERT(ret >= 0);

#ifndef BOOTLOADER
    /**
     * Setup application layer, but only if firmware is not ment to be used as bootloader. This
     * will setup such things as interlock trip, clear and read, and other application-specific
     * parameters and functions.
     */
    ret = hapi_setup(mod);
#endif

    return (ret < 0) ? -1 : 0;
}

/**************************************************************************************************
 * 
 * hal_reset()
 * 
 *************************************************************************************************/
void
hal_reset(void)
{
#ifndef BOOTLOADER
    /**
     * Interlock trip must be asserted before CPU reset in order to notify other modules on the
     * CAN bus that something out of ordinary is happening with this module.
     */
    hapi_interlock_trip();
#endif

    SysCtl_resetDevice();
}

/**************************************************************************************************
 * 
 * hal_tic()
 * 
 *************************************************************************************************/
__attribute__((ramfunc)) void
hal_tic(void)
{
    ASSERT(hal.swc->id >= 0);

    hal.swc->state = timer_state(hal.swc->id);
}

/**************************************************************************************************
 * 
 * hal_toc()
 * 
 *************************************************************************************************/
__attribute__((ramfunc)) uint32_t
hal_toc(void)
{
    ASSERT(hal.swc->id >= 0);

    uint32_t state = timer_state(hal.swc->id);

    cpu_ticks = hal.swc->state - state;
    /* This equation is also valid when CPU timer overflows */
    return cpu_ticks;
}

uint32_t
hal_get_cpu_busy(void)
{
    return cpu_ticks;
}

/**************************************************************************************************
 * 
 * hal_get_frequency()
 * 
 *************************************************************************************************/
uint32_t
hal_get_frequency(void)
{
    ASSERT(hal.task->id >= 0);

    return hal.task->freq;
}

/**************************************************************************************************
 * 
 * hal_tick()
 * 
 *************************************************************************************************/
bool
hal_tick(void)
{
    ASSERT(hal.task->id >= 0);

    return timer_status(hal.task->id);
}

/**************************************************************************************************
 * 
 * hal_led()
 * 
 *************************************************************************************************/
int
hal_led(bool state)
{
    int ret;

    ret = dio_write(hal.map->mcu_led, state);

    return (ret < 0) ? -1 : 0;
}

/**************************************************************************************************
 * 
 * hal_led_toggle()
 * 
 *************************************************************************************************/
int
hal_led_toggle(void)
{
    int ret;

    ret = dio_toggle(hal.map->mcu_led);

    return (ret < 0) ? -1 : 0;
}

/**************************************************************************************************
 * 
 * hal_get_can()
 * 
 *************************************************************************************************/
const struct net *
hal_get_can(void)
{
    ASSERT(hal.can->id < hal.can->count);

    return hal.drv->can[hal.can->id];
}

/**************************************************************************************************
 * 
 * hal_get_flash()
 * 
 *************************************************************************************************/
const struct net *
hal_get_flash(void)
{
    return hal.drv->flash;
}

/**************************************************************************************************
 * 
 * hal_get_eeprom()
 * 
 *************************************************************************************************/
const struct net *
hal_get_eeprom(void)
{
    return hal.drv->eeprom;
}

/**************************************************************************************************
 * 
 * hal_get_spi()
 * 
 *************************************************************************************************/
const struct net *
hal_get_spi(void)
{
    return hal.drv->spi;
}

/**************************************************************************************************
 * 
 * hal_get_i2c()
 * 
 *************************************************************************************************/
const struct net *
hal_get_i2c(void)
{
    return hal.drv->i2c;
}

/**************************************************************************************************
 * 
 * hal_get_i2c()
 * 
 *************************************************************************************************/
const struct net *
hal_get_fsi(void)
{   
    #ifdef F28004X
    return hal.drv->fsi;
    #else
    return NULL;
    #endif
}

/**************************************************************************************************
 * 
 * hal_get_id()
 * 
 *************************************************************************************************/
extern unsigned long
hal_get_SN(void)
{
    const struct id *id = id_get();
    return id->uid;
}

