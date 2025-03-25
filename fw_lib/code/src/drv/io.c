/**************************************************************************************************
 * 
 * \file io.c
 * 
 * \brief I/O driver implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "inc/drv/io.h"

#include "driverlib/gpio.h"

#include "inc/drv/id.h"
#include "inc/lib/utils.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**************************************************************************************************
 * 
 * Defines for the driver
 * 
 *************************************************************************************************/

#if defined(F2803X)
#define IO_NUM_DIO      45      /* Total number of GPIOs in ports A and B (GPIO0..GPIO44)       */
#define IO_NUM_AN       16      /* Total number of GPIOs in port D (GPIO96..GPIO111)            */
#define IO_AN0          96      /* Number of first GPIO pin in port D                           */
#elif defined(F28004X)
#define IO_NUM_DIO      60      /* Total number of GPIOs in ports A, B, and C (GPIO0..GPIO59)   */
#define IO_NUM_AN       24      /* Total number of GPIOs in port H (GPIO224..GPIO247)           */
#define IO_AN0          224     /* Number of first GPIO pin in port H                           */
#elif defined(F28P65X)
#define IO_NUM_DIO      224      /* Total number of GPIOs in ports  A, B,  C, d , e , f , g (GPIO0..GPIO223)  */
#define IO_NUM_AN       17      /* Total number of GPIOs in port H    (GPIO225 .. AIO242)    */
#define IO_AN0          225     /* Number of first GPIO pin in port H   */
#else
#error "io.c - dsp not defined"
#endif

#if defined(F28004X)
/**************************************************************************************************
 * 
 * \brief Checks if GPIO pin is connected to analog subsystem
 * 
 * \param pin GPIO pin number
 * 
 * \note The analog signals are multiplexed with digital inputs, and they do not have digital
 * output capability. Analog mode is enabled by default.
 * 
 * \note This function is implemented in f2803x driverlib, but not in f28004x driverlib.
 * 
 * \return \b true if pin is connected to analog subsystem; \b false otherwise
 * 
 *************************************************************************************************/
static bool
GPIO_isPinAnalog(uint32_t pin)
{
    return ((pin >= 224U && pin <= 247U) || (pin == 22U) || (pin == 23U));
}
#endif


#if defined(F28P65X)
/**************************************************************************************************
 * 
 * \brief Checks if GPIO pin is connected to analog subsystem
 * 
 * \param pin GPIO pin number
 * 
 * \note The analog signals are multiplexed with digital inputs, and they do not have digital
 * output capability. Analog mode is enabled by default.
 * 
 * \note This function is implemented in f2803x driverlib, but not in f28004x driverlib.
 * 
 * \return \b true if pin is connected to analog subsystem; \b false otherwise
 * 
 *************************************************************************************************/
static bool
GPIO_isPinAnalog(uint32_t pin)
{
    return ((pin >= 225U && pin <= 247U) || (pin == 203U ) || (pin == 204U ) || (pin == 205U ) || (pin == 206U ) 
                                         || (pin == 207U ) || (pin == 208U ) || (pin == 209U ) 
                                         || (pin == 199U ) || (pin == 200U ) 
                                         || (pin == 213U ) || (pin == 214U ) 
    );
}
#endif

/**************************************************************************************************
 * 
 * List of GPIO registers which default (on-reset) state is 0
 * 
 *************************************************************************************************/
