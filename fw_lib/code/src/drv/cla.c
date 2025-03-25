/**************************************************************************************************
 * 
 * \file cla.c
 * 
 * \brief CLA driver implementation
 * 
 * \author Jorge Sola
 * 
 *************************************************************************************************/

#include "inc/drv/cla.h"

#include "driverlib/sysctl.h"
#include "driverlib/cla.h"
#include "driverlib/memcfg.h"

#include "inc/drv/clk.h"

#include <stdint.h>

/**************************************************************************************************
 * 
 * Generic CLA object definition
 * 
 *************************************************************************************************/
_CLA_OBJ_STRUCT(
    _CLA_OBJ_STRUCT_MEMBER(var);
);

/**************************************************************************************************
 * 
 * cla_setup()
 * 
 *************************************************************************************************/
int
cla_setup(const struct _cla *cla)
{
    #if defined(F2803X)
    /* CLA not supported in F2803x */
    return -1;

    #elif defined(F28004X)
    struct _cla_priv *priv = cla->priv;
    struct _cla_var * const *var = &cla->var;

    ASSERT(obj_check(cla, OBJ__CLA));

    /** Initialization of CLA includes:
     * - Enabling clock for the peripheral
     * - Setting LS0 RAM memory as program memory
     * - Setting LS1 to LS7 RAM memory as shared data memory
     * - Disabling background task
     * - Enabling IACK (CPU can start a CLA task)
    */
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_CLA1);

    MemCfg_setLSRAMMasterSel(MEMCFG_SECT_LS0, MEMCFG_LSRAMMASTER_CPU_CLA1);
	MemCfg_setCLAMemType(MEMCFG_SECT_LS0, MEMCFG_CLA_MEM_PROGRAM);

    MemCfg_setLSRAMMasterSel(MEMCFG_SECT_LS1, MEMCFG_LSRAMMASTER_CPU_CLA1);
    MemCfg_setLSRAMMasterSel(MEMCFG_SECT_LS2, MEMCFG_LSRAMMASTER_CPU_CLA1);
    MemCfg_setLSRAMMasterSel(MEMCFG_SECT_LS3, MEMCFG_LSRAMMASTER_CPU_CLA1);
    MemCfg_setLSRAMMasterSel(MEMCFG_SECT_LS4, MEMCFG_LSRAMMASTER_CPU_CLA1);
    MemCfg_setLSRAMMasterSel(MEMCFG_SECT_LS5, MEMCFG_LSRAMMASTER_CPU_CLA1);
    MemCfg_setLSRAMMasterSel(MEMCFG_SECT_LS6, MEMCFG_LSRAMMASTER_CPU_CLA1);
    MemCfg_setLSRAMMasterSel(MEMCFG_SECT_LS7, MEMCFG_LSRAMMASTER_CPU_CLA1);
    MemCfg_setCLAMemType(MEMCFG_SECT_LS1, MEMCFG_CLA_MEM_DATA);
    MemCfg_setCLAMemType(MEMCFG_SECT_LS2, MEMCFG_CLA_MEM_DATA);
    MemCfg_setCLAMemType(MEMCFG_SECT_LS3, MEMCFG_CLA_MEM_DATA);
    MemCfg_setCLAMemType(MEMCFG_SECT_LS4, MEMCFG_CLA_MEM_DATA);
    MemCfg_setCLAMemType(MEMCFG_SECT_LS5, MEMCFG_CLA_MEM_DATA);
    MemCfg_setCLAMemType(MEMCFG_SECT_LS6, MEMCFG_CLA_MEM_DATA);
    MemCfg_setCLAMemType(MEMCFG_SECT_LS7, MEMCFG_CLA_MEM_DATA);

    CLA_disableBackgroundTask(CLA1_BASE);
    CLA_enableIACK(CLA1_BASE);

    #pragma diag_suppress=770

    /* Initialize each declared task */
    unsigned i;
    for (i = 0U; i < priv->size; i++) {
        ASSERT(var[i]);

        if (!var[i]->enable) {
            continue;
        }

        uint16_t taskFlag = (1 << var[i]->task_num);
        CLA_mapTaskVector(CLA1_BASE, (CLA_MVECTNumber)var[i]->task_num,(uint16_t)(var[i]->task_fun));
        CLA_setTriggerSource((CLA_TaskNumber)var[i]->task_num,(CLA_Trigger) var[i]->trigger);
        CLA_enableTasks(CLA1_BASE, taskFlag);
    }

    #pragma diag_error=770

    return 0;

    #endif
}
