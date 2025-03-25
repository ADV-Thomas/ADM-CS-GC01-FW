/**************************************************************************************************
 * 
 * \file wch.c
 * 
 * \brief Window Hardware Comparator driver implementation
 * 
 * \author Jorge Sola
 * 
 *************************************************************************************************/

#include "inc/drv/wch.h"

#include "driverlib/sysctl.h"
#include "driverlib/asysctl.h"

#include "inc/drv/clk.h"

#include <stdint.h>

/** This lookup table returns the Comparator submodule number based on desired ADC core and channel */
static const uint16_t ADC_A_core_lookup[16] = {255,255,1,1,2,2,5,5,6,6,7,1,2,255,5,6};
static const uint16_t ADC_B_core_lookup[16] = {7,7,3,3,4,4,1,1,2,2,3,4,7,255,255,255};
static const uint16_t ADC_C_core_lookup[16] = {1,2,3,4,5,6,3,3,4,4,7,7,255,255,7,255};


/* This lookup table returns the Comparator submodule MUX to be used in ASysCtl_selectCMPHPMux() functions */
static const uint16_t ADC_A_mux_lookup[16] = {255,255,0x00,0x00,0x03,0x03,0x0C,0x0C,0x10,0x10,0x13,0x00,0x03,255,0x0C,0x10};
static const uint16_t ADC_B_mux_lookup[16] = {0x13,0x13,0x06,0x06,0x09,0x09,0x00,0x00,0x03,0x03,0x06,0x09,0x13,255,255,255};
static const uint16_t ADC_C_mux_lookup[16] = {0x00,0x03,0x06,0x09,0x0C,0x10,0x06,0x06,0x09,0x09,0x13,0x13,255,255,0x13,255};

/* This lookup table returns the the values needed to wire the ADC signals as positive terminals in the comparator */
static const uint16_t ADC_A_value_lookup[16] = {255,255,0,3,0,3,0,3,0,3,0,4,4,255,4,4};
static const uint16_t ADC_B_value_lookup[16] = {3,0,0,3,0,3,0,4,0,4,4,4,4,255,255,255};
static const uint16_t ADC_C_value_lookup[16] = {1,1,1,1,1,1,0,4,0,4,0,4,255,255,1,255};


/**************************************************************************************************
 * 
 * \brief Based on desired ADC core and channel to be windowed, the right comparator subbmodule
 * with the right mux and value is resolved based on a look-up table extracted from the reference
 * manual. These look-up tables are implemented as specified in SPRUI33D page 1416
 *  (Table 12-1. Analog Pins and Internal Connections)
 * 
 * \param [in] adc_core is the ADC core to which the comparator should be connected (A = 1, B = 2, C = 3).
 * \param [in] adc_ch ADC channel to which the comparator should be connected.
 * \param [out] wch_core pointer to variable that holds the comparator submodule number.
 * \param [out] mux_select pointer to the mux_select variable.
 * \param [out] mux_select pointer to value.
 * 
 *************************************************************************************************/
static int
wch_resolve(uint16_t adc_core, uint16_t adc_ch, uint16_t *wch_core, uint32_t *mux_sel, uint32_t *value)
{
    uint16_t mux_sel_temp;
    uint16_t value_temp;
    uint16_t wch_core_temp;

    /** Check for channel in range. Since channel 13 is not wired to any analog input,
    * when channel is out of range, we can set it to 13 and it will return 255, which
    * means error 
    * */
    if(adc_ch > 15){
        adc_ch = 13;
    }

    switch(adc_core){
        case 1U:
        wch_core_temp = ADC_A_core_lookup[adc_ch];
        mux_sel_temp = ADC_A_mux_lookup[adc_ch];
        value_temp = ADC_A_value_lookup[adc_ch];
        break;

        case 2U:
        wch_core_temp = ADC_B_core_lookup[adc_ch];
        mux_sel_temp = ADC_B_mux_lookup[adc_ch];
        value_temp = ADC_B_value_lookup[adc_ch];
        break;

        case 3U:
        wch_core_temp = ADC_C_core_lookup[adc_ch];
        mux_sel_temp = ADC_C_mux_lookup[adc_ch];
        value_temp = ADC_C_value_lookup[adc_ch];
        break;

        default:
        wch_core_temp = 255;
        mux_sel_temp = 255;
        value_temp = 255;
        break;
    }

    if(wch_core_temp == 255 || mux_sel_temp == 255 || value_temp == 255){
        return -1;
    }
    else{
        *wch_core = wch_core_temp;
        *mux_sel = (uint32_t)mux_sel_temp;
        *value = (uint32_t)value_temp;
        return 0;
    }    
}