#if defined(F2803X)
static const uint16_t io_ctrl[] = {
    GPIO_O_GPACTRL,
    GPIO_O_GPAQSEL1,
    GPIO_O_GPAQSEL2,
    GPIO_O_GPAMUX1,
    GPIO_O_GPAMUX2,
    GPIO_O_GPADIR,
    /*GPIO_O_GPAPUD,*/
    GPIO_O_GPBCTRL,
    GPIO_O_GPBQSEL1,
    GPIO_O_GPBMUX1,
    GPIO_O_GPBDIR,
    GPIO_O_GPBPUD,
    /*GPIO_O_AIOMUX1,*/
    GPIO_O_AIODIR
};
#elif defined(F28004X)
static const uint16_t io_ctrl[] = {
    GPIO_O_GPACTRL,
    GPIO_O_GPAQSEL1,
    GPIO_O_GPAQSEL2,
    GPIO_O_GPAMUX1,
    GPIO_O_GPAMUX2,
    GPIO_O_GPADIR,
    /*GPIO_O_GPAPUD,*/
    GPIO_O_GPAINV,
    GPIO_O_GPAODR,
    /*GPIO_O_GPAAMSEL,*/
    GPIO_O_GPAGMUX1,
    GPIO_O_GPAGMUX2,
    GPIO_O_GPACSEL1,
    GPIO_O_GPACSEL2,
    GPIO_O_GPACSEL3,
    GPIO_O_GPACSEL4,
    /*GPIO_O_GPALOCK,*/
    /*GPIO_O_GPACR,*/
    GPIO_O_GPBCTRL,
    GPIO_O_GPBQSEL1,
    GPIO_O_GPBQSEL2,
    /*GPIO_O_GPBMUX1,*/
    GPIO_O_GPBMUX2,
    GPIO_O_GPBDIR,
    /*GPIO_O_GPBPUD,*/
    GPIO_O_GPBINV,
    GPIO_O_GPBODR,
    /*GPIO_O_GPBGMUX1,*/
    GPIO_O_GPBGMUX2,
    GPIO_O_GPBCSEL1,
    GPIO_O_GPBCSEL2,
    GPIO_O_GPBCSEL3,
    GPIO_O_GPBCSEL4,
    /*GPIO_O_GPBLOCK,*/
    /*GPIO_O_GPBCR,*/
    GPIO_O_GPHCTRL,
    GPIO_O_GPHQSEL1,
    GPIO_O_GPHQSEL2,
    /*GPIO_O_GPHPUD,*/
    GPIO_O_GPHINV
    /*GPIO_O_GPHAMSEL,*/
    /*GPIO_O_GPHLOCK,*/
    /*GPIO_O_GPHCR*/
};
#elif defined(F28P65X)
static const uint16_t io_ctrl[] = {
   GPIO_O_GPACTRL,
    GPIO_O_GPAQSEL1,
    GPIO_O_GPAQSEL2,
    GPIO_O_GPAMUX1,
    GPIO_O_GPAMUX2,
    GPIO_O_GPADIR,
    /*GPIO_O_GPAPUD,*/
    GPIO_O_GPAINV,
    GPIO_O_GPAODR,
    /*GPIO_O_GPAAMSEL,*/
    GPIO_O_GPAGMUX1,
    GPIO_O_GPAGMUX2,
    GPIO_O_GPACSEL1,
    GPIO_O_GPACSEL2,
    GPIO_O_GPACSEL3,
    GPIO_O_GPACSEL4,
    /*GPIO_O_GPALOCK,*/
    /*GPIO_O_GPACR,*/
    GPIO_O_GPBCTRL,
    GPIO_O_GPBQSEL1,
    GPIO_O_GPBQSEL2,
    /*GPIO_O_GPBMUX1,*/
    GPIO_O_GPBMUX2,
    GPIO_O_GPBDIR,
    /*GPIO_O_GPBPUD,*/
    GPIO_O_GPBINV,
    GPIO_O_GPBODR,
    /*GPIO_O_GPBGMUX1,*/
    GPIO_O_GPBGMUX2,
    GPIO_O_GPBCSEL1,
    GPIO_O_GPBCSEL2,
    GPIO_O_GPBCSEL3,
    GPIO_O_GPBCSEL4,
    /*GPIO_O_GPBLOCK,*/
    /*GPIO_O_GPBCR,*/
    GPIO_O_GPHCTRL,
    GPIO_O_GPHQSEL1,
    GPIO_O_GPHQSEL2,
    /*GPIO_O_GPHPUD,*/
    GPIO_O_GPHINV
    /*GPIO_O_GPHAMSEL,*/
    /*GPIO_O_GPHLOCK,*/
    /*GPIO_O_GPHCR*/
};
#else
#error "io.c - dsp not defined"
#endif

