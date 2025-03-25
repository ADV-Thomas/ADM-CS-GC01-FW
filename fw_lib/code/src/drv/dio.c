/**************************************************************************************************
 * 
 * \file dio.h
 * 
 * \brief Digital input-output driver implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "inc/drv/dio.h"

#include "inc/lib/debug.h"

#include "driverlib/gpio.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**************************************************************************************************
 * 
 * dio_write()
 * 
 *************************************************************************************************/
int
dio_write(enum io dio, bool state)
{
    uint32_t pin;
    enum io_fun fun;

    bool status = io_status(dio, &pin, &fun, NULL);

    if (status && (fun == IO_DOUT)) {
        GPIO_writePin(pin, (state ? 1U : 0U));
        return 0;
    } else {
        return -1;
    }
}

/**************************************************************************************************
 * 
 * dio_toggle()
 * 
 *************************************************************************************************/
int
dio_toggle(enum io dio)
{
    uint32_t pin;
    enum io_fun fun;

    bool status = io_status(dio, &pin, &fun, NULL);

    if (status && (fun == IO_DOUT)) {
        GPIO_togglePin(pin);
        return 0;
    } else {
        return -1;
    }
}

/**************************************************************************************************
 * 
 * dio_read()
 * 
 *************************************************************************************************/
int
dio_read(enum io dio, bool *state)
{
    ASSERT(state);

    uint32_t pin;
    enum io_fun fun;

    bool status = io_status(dio, &pin, &fun, NULL);

    if (status && (fun == IO_DIN || fun == IO_DOUT)) {
        *state = GPIO_readPin(pin) != 0;
        return 0;
    } else {
        return -1;
    }
}
