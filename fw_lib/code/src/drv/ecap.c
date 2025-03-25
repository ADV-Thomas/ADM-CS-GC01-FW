/**************************************************************************************************
 *
 * \file ecap.c
 *
 * \brief ECAP driver implementation
 *
 * \author Jorge Sola
 *
 *************************************************************************************************/

#include "inc/drv/ecap.h"

#include "driverlib/sysctl.h"
#include "driverlib/xbar.h"

#include "inc/drv/clk.h"


#include <stdint.h>
#include <stddef.h>

/**************************************************************************************************
 *
 * \brief Setups ECAP core
 *
 * \param var ECAP variable
 *
 * \return 0 if operation is successful; -1 otherwise
 *
 *************************************************************************************************/
static int setup_core(struct _ecap_var *var)
{
    ASSERT(ECAP_isBaseValid(var->base));

    int ret;
    uint32_t pin;

    /**
     * SETUP ECAP CORE
     * - Enable peripheral clock
     * - Connect GPIO to INPUT_XBAR (ECAP1 connected to INPUT1 XBAR, ECAP2 to INPUT2, etc.)
     * - Configure ECAP module to read directly period and pulse width of signal
     */
    SysCtl_enablePeripheral((SysCtl_PeripheralPCLOCKCR)(((var->core - 1U) << 8U) | 0x0003U));

    /**
     * It takes 2-5 CPU cycles before peripheral clock is enabled. Configuration registers cannot
     * be modified before peripheral clock has been enabled!
     */
    asm(" RPT #5 || NOP");


    /** Connect GPIO, Input XBAR, select ECAP input and set event prescaler
     * NOTE: ECAP1 connected to INPUT1 XBAR, ECAP2 to INPUT2,...ECAPN to
     * INPUTN, etc.
     * */
    ret = io_connect(var->io, IO_DIN);
    if (ret < 0) {
        return -1;
    }
    io_decode(var->io, &pin, NULL, NULL);
    #ifndef F28P65X
        XBAR_setInputPin((XBAR_InputNum)(var->core - 1U), pin);
    #else
        XBAR_setInputPin(INPUTXBAR_BASE,(XBAR_InputNum)(var->core - 1U), pin);
    #endif

    ECAP_selectECAPInput(var->base, (ECAP_InputCaptureSignals)(var->core - 1U));
    ECAP_setEventPrescaler(var->base, (var->div >> 1));

    /** Configure ECAP module with the following characteristics:
     * - Capture mode
     * - Continuous run wrapping on event 2
     * - Event 1 -> rising polarity. Event 2 -> falling polarity
     * - Reset timer on Event 1, so that we can read period and pulse width directly.
     */
    ECAP_enableCaptureMode(var->base);
    ECAP_setCaptureMode(var->base, ECAP_CONTINUOUS_CAPTURE_MODE, ECAP_EVENT_2);

    ECAP_setEventPolarity(var->base, ECAP_EVENT_1, ECAP_EVNT_RISING_EDGE);
    ECAP_setEventPolarity(var->base, ECAP_EVENT_2, ECAP_EVNT_FALLING_EDGE);

    ECAP_enableCounterResetOnEvent(var->base, ECAP_EVENT_1);

    ECAP_enableTimeStampCapture(var->base);
    ECAP_startCounter(var->base);

    return 0;
}


/**************************************************************************************************
 *
 * Generic ECAP object definition
 *
 *************************************************************************************************/
_ECAP_OBJ_STRUCT(_ECAP_OBJ_STRUCT_MEMBER(var););

/**************************************************************************************************
 *
 * ecap_setup()
 *
 *************************************************************************************************/
int ecap_setup(const struct _ecap *ecap)
{
#if defined(F2803X)
    // Not implemented
    return -1;
#elif defined(F28004X) ||  defined(F28P65X) 
    struct _ecap_priv *priv      = ecap->priv;
    struct _ecap_var *const *var = &ecap->var;

    ASSERT(obj_check(ecap, OBJ__ECAP));


    /**
     * SETUP ECAP CORES
     * - Each ECAP core should be setup only once
     */
    int ret;

    unsigned i;
    for (i = 0U; i < priv->size; i++) {
        ASSERT(var[i]);

        if (!var[i]->enable) {
            continue;
        }


        ASSERT((var[i]->core >= 1U) && (var[i]->core <= 7U));

        var[i]->base = ECAP1_BASE + ((var[i]->core - 1U) * (ECAP2_BASE - ECAP1_BASE));


        uint16_t mask = 1U << var[i]->core;
        if ((priv->init & mask) == 0U) {
            ret = setup_core(var[i]);
            if (ret < 0) {
                return -1;
            } else {
                priv->init |= mask;
            }
        }
    }

    return 0;
#endif
}