/**************************************************************************************************
 * 
 * List of GPIO registers with custom default values
 * 
 *************************************************************************************************/
#if defined(F2803X)
static const uint32_t io_reg[][2] = {
    { GPIO_O_GPAPUD,  0x00000FFFUL },
    { GPIO_O_AIOMUX1, 0x33303330UL }
};
#elif defined(F28004X)
static const uint32_t io_reg[][2] = {
    { GPIO_O_GPAPUD,   0xFFFFFFFFUL },
    { GPIO_O_GPAAMSEL, 0x00C00000UL }, //GPIO A Analog Mode Select (GPIO0 to GPIO31
    { GPIO_O_GPBMUX1,  0x00000CC0UL },
    { GPIO_O_GPBPUD,   0xFFFFFFFFUL },
    { GPIO_O_GPBGMUX1, 0x00000CC0UL },
    { GPIO_O_GPHPUD,   0xFFFFFFFFUL },
    { GPIO_O_GPHAMSEL, 0xFFFFFFFFUL }
};
#elif defined(F28P65X)
static const uint32_t io_reg[][2] = {
    { GPIO_O_GPAPUD,   0xFFFFFFFFUL }, //GPIO A Pull Up Disable Register (GPIO0 to 31)
    //{ GPIO_O_GPAAMSEL, 0x00C00000UL }, //no there 
    { GPIO_O_GPBMUX1,  0x00000CC0UL }, //GPIO B Mux 1 Register (GPIO32 to 47)
    { GPIO_O_GPBPUD,   0xFFFFFFFFUL }, //GPIO B Pull Up Disable Register (GPIO32 to 63)
    { GPIO_O_GPBGMUX1, 0x00000CC0UL }, //GPIO B Peripheral Group Mux (GPIO32 to 47)
    { GPIO_O_GPHPUD,   0xFFFFFFFFUL }, //GPIO H Pull Up Disable Register (GPIO224 to  255)
    { GPIO_O_GPHAMSEL, 0xFFFFFFFFUL } //GPIO H Analog Mode Select register (GPIO224 to GPIO255)
};
#else
#error "io.c - dsp not defined"
#endif

/**************************************************************************************************
 * 
 * \brief Resets all GPIO registers to default (on-reset) state
 * 
 * \param None
 * 
 * \return None
 * 
 *************************************************************************************************/
static void
GPIO_reset(void)
{
    EALLOW;

    unsigned i;

    for (i = 0U; i < LENGTH(io_ctrl); i++) {
        HWREG(GPIOCTRL_BASE + io_ctrl[i]) = 0U;
    }

    for (i = 0U; i < LENGTH(io_reg); i++) {
        HWREG(GPIOCTRL_BASE + io_reg[i][0]) = io_reg[i][1];
    }

    HWREG(GPIODATA_BASE + GPIO_O_GPADAT) = 0U;
    HWREG(GPIODATA_BASE + GPIO_O_GPBDAT) = 0U;
#ifdef F2803X
    HWREG(GPIODATA_BASE + GPIO_O_AIODAT) = 0U;
#endif /* F2803X */

    EDIS;
}

/**************************************************************************************************
 * 
 * Decode recipe for I/O pin configuration code
 * 
 *************************************************************************************************/
union io_u {
    enum io io;                 /* I/O pin configuration code                                   */
    struct {
        uint32_t pin:8;         /* GPIO pin number                                              */
        uint32_t mux:4;         /* (G)MUX register value                                        */
        uint32_t fun:16;        /* I/O peripheral function code (see 'enum io_fun')             */
        uint32_t :2;            /* Reserved                                                     */
        uint32_t status:1;      /* True when I/O pin is connected                               */
        uint32_t enable:1;      /* True when I/O pin is available                               */
    };
};

/**************************************************************************************************
 * 
 * I/O pin map
 * 
 *************************************************************************************************/
static union io_u io_u[IO_NUM_DIO + IO_NUM_AN];

/**************************************************************************************************
 * 
 * List of GPIO pins that are not available depending on the package size
 * 
 *************************************************************************************************/

