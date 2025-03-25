/**************************************************************************************************
 * 
 * \file adc.c
 * 
 * \brief ADC driver implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "inc/drv/adc.h"

#include "driverlib/sysctl.h"

#include "inc/drv/clk.h"

#include <stdint.h>

/**************************************************************************************************
 * 
 * Defines for the driver
 * 
 *************************************************************************************************/

#if defined(F2803X)
#define POWER_UP_DELAY      1000U           /* Delay for ADC to be stable after power-up (us)   */
#define F_DIV_MAX           2U              /* Maximum system clock divider                     */
#define F_CLK_MIN           1000UL          /* ADC clock minimum operating frequency (Hz)       */
#define F_CLK_MAX           60000000UL      /* ADC clock maximum operating frequency (Hz)       */
#define INT_NUMBER          9U              /* Number of interrupt lines                        */
#define OFFSET_TRIM         80U             /* Bias for zero-offset calibration                 */
#elif defined(F28004X)
#define POWER_UP_DELAY      5000U           /* Delay for ADC to be stable after power-up (us)   */
#define F_DIV_MAX           8U              /* Maximum system clock divider                     */
#define F_CLK_MIN           5000000UL       /* ADC clock minimum operating frequency (Hz)       */
#define F_CLK_MAX           50000000UL      /* ADC clock maximum operating frequency (Hz)       */
#define INT_NUMBER          4U              /* Number of interrupt lines                        */
#define OFFSET_TRIM         112U            /* Bias for zero-offset calibration                 */
#elif defined(F28P65X)
#define POWER_UP_DELAY      5000U           /* Delay for ADC to be stable after power-up (us)   */
#define F_DIV_MAX           8U              /* Maximum system clock divider                     */
#define F_CLK_MIN           5000000UL       /* ADC clock minimum operating frequency (Hz)       */
#define F_CLK_MAX           50000000UL      /* ADC clock maximum operating frequency (Hz)       */
#define INT_NUMBER          4U              /* Number of interrupt lines                        */
#define OFFSET_TRIM         112U            /* Bias for zero-offset calibration                 */
#else
#error "adc.c - dsp not defined"
#endif

/**************************************************************************************************
 * 
 * \brief Calculates number of ADC clock cycles for given sampling time
 * 
 * \param sample sampling time (ns)
 * \param window storage for calculated ADC clock cycles
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
static int
sample_window(uint32_t sample, uint32_t *window)
{
    ASSERT((sample > 0U) && window);

#if defined(F28004X)
    if (sample < 75U) {
        return -1;
    }
#endif

    const struct _clk *_clk = clk_get();
    const struct clk *clk = &_clk->adc;

    ASSERT(_clk);
    ASSERT(clk->Hz > 0U);

    uint32_t cycles = (((uint64_t) sample * clk->Hz) + (1000000000UL - 1U)) / 1000000000UL;
    if (cycles == 0U) {
        return -1;
    }



#if defined(F2803X)
    /* Decrement by one for ACQPS field in ADCSOCxCTL register */
    cycles--;
    if (cycles > 63U) {
        return -1;
    }

    /* Some values are invalid. Check Table 8-18. in the datasheet. */
    const unsigned invalid[5][2] = {
        {0U, 5U}, { 16U, 20U }, { 29U, 33U }, { 42U, 46U }, { 55U, 59U }
    };

    unsigned i;
    for (i = 0U; i < 5U; i++) {
        if ((cycles >= invalid[i][0]) && (cycles <= invalid[i][1])) {
            cycles = invalid[i][1] + 1U;
            break;
        }
    }
#elif defined(F28004X)
    if (cycles > 513U) {
        return -1;
    }
#endif

    *window = cycles;

    return 0;
}

/**************************************************************************************************
 * 
 * \brief Configures and enables ADC interrupt
 * 
 * \param base base address of the ADC module
 * \param number ADC interrupt number
 * \param soc SOC wrapper number
 * 
 * \return None
 * 
 *************************************************************************************************/
void
setup_interrupt(uint32_t base, uint16_t number, uint16_t soc)
{
    ASSERT(ADC_isBaseValid(base));
    ASSERT(soc < 16U);
    ASSERT((number >= 1U) && (number <= INT_NUMBER));

    ADC_IntNumber _number = (ADC_IntNumber) (number - 1U);
    ADC_SOCNumber _soc = (ADC_SOCNumber) soc;

    ADC_disableInterrupt(base, _number);

    ADC_setInterruptSource(base, _number, _soc);
    ADC_disableContinuousMode(base, _number);

    ADC_enableInterrupt(base, _number);
    ADC_clearInterruptStatus(base, _number);
}

/**************************************************************************************************
 * 
 * \brief Setups ADC core
 * 
 * \param base base address of the ADC module
 * \param result base address of the ADC result
 * \param div ADC clock divider
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
static int
setup_core(uint32_t base, uint32_t result, uint16_t div)
{
    ASSERT(ADC_isBaseValid(base));
    ASSERT((result > 0U) && (div > 0U));

    /**
     * SETUP ADC CORE
     * - enables ADC peripheral clock
     * - resets ADC core to default configuration and copies calibration values
     * - configures ADC clock prescaler and delay after power-up
     * - generate interrupt pulse on end-of-conversion event
     * - sets high-priority mode for all SOC wrappers
     */

