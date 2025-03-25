/**************************************************************************************************
 *
 * \file pga.c
 *
 * \brief PGA driver implementation
 *
 * \author Jorge Sola
 *
 *************************************************************************************************/

#include "inc/drv/pga.h"


#include "driverlib/sysctl.h"

#include "inc/drv/clk.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


/**************************************************************************************************
 *
 * Generic PGA object definition
 *
 *************************************************************************************************/
_PGA_OBJ_STRUCT(_PGA_OBJ_STRUCT_MEMBER(var););


static int setup_core(struct _pga_var *pga_var){
    /** TODO: implement! */

    uint16_t code = ((pga_var->core-1U) << 8) | 0x000FU;

    SysCtl_enablePeripheral((SysCtl_PeripheralPCLOCKCR) code);
    asm(" RPT #5 || NOP");

    /* Reset PGA core and restore calibration values */
    SysCtl_resetPeripheral((SysCtl_PeripheralSOFTPRES) code);

    PGA_setGain(pga_var->base,pga_var->gain);
    PGA_setFilterResistor(pga_var->base,PGA_LOW_PASS_FILTER_DISABLED);
    PGA_enable(pga_var->base);

    return 0;
}

/**************************************************************************************************
 *
 * pga_setup()
 *
 *************************************************************************************************/
int pga_setup(const struct _pga *pga)
{
#if defined(F2803X)
    /* F2803X does not have PGA module */
    return -1;

#elif defined(F28004X)
    struct _pga_priv *priv      = pga->priv;
    struct _pga_var *const *var = &pga->var;

    ASSERT(obj_check(adc, OBJ__PGA));

    /**
     * SETUP PGA CHANNELS
     * - Each PGA channel should be setup only once
     */
    int ret;

    unsigned i;
    for (i = 0U; i < priv->size; i++) {
        ASSERT(var[i]);

        if (!var[i]->enable) {
            continue;
        }

        if(var[i]->core < 1 || var[i]->core > 5){
            return -1;
        }

        var[i]->base = PGA1_BASE + ((var[i]->core - 1U) * (PGA2_BASE - PGA1_BASE));
        uint16_t mask = 1U << var[i]->core;
        if ((priv->init & mask) == 0U) {
            ret = setup_core(var[i]);
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