#if defined(F2803X)
/* List of GPIO pins that are not available in all packages */
static const unsigned io_0[] = {
    96, 97, 99, 101, 103, 104, 105, 107, 108, 111
};

/* List of GPIO pins that are not available in 56-pin package */
static const unsigned io_56[] = {
    8, 9, 10, 11, 24, 32, 33
};

/* List of GPIO pins that are not available in 64-pin package. Also applies to 56-pin package. */
static const unsigned io_64[] = {
    13, 14, 15, 25, 26, 27, 39, 40, 41, 42, 43, 44
};
#elif defined(F28004X)
/* List of GPIO pins that are not available in all packages */
static const unsigned io_0[] = {
    19, 20, 21, 36, 38, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55
};

/* List of GPIO pins that are not available in 56-pin package */
static const unsigned io_56[] = {
    10
};

/* List of GPIO pins that are not available in 64-pin package. Also applies to 56-pin package. */
static const unsigned io_64[] = {
    14, 15, 25, 26, 27, 30, 31, 34, 39, 40, 56, 57, 58, 59
};

/**
 * List of GPIO pins that are not available in 64-pin Q-temperature package. Does not apply to
 * 56-pin package!
 */
static const unsigned io_64q[] = {
    12, 33
};



#elif defined(F28P65X)
/* List of GPIO pins that are not available in all packages */
static const unsigned io_0[] = {

};


static const unsigned io_100[] = {
    5,6,7,8,9, 16,17,18,19,20,21,22,23,24,26,27,28,29,31,32,33,36,37,38,39,40,
    44,45,48,49,50,51,52,53,54,55,56,57,67,68,74,75,76,77,79,81,82,83,88,93,94,95,
    96,97,98,99
};

static const unsigned io_176[] = {
    
};

static const unsigned io_169[] = {
    
};

static const unsigned io_256[] = {
    
};


#endif

/**************************************************************************************************
 * 
 * \brief Returns \b io_u array index for given I/O pin number
 * 
 * \param pin I/O pin number
 * 
 * \details There is a large gap between last digital I/O and first analog I/O element. In order
 * to save memory, analog I/O elements in \b io_u array are grouped immediately after digital I/O
 * elements. This function calculates array index from the I/O pin number.
 * 
 * \return Array index for given I/O pin number; -1 if pin number is not valid
 * 
 *************************************************************************************************/
static int
io_index(uint32_t pin)
{
    if (pin < IO_NUM_DIO) {
        return pin;
    } else if ((pin >= IO_AN0) && (pin < (IO_AN0 + IO_NUM_AN))) {
        return (pin - IO_AN0 + IO_NUM_DIO);
    } else { /* Invalid pin number */
        return -1;
    }
}

/**************************************************************************************************
 * 
 * \brief Disables pins specified by the list
 * 
 * \param list list with pins to disable
 * \param size number elements in the list
 * 
 * \return None
 * 
 *************************************************************************************************/
static void
io_disable(const unsigned *list, unsigned size)
{
    ASSERT(list);

    unsigned i;
    
    for (i = 0U; i < size; i++) {
        int ind = io_index(list[i]);
        ASSERT(ind >= 0);

        io_u[ind].fun = IO_NONE;
        io_u[ind].status = 0U;
        io_u[ind].enable = 0U;
    }
}

/**************************************************************************************************
 * 
 * \brief Writes default configuration to I/O pin map
 * 
 * \param pin I/O pin number
 * 
 * \return None
 * 
 *************************************************************************************************/