#if defined(F2803X)
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_ADC);
    asm(" RPT #2 || NOP");

    /* Reset ADC core and restore calibration values */
    ADC_resetModule(base);
#elif defined(F28004X) || defined(F28P65X)
    uint16_t core = ((base - ADCA_BASE) / (ADCB_BASE - ADCA_BASE));
    uint16_t code = (core << 8) | 0x000DU;

    SysCtl_enablePeripheral((SysCtl_PeripheralPCLOCKCR) code);
    asm(" RPT #5 || NOP");

    /* Reset ADC core and restore calibration values */
    SysCtl_resetPeripheral((SysCtl_PeripheralSOFTPRES) code);
#endif

    ADC_disableConverter(base);
#if defined(F2803X)
    ADC_powerDown(base);
#endif

    ADC_setPrescaler(base, (ADC_ClkPrescale) (2U * (div - 1U)));

#if defined(F2803X)
    ADC_setVREF(base, ADC_REFERENCE_INTERNAL);
#elif defined(F28004X) || defined(F28P65X)
    ADC_setVREF(base, ADC_REFERENCE_INTERNAL, ADC_REFERENCE_3_3V);
#endif

#if defined(F2803X)
    ADC_powerUp(base);
#endif
    ADC_enableConverter(base);

    clk_delay(POWER_UP_DELAY);

    ADC_setInterruptPulseMode(base, ADC_PULSE_END_OF_CONV);

    /* All SOC wrappers are arbitrated by SOC number (lower number has higher priority) */
    ADC_setSOCPriority(base, ADC_PRI_ALL_HIPRI);

#if defined(F2803X)
    /**
     * ZERO-OFFSET CALIBRATION
     */
    ADC_connectVREFLO(base);

    int ret;
    uint32_t window;

    ret = sample_window(200U, &window);
    if (ret < 0) {
        return -1;
    }

    unsigned soc;
    for (soc = 0U; soc < 16U; soc++) {
        ADC_setupSOC(base, (ADC_SOCNumber) soc, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN13, window);
    }

    /* ADCINT1 flag will be set on EOC15 */
    setup_interrupt(base, 1U, 15U);

    ADC_setOffsetTrim(base, OFFSET_TRIM);

    const unsigned samples = 32U * 16U;
    uint32_t sum = 0U;

    unsigned i;
    for (i = 0U; i < samples; i += 16U) {
        ADC_forceMultipleSOC(base, 0xFFFFU);

        /* Wait maximum 10 x 10 us for all conversions to finish */
        unsigned timeout = 10U;
        while (!ADC_getInterruptStatus(base, ADC_INT_NUMBER1)) {
            if (timeout-- == 0U) {
                return -1;
            }
            clk_delay(10U);
        }

        ADC_clearInterruptStatus(base, ADC_INT_NUMBER1);

        for (soc = 0U; soc < 16U; soc++) {
            sum += ADC_readResult(result, (ADC_SOCNumber) soc);
        }
    }

    ADC_disableInterrupt(base, ADC_INT_NUMBER1);

    uint16_t offset = OFFSET_TRIM - ((uint16_t) (sum / samples));

    /**
     * CAUTION: If device calibration routine in \e SysCtl_deviceCal() is called after this point,
     * it will overwrite the calibrated offset.
     */
    ADC_setOffsetTrim(base, offset);

    ADC_disconnectVREFLO(base);
    ADC_disconnectTempSensor(base);

#elif defined(F28004X) || defined(F28P65X)
    /** Zero offset calibration not needed in F28004X devices:
     * https://e2e.ti.com/support/microcontrollers/c2000-microcontrollers-group/c2000/f/c2000-microcontrollers-forum/1055469/tms320f28035-tms320f280049c-adc-accuracy
     * but that offset is ment for 25 deg ambient. We redo the offset calibration in case the
     * environment has a different temperature*/
    int ret;
    uint32_t window;

    ret = sample_window(200U, &window);
    if (ret < 0) {
        return -1;
    }

    unsigned soc;
    for (soc = 0U; soc < 16U; soc++) {
        ADC_setupSOC(base, (ADC_SOCNumber) soc, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN13, window);
    }

    /* ADCINT1 flag will be set on EOC15 */
    setup_interrupt(base, 1U, 15U);

    /* This adds an artificial offset to account for negative offset that may
    reside in the ADC core (taken from reference manual)*/
    HWREGH(base + ADC_O_OFFTRIM) = OFFSET_TRIM;

    const unsigned samples = 32U * 16U;
    uint32_t sum = 0U;

    unsigned i;
    for (i = 0U; i < samples; i += 16U) {
        ADC_forceMultipleSOC(base, 0xFFFFU);

        /* Wait maximum 10 x 10 us for all conversions to finish */
        unsigned timeout = 10U;
        while (!ADC_getInterruptStatus(base, ADC_INT_NUMBER1)) {
            if (timeout-- == 0U) {
                return -1;
            }
            clk_delay(10U);
        }

        ADC_clearInterruptStatus(base, ADC_INT_NUMBER1);

        for (soc = 0U; soc < 16U; soc++) {
            sum += ADC_readResult(result, (ADC_SOCNumber) soc);
        }
    }

    ADC_disableInterrupt(base, ADC_INT_NUMBER1);

    /**
     * CAUTION: If device calibration routine in \e SysCtl_deviceCal() is called after this point,
     * it will overwrite the calibrated offset.
     */
    uint16_t offset = OFFSET_TRIM - ((uint16_t) (sum / samples));
    HWREGH(base + ADC_O_OFFTRIM) = offset;
