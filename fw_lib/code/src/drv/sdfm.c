/**************************************************************************************************
 *
 * \file sdfm.c
 *
 * \brief SDFM driver implementation
 *
 * \author Jorge Sola
 *
 *************************************************************************************************/

#include "inc/drv/sdfm.h"


#include "driverlib/sysctl.h"

#include "inc/drv/clk.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

static const int osr_ranges[12]        = {31, 40, 50, 63, 80, 101, 127, 161, 181, 203, 255, 256};
static const int shift_values_sin3[12] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 8, 9, 10};

/**************************************************************************************************
 *
 * \brief Calculates required bit shifting based on OSR and filter type (Sin3)
 *
 * \param var SDFM variable handler
 *
 * \return The shift value if OSR is within range; 255 otherwise
 *
 *************************************************************************************************/
static int get_shift_value(const struct _sdfm_var *var)
{
    int i;

    for (i = 0; i < 12; i++) {
        if (var->oversampling <= osr_ranges[i]) {
            return shift_values_sin3[i];
        }
    }
    return 255;
}


/**************************************************************************************************
 *
 * \brief Configures SOC wrappers to given configuration
 *
 * \param var ADC variable handler
 *
 * \return 0 if operation is successful; -1 otherwise
 *
 *************************************************************************************************/
static int setup_channel(const struct _sdfm_var *var)
{
    int ret;
    ASSERT(var);
    SDFM_FilterNumber filter_num = (SDFM_FilterNumber)(var->channel);

    SDFM_setupModulatorClock(SDFM1_BASE, filter_num, (SDFM_ModulatorClockMode)var->clk_mode);
    SDFM_configDataFilter(
        SDFM1_BASE, (filter_num | SDFM_FILTER_SINC_3 | SDFM_SET_OSR(var->oversampling)),
        (SDFM_DATA_FORMAT_16_BIT | SDFM_FILTER_ENABLE | SDFM_SHIFT_VALUE(var->shift)));
    SDFM_setOutputDataFormat(SDFM1_BASE, filter_num, SDFM_DATA_FORMAT_16_BIT);

    ret = io_connect(var->io, IO_NONE);

    if(var->io_clk != IOX){
        ret |= io_connect(var->io_clk, IO_NONE); 
    }
    
    if (ret < 0) {
        return -1;
    }

    return 0;
}

/**************************************************************************************************
 *
 * Generic ADC object definition
 *
 *************************************************************************************************/
_SDFM_OBJ_STRUCT(_SDFM_OBJ_STRUCT_MEMBER(var););
/**************************************************************************************************
 *
 * adc_setup()
 *
 *************************************************************************************************/
int sdfm_setup(const struct _sdfm *sdfm)
{
#if defined(F2803X)
    /* F2803X does not have SDFM module */
    return -1;

#elif defined(F28004X) || defined(F28P65X) //TODO F28P65X
    struct _sdfm_priv *priv      = sdfm->priv;
    struct _sdfm_var *const *var = &sdfm->var;

    ASSERT(obj_check(adc, OBJ__SDFM));

    /**
     * SETUP SDFM CORE
     */
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_SD1);

    /**
     * It takes 2-5 CPU cycles before peripheral clock is enabled. Configuration registers cannot
     * be modified before peripheral clock has been enabled!
     */
    asm(" RPT #5 || NOP");

    SDFM_enableMasterFilter(SDFM1_BASE);

    /**
     * SETUP SDFM CHANNELS
     * - Each SDFM channel should be setup only once
     */
    int ret;

    unsigned i;
    for (i = 0U; i < priv->size; i++) {
        ASSERT(var[i]);

        if (!var[i]->enable) {
            continue;
        }

        enum io_fun fun;

        io_decode(var[i]->io, NULL, &fun, NULL);

        var[i]->channel = ((fun & 0xFU) >> 1);
        var[i]->shift   = get_shift_value(var[i]);

        /** Return -1 if pin function is not SDFM or if OSR is
         * out of range
         * */
        if (((fun & 0xF0U) != 0x90U) || (var[i]->oversampling > 256)) {
            return -1;
        }

        uint16_t mask = 1U << var[i]->channel;
        if ((priv->init & mask) == 0U) {
            ret = setup_channel(var[i]);
            if (ret < 0) {
                return -1;
            } else {
                priv->init |= mask;
            }
        } else {
            return -1;
        }
    }

    return 0;
#endif
}