static void
io_default(uint32_t pin)
{
    int ind = io_index(pin);
    ASSERT(ind >= 0);

    union io_u _io = {
        .pin = pin,
        .mux = 0U,
        .fun = IO_DIN,
        .status = 0U,
        .enable = 1U
    };

    if (GPIO_isPinAnalog(pin)) {
        _io.fun = IO_AIN;
    }

    /* Some I/O pins should not be used although they physically exist */
    bool disable = false;

#if defined(F2803X)
    if (pin == 35U) {
        _io.io = IO35_JTAG_TDI;
        disable = true;
    } else if (pin == 36U) {
        _io.io = IO36_JTAG_TMS;
        disable = true;
    } else if (pin == 37U) {
        _io.io = IO37_JTAG_TDO;
        disable = true;
    } else if (pin == 38U) {
        _io.io = IO38_JTAG_TCK;
        disable = true;
    }
#elif defined(F28004X)
    if (pin == 18U) {
        _io.io = IO18_X2;
        disable = true;
    } else if (pin == 35U) {
        _io.io = IO35_JTAG_TDI;
        disable = true;
    } else if (pin == 37U) {
        _io.io = IO37_JTAG_TDO;
        disable = true;
    }

#elif defined(F28P65X)
    if (pin == 221U) {
        _io.io = IO221_X2;
        disable = true;
    }else if (pin == 222U) {
        _io.io = IO222_JTAG_TDI;
        disable = true;
    } else if (pin == 223U) {
        _io.io = IO223_JTAG_TDO;
        disable = true;
    }
#endif

    if (disable) {
        _io.status = 0U;
        _io.enable = 0U;
    }

    io_u[ind].io = _io.io;
}

/**************************************************************************************************
 * 
 * \brief Returns peripheral function group from function code
 * 
 * \param fun I/O peripheral function code
 * 
 * \return Peripheral function group
 * 
 *************************************************************************************************/
static inline enum io_group
io_get_group(enum io_fun fun)
{
    return (enum io_group) (fun & 0xFFF0U);
}

/**************************************************************************************************
 * 
 * \brief Calculates pin configuration code from \e pin_map.h
 * 
 * \param io I/O configuration code
 * 
 * \return Pin configuration code
 * 
 *************************************************************************************************/
static uint32_t
io_config(enum io io)
{
    union io_u _io = {
        .io = io
    };

    uint16_t mux = _io.mux & 0xFU;
    uint16_t shift = ((_io.pin % 16U) * 2U) & 0xFFU;

    uint16_t offset = ((_io.pin / 32U) * (GPIO_O_GPBMUX1 - GPIO_O_GPAMUX1));
    offset += (GPIO_O_GPAMUX1 + (((_io.pin % 32U) / 16U) * 2U));

    return (((uint32_t) offset << 16U) | (shift << 8U) | (mux << 0U));
}

/**************************************************************************************************
 * 
 * io_reset()
 * 
 *************************************************************************************************/
void
io_reset(void)
{
#if defined(F28004X)
    GPIO_unlockPortConfig(GPIO_PORT_A, 0xFFFFFFFFU);
    GPIO_unlockPortConfig(GPIO_PORT_B, 0xFFFFFFFFU);
    GPIO_unlockPortConfig(GPIO_PORT_H, 0xFFFFFFFFU);
#endif

#if defined(F28P65X)
    GPIO_unlockPortConfig(GPIO_PORT_A, 0xFFFFFFFFU);
    GPIO_unlockPortConfig(GPIO_PORT_B, 0xFFFFFFFFU);
    GPIO_unlockPortConfig(GPIO_PORT_C, 0xFFFFFFFFU);
    GPIO_unlockPortConfig(GPIO_PORT_D, 0xFFFFFFFFU);
    GPIO_unlockPortConfig(GPIO_PORT_E, 0xFFFFFFFFU);
    GPIO_unlockPortConfig(GPIO_PORT_F, 0xFFFFFFFFU);
    GPIO_unlockPortConfig(GPIO_PORT_G, 0xFFFFFFFFU);
    GPIO_unlockPortConfig(GPIO_PORT_H, 0xFFFFFFFFU);
#endif

    GPIO_reset();

    unsigned pin;

    for (pin = 0U; pin < IO_NUM_DIO; pin++) {
        io_default(pin);
    }

    for (pin = 0U; pin < IO_NUM_AN; pin++) {
        io_default(pin + IO_AN0);
    }

    const struct id *id = id_get();
    ASSERT(id);

    io_disable(io_0, LENGTH(io_0));

    #if defined(F2803X) || defined(F28004X) 
        switch (id->count) {
            case 56U:
                io_disable(io_56, LENGTH(io_56));
                /* Intentional fall through */
            case 64U:
                io_disable(io_64, LENGTH(io_64));
        }

    #elif defined(F28P65X)
        switch (id->count) {
            case 100U:
                io_disable(io_100, LENGTH(io_100));
                break;
            case 176U:
                io_disable(io_176, LENGTH(io_176));
                break;
            case 169U:
                io_disable(io_169, LENGTH(io_169));
                break;
            case 256U:
                io_disable(io_256, LENGTH(io_256));
                break;
        }
                

    #endif



#ifdef F28004X
        /* Some pins are not available in the 64-pin Q100 rated package */
        if ((id->count == 64U) && id->q100) {
            io_disable(io_64q, LENGTH(io_64q));
        }
#endif /* F28004X */
    
}