/**************************************************************************************************
 * 
 * \brief Setups WCH submodule
 * 
 * \param base base address of the WCH submodule
 * \param adc_input ADC input to be wired to comparator submodule
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
static int
setup_core(struct _wch_var *var)
{
    uint16_t core = ((var->base - CMPSS1_BASE) / (CMPSS2_BASE - CMPSS1_BASE));
    uint16_t code = (core << 8) | 0x000EU;

    SysCtl_enablePeripheral((SysCtl_PeripheralPCLOCKCR) code);
    asm(" RPT #5 || NOP");

    /* Reset WCH core */
    SysCtl_resetPeripheral((SysCtl_PeripheralSOFTPRES) code);

    CMPSS_disableModule(var->base);



    /* Configure HIGH Comparator:
    - Inverting input is driven by internal DAC
    - Digital filter output drives CTRIPH (trip high signal)
    - Digital filter latched output drives CTRIPOUTH
    */

    /* Configure LOW Comparator:
    - Inverting input is driven by internal DAC
    - Output is inverted
    - Digital filter output drives CTRIPH (trip high signal)
    - Digital filter latched output drives CTRIPOUTH
    */
    CMPSS_configHighComparator(var->base, CMPSS_INSRC_DAC);
    CMPSS_configLowComparator(var->base, CMPSS_INSRC_DAC | CMPSS_INV_INVERTED);

    CMPSS_enableModule(var->base);

    /* Configure high and low trip filters:
    - FIFO with 32 samples
    - Debouncing with 20 samples
    - Frequency is SYSFREQ/10
     */
    CMPSS_configFilterHigh(var->base, 1u, 32u, 17u);
    CMPSS_configFilterLow(var->base, 1u, 32u, 17u);

    CMPSS_initFilterHigh(var->base);
    CMPSS_initFilterLow(var->base);


    //CMPSS_configOutputsHigh(base, CMPSS_TRIPOUT_FILTER| CMPSS_TRIP_FILTER);
    //CMPSS_configOutputsLow(base, CMPSS_TRIPOUT_FILTER | CMPSS_TRIP_FILTER);
    CMPSS_configOutputsHigh(var->base, CMPSS_TRIPOUT_ASYNC_COMP| CMPSS_TRIPOUT_ASYNC_COMP);
    CMPSS_configOutputsLow(var->base, CMPSS_TRIPOUT_ASYNC_COMP | CMPSS_TRIPOUT_ASYNC_COMP);

    // Set Hysteresis to zero
    CMPSS_setHysteresis(var->base, 0);

    //EPWMSYNCPER will not reset latch
    CMPSS_configLatchOnPWMSYNC(var->base, false, false);

    // Disable EPWM blanking
    CMPSS_disableBlanking(var->base);

    /**
     * Configure ramp
     *  COMPDACCTL.bit.RAMPSOURCE = 0   EPWM1SYNCPER signal is used within the CMPSS module
     *  COMPDACCTL.bit.RAMPLOADSEL = 0  RAMPSTS is updated from RAMPMAXREFA
     *  RAMPMAXREFS = 0                 Ramp maximum reference shadow register
     *  RAMPDECVALS = 0                 Ramp decrement value shadow register
     *  RAMPDLYS = 0                    Ramp delay shadow register
     */
    CMPSS_configRamp(var->base, (4095U<<4), 0, 0, var->pwm_sync, false);

    /**
     * Configure DAC
     *  COMPDACCTL.bit.DACSOURCE = 0    DACHVALA is updated from DACHVALS
     *  COMPDACCTL.bit.SELREF = 0       VDDA is the voltage reference for DAC
     *  COMPDACCTL.bit.SWLOADSEL = 0    DACxVALA is updated from DACxVALS on SYSCLK
     */
    CMPSS_configDAC(var->base, CMPSS_DACVAL_SYSCLK | CMPSS_DACREF_VDDA | CMPSS_DACSRC_RAMP);


    // By default, set the extreme values so that the comparators are not trip
    CMPSS_setDACValueHigh(var->base, 4095u);
    CMPSS_setDACValueLow(var->base, 0u);

    CMPSS_clearFilterLatchHigh(var->base);
    CMPSS_clearFilterLatchLow(var->base);

    /* Connect comparator outputs to the selected XBAR trip. 
    * The value passed to the config function is extracted from
    * table 9-2 p.969 in SPRUI33d, where LSB selects a comparator
    * high or low trip, and MSB selects the mux that contains the required
    * comparator
    */
    
    uint32_t mux = (core*4)<<8;
    XBAR_EPWMMuxConfig value;

    if(var->xbar_trip_h == var->xbar_trip_l && var->xbar_trip_h > 0){
        /* The Xbar is tripped by both High or Low comparators */
        value = (XBAR_EPWMMuxConfig)(mux | 0x0001);
        XBAR_setEPWMMuxConfig((XBAR_TripNum)var->xbar_trip_h, value);
        XBAR_enableEPWMMux((XBAR_TripNum)var->xbar_trip_h, (1 << (core*2)));
    }
    else{
        /* High trip config */
        value = (XBAR_EPWMMuxConfig)(mux | 0x0000);
        XBAR_setEPWMMuxConfig((XBAR_TripNum)var->xbar_trip_h, value);
        XBAR_enableEPWMMux((XBAR_TripNum)var->xbar_trip_h, (1 << (core*2)));

        /* Low trip config */
        value = (XBAR_EPWMMuxConfig)(mux | 0x0200);
        XBAR_setEPWMMuxConfig((XBAR_TripNum)var->xbar_trip_l, value);
        XBAR_enableEPWMMux((XBAR_TripNum)var->xbar_trip_l, (1 << (core*2 + 1)));        
    }

    return 0;
}