#endif

    return 0;
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
static int
setup_channel(const struct _adc_var *var)
{
    ASSERT(var && (var->count > 0U) && var->soc);

    int ret;
    uint32_t window;
    uint16_t soc_n;
    uint16_t trig_src;

    ret = sample_window(var->sample, &window);
    if (ret < 0) {
        return -1;
    }

    unsigned i;
    for (i = 0U; i < var->count; i++) {
        // We split the trigger source (MSB) and SOC slot number (LSB)
        trig_src = (var->soc[i] & 0xFF00) >> 8;
        soc_n = (var->soc[i] & 0x00FF);

        ASSERT(trig_src <= 20U)
        // The following if/else is made to be backward compatible with previous commit version
        // i.e. From commit 3e49720 and older
        // The logic is: if MSB is not used, the trigger source is the one specified by the field
        // .trigger. If MSB is used, then the trigger is specified by this value.
        // WARNING: this assumes that ADC_TRIGGER_SW_ONLY will never be used (because its value is 0)
        if(trig_src == 0){
            ADC_setupSOC(var->base, (ADC_SOCNumber)var->soc[i], var->trigger, var->channel, window);
        }
        else{
            ADC_setupSOC(var->base, (ADC_SOCNumber) soc_n, (ADC_Trigger)trig_src, var->channel, window);
        }
    }

    /* Setup interrupt for last SOC wrapper from the list */
    if (var->int_n > 0U) {
        soc_n = (var->soc[var->count-1U]) & 0x00FF;
        setup_interrupt(var->base, var->int_n, soc_n);
    }

    return 0;
}

/**************************************************************************************************
 * 
 * Generic ADC object definition
 * 
 *************************************************************************************************/
_ADC_OBJ_STRUCT(
    _ADC_OBJ_STRUCT_MEMBER(var);
);

/**************************************************************************************************
 * 
 * adc_setup()
 * 
 *************************************************************************************************/
int
adc_setup(const struct _adc *adc)
{
    struct _adc_priv *priv = adc->priv;
    struct _adc_var * const *var = &adc->var;

    ASSERT(obj_check(adc, OBJ__ADC));

    /**
     * INITIALIZE ADC CLOCK FREQUENCY
     * - ADC clock is derived from the system clock
     * - Calculates clock prescaler to get ADC clock frequency within defined range
     */

    const struct _clk *_clk = clk_get();
    const struct clk *clk = &_clk->cpu;

    ASSERT(_clk);

    unsigned div;
    for (div = 1U; div <= F_DIV_MAX; div++) {
        uint32_t f = clk->Hz / div;
        if ((f >= F_CLK_MIN) && (f <= F_CLK_MAX)) {
            clk_set_adc(f);
            break;
        } else if (div == F_DIV_MAX) {
            return -1;
        }
    }

    /**
     * SETUP ADC CORES AND CHANNELS
     * - Each ADC core should be setup only once
     * - ADC channel is configured only if enabled
     */

    int ret;

    unsigned i;
    for (i = 0U; i < priv->size; i++) {
        ASSERT(var[i]);

        if (!var[i]->enable) {
            continue;
        }

#if defined(F2803X)
        ASSERT(var[i]->core == 1U);

        var[i]->base = ADCA_BASE;
        var[i]->result = ADCARESULT_BASE;
#elif defined(F28004X)
        ASSERT((var[i]->core >= 1U) && (var[i]->core <= 3U));

        var[i]->base = ADCA_BASE + ((var[i]->core - 1U) * (ADCB_BASE - ADCA_BASE));
        var[i]->result = ADCARESULT_BASE + ((var[i]->core - 1U) *
            (ADCBRESULT_BASE - ADCARESULT_BASE));
#endif

        uint16_t mask = 1U << var[i]->core;
        if ((priv->init & mask) == 0U) {
            ret = setup_core(var[i]->base, var[i]->result, div);
            if (ret < 0) {
                return -1;
            } else {
                priv->init |= mask;
            }
        }

        ret = setup_channel(var[i]);
        if (ret < 0) {
            return -1;
        }
    }

    return 0;
}