/**************************************************************************************************
 * 
 * io_connect()
 * 
 *************************************************************************************************/
int
io_connect(enum io io, enum io_fun exp)
{
    if (io == IOX) {
        return -1;
    }

    /* Decode I/O configuration code */
    union io_u _io = {
        .io = io
    };

    /* Verify expected peripheral function */
    if ((exp != IO_NONE) && (_io.fun != exp)) {
        return -2;
    }

    int ind = io_index(_io.pin);
    ASSERT(ind >= 0);

    /* Cannot connect if I/O pin is not available or already connected */
    if (!io_u[ind].enable || io_u[ind].status) {
        return -3;
    }

    switch (io_get_group((enum io_fun) _io.fun)) {
    case IO_DIO:
        if (GPIO_isPinAnalog(_io.pin)) {
#if defined(F28004X) || defined(F28P65X) 
            /* AIO pins (except GPIO22 and GPIO23) cannot be used as digital output on f28004x */
            if ((_io.pin >= IO_AN0) && (_io.fun == IO_DOUT)) {
                return -4;
            }
#endif
            GPIO_setAnalogMode(_io.pin, GPIO_ANALOG_DISABLED);
        }
#if defined(F2803X)
        /* Disable internal pull-up resistor for GPIO12..GPIO44 on f2803x */
        GPIO_setPadConfig(_io.pin, GPIO_PIN_TYPE_STD);
#endif
        if (_io.fun == IO_DOUT) {
            GPIO_setDirectionMode(_io.pin, GPIO_DIR_MODE_OUT);
        } else { /* fun == IO_DIN */
            GPIO_setDirectionMode(_io.pin, GPIO_DIR_MODE_IN);
        }
        break;

    case IO_CAN:
        /* CAN is not available on analog pins */
        if (GPIO_isPinAnalog(_io.pin)) {
            return -5;
        }
        break;

    case IO_I2C:
        /* I2C is not available on analog pins */
        if (GPIO_isPinAnalog(_io.pin)) {
            return -6;
        }
        GPIO_setQualificationMode(_io.pin, GPIO_QUAL_ASYNC);
        /* Internal pull-up resistor is not necessary, but it does no harm */
        GPIO_setPadConfig(_io.pin, GPIO_PIN_TYPE_PULLUP);
        break;

    case IO_EPWM:
        /* ePWM is not available on analog pins */
        if (GPIO_isPinAnalog(_io.pin)) {
            return -7;
        }
#if defined(F2803X)
        /* Disable internal pull-up resistor for GPIO12..GPIO44 on f2803x */
        GPIO_setPadConfig(_io.pin, GPIO_PIN_TYPE_STD);
#endif
        GPIO_setDirectionMode(_io.pin, GPIO_DIR_MODE_OUT);
        break;

#if defined(F28004X)
    /* Sigma-delta is supported only on f28004x */
    case IO_SD:
        /* Only GPIO22 analog pin supports sigma-delta */
        if (_io.pin == 22U) {
            GPIO_setAnalogMode(_io.pin, GPIO_ANALOG_DISABLED);
        } else if (GPIO_isPinAnalog(_io.pin)) {
            return -8;
        }
        GPIO_setQualificationMode(_io.pin, GPIO_QUAL_ASYNC);
        GPIO_setPadConfig(_io.pin, GPIO_PIN_TYPE_INVERT);
        break;
    case IO_FSI:
        if (GPIO_isPinAnalog(_io.pin)) {
            return -9;
        }
        GPIO_setQualificationMode(_io.pin, GPIO_QUAL_ASYNC);

        break;
#endif
    case IO_SPI:
        /* SPI is not available on analog pins */
        if (GPIO_isPinAnalog(_io.pin)) {
            return -10;
        }
        break;
    case IO_OUTXBAR:
        break;

    /* Unsupported peripheral function */
    default:
        return -11;
    }
    
    if (_io.pin < IO_NUM_DIO) {
        GPIO_setPinConfig(io_config(io));
    }

    /* I/O pin has been connected */
    io_u[ind].io = io;

    return 0;
}