/**************************************************************************************************
 * 
 * Generic WCH object definition
 * 
 *************************************************************************************************/
_WCH_OBJ_STRUCT(
    _WCH_OBJ_STRUCT_MEMBER(var);
);

/**************************************************************************************************
 * 
 * wch_setup()
 * 
 *************************************************************************************************/
int
wch_setup(const struct _wch *wch)
{
    // Hardware Window comparators not implemented for F2803X devices for the moment
#if defined(F2803X)
        return -1


#elif defined(F28004X) || defined(F28P65X) //TODO F28P65X

    struct _wch_priv *priv = wch->priv;
    struct _wch_var * const *var = &wch->var;
    uint32_t mux_select;
    uint32_t mux_value;
    ASSERT(obj_check(wch, OBJ__ADC));

    /**
     * SETUP WCH CORE
     * - WCH submodule is resolved based on desired ADC core and channel to be windowed
     * - Each WCH submodule should be setup only once
     * - WCH submodule is configured only if enabled
     */
    int ret;

    unsigned i;
    for (i = 0U; i < priv->size; i++) {
        ASSERT(var[i]);

        if (!var[i]->enable) {
            continue;
        }

        // Extract required Comparator submodule based on desired ADC core and channel to be windowed
        ret = wch_resolve(var[i]->adc_core, var[i]->adc_channel, &var[i]->core, &mux_select, &mux_value);
        if(ret < 0){
            return -1;
        }

        /* Connect analog input channels to comparator subsystems for High and Low comparators */
        ASysCtl_selectCMPHPMux((ASysCtl_CMPHPMuxSelect)mux_select, mux_value);
        ASysCtl_selectCMPLPMux((ASysCtl_CMPLPMuxSelect)mux_select, mux_value);

        ASSERT((var[i]->core >= 1U) && (var[i]->core <= 7U));
        var[i]->base = CMPSS1_BASE + ((var[i]->core - 1U) * (CMPSS2_BASE - CMPSS1_BASE));

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