/**************************************************************************************************
 * 
 * io_disconnect()
 * 
 *************************************************************************************************/
int
io_disconnect(enum io io)
{
    if (io == IOX) {
        return -1;
    }

    /* Decode I/O configuration code */
    union io_u _io = {
        .io = io
    };

    int ind = io_index(_io.pin);
    ASSERT(ind >= 0);

    /* Cannot disconnect if I/O pin is not available */
    if (!io_u[ind].enable) {
        return -1;
    }
    
#if defined(F2803X)
    GPIO_setDirectionMode(_io.pin, GPIO_DIR_MODE_IN);
    GPIO_writePin(_io.pin, 0U);
    
    if (GPIO_isPinAnalog(_io.pin)) {
        GPIO_setAnalogMode(_io.pin, GPIO_ANALOG_ENABLED);
    } else { /* !GPIO_isPinAnalog */
        GPIO_setPinConfig(io_config(_io.io));
        GPIO_setQualificationMode(_io.pin, GPIO_QUAL_SYNC);

        /* Pull-up resistor is disabled for GPIO0..11 and enabled for GPIO12..44 */
        if (_io.pin <= 11U) {
            GPIO_setPadConfig(_io.pin, GPIO_PIN_TYPE_STD);
        } else { /* pin > 11U */
            GPIO_setPadConfig(_io.pin, GPIO_PIN_TYPE_PULLUP);
        }
    }
#elif defined(F28004X) || defined(F28P65X)
    if (GPIO_isPinAnalog(_io.pin)) {
        GPIO_setAnalogMode(_io.pin, GPIO_ANALOG_ENABLED);
    }

    if (_io.pin < IO_NUM_DIO) {
        GPIO_setDirectionMode(_io.pin, GPIO_DIR_MODE_IN);
        GPIO_writePin(_io.pin, 0U);
        GPIO_setPinConfig(io_config(_io.io));
    }

    GPIO_setQualificationMode(_io.pin, GPIO_QUAL_SYNC);
    GPIO_setPadConfig(_io.pin, GPIO_PIN_TYPE_STD);
#endif

    /* I/O pin has been disconnected */
    io_default(_io.pin);

    return 0;
}

/**************************************************************************************************
 * 
 * io_status()
 * 
 *************************************************************************************************/
bool
io_status(enum io io, uint32_t *pin, enum io_fun *fun, enum io_group *group)
{
    union io_u _io = {
        .io = io
    };

    int ind = io_index(_io.pin);
    ASSERT(ind >= 0);

    io_decode(io_u[ind].io, pin, fun, group);

    return io_u[ind].status;
}

/**************************************************************************************************
 * 
 * io_decode()
 * 
 *************************************************************************************************/
int
io_decode(enum io io, uint32_t *pin, enum io_fun *fun, enum io_group *group)
{
    /* Decode I/O configuration code */
    union io_u _io = {
        .io = io
    };

    if (pin) {
        *pin = _io.pin;
    }

    if (fun) {
        *fun = (enum io_fun) _io.fun;
    }

    if (group) {
        *group = io_get_group((enum io_fun) _io.fun);
    }

    return 0;
}
