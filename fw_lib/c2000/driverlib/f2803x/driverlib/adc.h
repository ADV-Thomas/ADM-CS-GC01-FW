/**************************************************************************************************
 * 
 * \file adc.h
 * 
 * \brief ADC driverlib interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef ADC_H
#define ADC_H

#include "inc/hw_adc.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include "cpu.h"
#include "debug.h"
#include "sysctl.h"

#include <stdint.h>
#include <stdbool.h>

/**************************************************************************************************
 * 
 * Values that can be passed to \e ADC_setPrescaler() as the \e prescale parameter.
 * 
 *************************************************************************************************/
typedef enum {
    ADC_CLK_DIV_1_0 = 0,                /* ADCCLK = SYSCLK / 1                                  */
    ADC_CLK_DIV_2_0 = 2                 /* ADCCLK = SYSCLK / 2                                  */
} ADC_ClkPrescale;

/**************************************************************************************************
 * 
 * Values that can be passed to \e ADC_setupSOC() as the \e trigger parameter.
 * 
 *************************************************************************************************/
typedef enum {
    ADC_TRIGGER_SW_ONLY    = 0,         /* ADCTRIG0 - Software only                             */
    ADC_TRIGGER_CPU1_TINT0 = 1,         /* ADCTRIG1 - CPU Timer 0                               */
    ADC_TRIGGER_CPU1_TINT1 = 2,         /* ADCTRIG2 - CPU Timer 1                               */
    ADC_TRIGGER_CPU1_TINT2 = 3,         /* ADCTRIG3 - CPU Timer 2                               */
    ADC_TRIGGER_GPIO       = 4,         /* ADCTRIG4 - GPIO XINT2                                */
    ADC_TRIGGER_EPWM1_SOCA = 5,         /* ADCTRIG5 - ePWM1, ADCSOCA                            */
    ADC_TRIGGER_EPWM1_SOCB = 6,         /* ADCTRIG6 - ePWM1, ADCSOCB                            */
    ADC_TRIGGER_EPWM2_SOCA = 7,         /* ADCTRIG7 - ePWM2, ADCSOCA                            */
    ADC_TRIGGER_EPWM2_SOCB = 8,         /* ADCTRIG8 - ePWM2, ADCSOCB                            */
    ADC_TRIGGER_EPWM3_SOCA = 9,         /* ADCTRIG9 - ePWM3, ADCSOCA                            */
    ADC_TRIGGER_EPWM3_SOCB = 10,        /* ADCTRIG10 - ePWM3, ADCSOCB                           */
    ADC_TRIGGER_EPWM4_SOCA = 11,        /* ADCTRIG11 - ePWM4, ADCSOCA                           */
    ADC_TRIGGER_EPWM4_SOCB = 12,        /* ADCTRIG12 - ePWM4, ADCSOCB                           */
    ADC_TRIGGER_EPWM5_SOCA = 13,        /* ADCTRIG13 - ePWM5, ADCSOCA                           */
    ADC_TRIGGER_EPWM5_SOCB = 14,        /* ADCTRIG14 - ePWM5, ADCSOCB                           */
    ADC_TRIGGER_EPWM6_SOCA = 15,        /* ADCTRIG15 - ePWM6, ADCSOCA                           */
    ADC_TRIGGER_EPWM6_SOCB = 16,        /* ADCTRIG16 - ePWM6, ADCSOCB                           */
    ADC_TRIGGER_EPWM7_SOCA = 17,        /* ADCTRIG17 - ePWM7, ADCSOCA                           */
    ADC_TRIGGER_EPWM7_SOCB = 18         /* ADCTRIG18 - ePWM7, ADCSOCB                           */
} ADC_Trigger;

/**************************************************************************************************
 * 
 * Values that can be passed to \e ADC_setupSOC() as the \e channel parameter.
 * 
 *************************************************************************************************/
typedef enum {
    ADC_CH_ADCIN0  = 0,                 /* ADCIN0 (A0 or A0-B0) is converted                    */
    ADC_CH_ADCIN1  = 1,                 /* ADCIN1 (A1 or A1-B1) is converted                    */
    ADC_CH_ADCIN2  = 2,                 /* ADCIN2 (A2 or A2-B2) is converted                    */
    ADC_CH_ADCIN3  = 3,                 /* ADCIN3 (A3 or A3-B3) is converted                    */
    ADC_CH_ADCIN4  = 4,                 /* ADCIN4 (A4 or A4-B4) is converted                    */
    ADC_CH_ADCIN5  = 5,                 /* ADCIN5 (A5 or A5-B5) is converted                    */
    ADC_CH_ADCIN6  = 6,                 /* ADCIN6 (A6 or A6-B6) is converted                    */
    ADC_CH_ADCIN7  = 7,                 /* ADCIN7 (A7 or A7-B7) is converted                    */
    ADC_CH_ADCIN8  = 8,                 /* ADCIN8 (B0) is converted                             */
    ADC_CH_ADCIN9  = 9,                 /* ADCIN9 (B1) is converted                             */
    ADC_CH_ADCIN10 = 10,                /* ADCIN10 (B2) is converted                            */
    ADC_CH_ADCIN11 = 11,                /* ADCIN11 (B3) is converted                            */
    ADC_CH_ADCIN12 = 12,                /* ADCIN12 (B4) is converted                            */
    ADC_CH_ADCIN13 = 13,                /* ADCIN13 (B5) is converted                            */
    ADC_CH_ADCIN14 = 14,                /* ADCIN14 (B6) is converted                            */
    ADC_CH_ADCIN15 = 15                 /* ADCIN15 (B7) is converted                            */
} ADC_Channel;

/**************************************************************************************************
 * 
 * Values passed to \e ADC_setupSOC(), \e ADC_setInterruptSOCTrigger(), \e ADC_forceSOC(),
 * \e ADC_getSOCStatus(), \e ADC_getSOCOverflowStatus(), \e ADC_clearSOCOverflowStatus(),
 * \e ADC_readResult() and \e ADC_setInterruptSource() as the \e soc parameter.
 * 
 *************************************************************************************************/
typedef enum {
    ADC_SOC_NUMBER0  = 0,               /* SOC/EOC number 0                                     */
    ADC_SOC_NUMBER1  = 1,               /* SOC/EOC number 1                                     */
    ADC_SOC_NUMBER2  = 2,               /* SOC/EOC number 2                                     */
    ADC_SOC_NUMBER3  = 3,               /* SOC/EOC number 3                                     */
    ADC_SOC_NUMBER4  = 4,               /* SOC/EOC number 4                                     */
    ADC_SOC_NUMBER5  = 5,               /* SOC/EOC number 5                                     */
    ADC_SOC_NUMBER6  = 6,               /* SOC/EOC number 6                                     */
    ADC_SOC_NUMBER7  = 7,               /* SOC/EOC number 7                                     */
    ADC_SOC_NUMBER8  = 8,               /* SOC/EOC number 8                                     */
    ADC_SOC_NUMBER9  = 9,               /* SOC/EOC number 9                                     */
    ADC_SOC_NUMBER10 = 10,              /* SOC/EOC number 10                                    */
    ADC_SOC_NUMBER11 = 11,              /* SOC/EOC number 11                                    */
    ADC_SOC_NUMBER12 = 12,              /* SOC/EOC number 12                                    */
    ADC_SOC_NUMBER13 = 13,              /* SOC/EOC number 13                                    */
    ADC_SOC_NUMBER14 = 14,              /* SOC/EOC number 14                                    */
    ADC_SOC_NUMBER15 = 15               /* SOC/EOC number 15                                    */
} ADC_SOCNumber;

/**************************************************************************************************
 * 
 * Values that can be passed to \e ADC_setInterruptSOCTrigger() as the \e trigger parameter.
 * 
 *************************************************************************************************/
typedef enum {
    ADC_INT_SOC_TRIGGER_NONE    = 0,    /* No ADCINT will trigger the SOC                       */
    ADC_INT_SOC_TRIGGER_ADCINT1 = 1,    /* ADCINT1 will trigger the SOC                         */
    ADC_INT_SOC_TRIGGER_ADCINT2 = 2     /* ADCINT2 will trigger the SOC                         */
} ADC_IntSOCTrigger;

/**************************************************************************************************
 * 
 * Values that can be passed to \e ADC_setInterruptPulseMode() as the \e mode parameter.
 * 
 *************************************************************************************************/
typedef enum {
    ADC_PULSE_END_OF_ACQ_WIN = 0x00,    /* Pulse occurs at the end of the acquisition window    */
    ADC_PULSE_END_OF_CONV    = 0x04     /* Pulse occurs at the end of the conversion            */
} ADC_PulseMode;

/**************************************************************************************************
 * 
 * Values that can be passed to \e ADC_getInterruptStatus(), \e ADC_clearInterruptStatus(),
 * \e ADC_getInterruptOverflowStatus(), \e ADC_clearInterruptOverflowStatus(),
 * \e ADC_enableInterrupt(), \e ADC_disableInterrupt(), \e ADC_setInterruptSource(),
 * \e ADC_enableContinuousMode(), and \e ADC_disableContinuousMode as the \e number parameter.
 * 
 *************************************************************************************************/
typedef enum {
    ADC_INT_NUMBER1 = 0,                /* ADCINT1 interrupt                                    */
    ADC_INT_NUMBER2 = 1,                /* ADCINT2 interrupt                                    */
    ADC_INT_NUMBER3 = 2,                /* ADCINT3 interrupt                                    */
    ADC_INT_NUMBER4 = 3,                /* ADCINT4 interrupt                                    */
    ADC_INT_NUMBER5 = 4,                /* ADCINT5 interrupt                                    */
    ADC_INT_NUMBER6 = 5,                /* ADCINT6 interrupt                                    */
    ADC_INT_NUMBER7 = 6,                /* ADCINT7 interrupt                                    */
    ADC_INT_NUMBER8 = 7,                /* ADCINT8 interrupt                                    */
    ADC_INT_NUMBER9 = 8                 /* ADCINT9 interrupt                                    */
} ADC_IntNumber;

/**************************************************************************************************
 * 
 * Values that can be passed to \e ADC_setSOCPriority() as the \e mode parameter.
 * 
 *************************************************************************************************/
typedef enum {
    ADC_PRI_ALL_ROUND_ROBIN  = 0x00,    /* Round robin mode is used for all channels            */
    ADC_PRI_SOC0_HIPRI       = 0x01,    /* SOC0 is high-priority, others in round robin         */
    ADC_PRI_THRU_SOC1_HIPRI  = 0x02,    /* SOC0..1 is high-priority, others in round robin      */
    ADC_PRI_THRU_SOC2_HIPRI  = 0x03,    /* SOC0..2 is high-priority, others in round robin      */
    ADC_PRI_THRU_SOC3_HIPRI  = 0x04,    /* SOC0..3 is high-priority, others in round robin      */
    ADC_PRI_THRU_SOC4_HIPRI  = 0x05,    /* SOC0..4 is high-priority, others in round robin      */
    ADC_PRI_THRU_SOC5_HIPRI  = 0x06,    /* SOC0..5 is high-priority, others in round robin      */
    ADC_PRI_THRU_SOC6_HIPRI  = 0x07,    /* SOC0..6 is high-priority, others in round robin      */
    ADC_PRI_THRU_SOC7_HIPRI  = 0x08,    /* SOC0..7 is high-priority, others in round robin      */
    ADC_PRI_THRU_SOC8_HIPRI  = 0x09,    /* SOC0..8 is high-priority, others in round robin      */
    ADC_PRI_THRU_SOC9_HIPRI  = 0x0A,    /* SOC0..9 is high-priority, others in round robin      */
    ADC_PRI_THRU_SOC10_HIPRI = 0x0B,    /* SOC0..10 is high-priority, others in round robin     */
    ADC_PRI_THRU_SOC11_HIPRI = 0x0C,    /* SOC0..11 is high-priority, others in round robin     */
    ADC_PRI_THRU_SOC12_HIPRI = 0x0D,    /* SOC0..12 is high-priority, others in round robin     */
    ADC_PRI_THRU_SOC13_HIPRI = 0x0E,    /* SOC0..13 is high-priority, others in round robin     */
    ADC_PRI_THRU_SOC14_HIPRI = 0x0F,    /* SOC0..14 is high-priority, others in round robin     */
    ADC_PRI_ALL_HIPRI        = 0x10     /* All SOCs are in high-priority mode                   */
} ADC_PriorityMode;

/**************************************************************************************************
 * 
 * Values passed to \e ADC_forceMultipleSOC() as the \e mask parameter.
 * 
 *************************************************************************************************/

#define ADC_FORCE_SOC0          0x0001U     /* Software trigger for ADC SOC0                    */
#define ADC_FORCE_SOC1          0x0002U     /* Software trigger for ADC SOC1                    */
#define ADC_FORCE_SOC2          0x0004U     /* Software trigger for ADC SOC2                    */
#define ADC_FORCE_SOC3          0x0008U     /* Software trigger for ADC SOC3                    */
#define ADC_FORCE_SOC4          0x0010U     /* Software trigger for ADC SOC4                    */
#define ADC_FORCE_SOC5          0x0020U     /* Software trigger for ADC SOC5                    */
#define ADC_FORCE_SOC6          0x0040U     /* Software trigger for ADC SOC6                    */
#define ADC_FORCE_SOC7          0x0080U     /* Software trigger for ADC SOC7                    */
#define ADC_FORCE_SOC8          0x0100U     /* Software trigger for ADC SOC8                    */
#define ADC_FORCE_SOC9          0x0200U     /* Software trigger for ADC SOC9                    */
#define ADC_FORCE_SOC10         0x0400U     /* Software trigger for ADC SOC10                   */
#define ADC_FORCE_SOC11         0x0800U     /* Software trigger for ADC SOC11                   */
#define ADC_FORCE_SOC12         0x1000U     /* Software trigger for ADC SOC12                   */
#define ADC_FORCE_SOC13         0x2000U     /* Software trigger for ADC SOC13                   */
#define ADC_FORCE_SOC14         0x4000U     /* Software trigger for ADC SOC14                   */
#define ADC_FORCE_SOC15         0x8000U     /* Software trigger for ADC SOC15                   */

/**************************************************************************************************
 * 
 * Values that can be passed to \e ADC_enableSimultaneousSampling() and
 * \e ADC_disableSimultaneousSampling() as the \e mask parameter.
 * 
 *************************************************************************************************/

#define ADC_SAMPLE_PAIR_SOC0_1      0x0001U     /* Simultaneous sampling for SOC0/1 pair        */
#define ADC_SAMPLE_PAIR_SOC2_3      0x0002U     /* Simultaneous sampling for SOC2/3 pair        */
#define ADC_SAMPLE_PAIR_SOC4_5      0x0004U     /* Simultaneous sampling for SOC4/5 pair        */
#define ADC_SAMPLE_PAIR_SOC6_7      0x0008U     /* Simultaneous sampling for SOC6/7 pair        */
#define ADC_SAMPLE_PAIR_SOC8_9      0x0010U     /* Simultaneous sampling for SOC8/9 pair        */
#define ADC_SAMPLE_PAIR_SOC10_11    0x0020U     /* Simultaneous sampling for SOC10/11 pair      */
#define ADC_SAMPLE_PAIR_SOC12_13    0x0040U     /* Simultaneous sampling for SOC12/13 pair      */
#define ADC_SAMPLE_PAIR_SOC14_15    0x0080U     /* Simultaneous sampling for SOC14/15 pair      */

/**************************************************************************************************
 * 
 * Values that can be passed to \e ADC_setVREF() as the \e mode parameter.
 * 
 *************************************************************************************************/
typedef enum {
    ADC_REFERENCE_INTERNAL,             /* Internal bandgap used for reference generation       */
    ADC_REFERENCE_EXTERNAL              /* VREFHI/VREFLO pins used for reference generation     */
} ADC_ReferenceMode;

/**************************************************************************************************
 * 
 * Prototypes for the APIs
 * 
 *************************************************************************************************/

#ifdef DEBUG
/**************************************************************************************************
 * 
 * \brief Checks ADC module base address
 * 
 * \param base base address of the ADC module
 * 
 * \return \b true if the ADC module base address is valid
 * 
 *************************************************************************************************/
static inline bool
ADC_isBaseValid(uint32_t base)
{
    return (base == ADCA_BASE);
}

/**************************************************************************************************
 * 
 * \brief Checks ADC result base address
 * 
 * \param base base address of the ADC result
 * 
 * \return \b true if the ADC result base address is valid
 * 
 *************************************************************************************************/
static inline bool
ADC_isResultBaseValid(uint32_t base)
{
    return (base == ADCARESULT_BASE);
}
#endif

/**************************************************************************************************
 * 
 * \brief Configures the ADC converter clock prescaler
 * 
 * \param base base address of the ADC module
 * \param prescale ADC clock prescaler
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
ADC_setPrescaler(uint32_t base, ADC_ClkPrescale prescale)
{
    ASSERT(ADC_isBaseValid(base));

    EALLOW;

    if (prescale == ADC_CLK_DIV_1_0) {
        HW_BIT_CLR(base, ADC, CTL2, CLKDIV2EN);
    } else { /* prescale == ADC_CLK_DIV_2_0 */
        HW_BIT_SET(base, ADC, CTL2, CLKDIV2EN);
    }

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Enables overlap of sample and conversion
 * 
 * \param base base address of the ADC module
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
ADC_enableOverlap(uint32_t base)
{
    ASSERT(ADC_isBaseValid(base));

    EALLOW;

    HW_BIT_CLR(base, ADC, CTL2, ADCNONOVERLAP);

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Disables overlap of sample and conversion
 * 
 * \param base base address of the ADC module
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
ADC_disableOverlap(uint32_t base)
{
    ASSERT(ADC_isBaseValid(base));

    EALLOW;

    HW_BIT_SET(base, ADC, CTL2, ADCNONOVERLAP);

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Configures ADC SOC wrapper
 * 
 * \param base base address of the ADC module
 * \param soc SOC wrapper number
 * \param trigger source that will cause SOC
 * \param channel analog input signal
 * \param window acquisition window duration
 * 
 * \details This function configures ADC SOC wrapper. The acquisition window duration is given as
 * number of the ADC clock cycles minus 1. Valid values for the \e window parameter are in range
 * 6..63 with exception of 16..20, 29..33, 42..46, 55..59 (see Table 8-18.). The total sampling
 * time is acquisition window duration plus conversion time (13 ADC clock cycles).
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
ADC_setupSOC(uint32_t base, ADC_SOCNumber soc, ADC_Trigger trigger, ADC_Channel channel,
    uint32_t window)
{
    ASSERT(ADC_isBaseValid(base));
    ASSERT((window >= 6U) && (window <= 63U));
    ASSERT(!((window >= 16U) && (window <= 20U)));
    ASSERT(!((window >= 29U) && (window <= 33U)));
    ASSERT(!((window >= 42U) && (window <= 46U)));
    ASSERT(!((window >= 55U) && (window <= 59U)));

    uint32_t offset = (uint32_t) soc;

    EALLOW;

    HW_FLD_SETO(base, offset, ADC, SOCxCTL, ACQPS, window);
    HW_FLD_SETO(base, offset, ADC, SOCxCTL, CHSEL, channel);
    HW_FLD_SETO(base, offset, ADC, SOCxCTL, TRIGSEL, trigger);

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Configures interrupt SOC trigger of an SOC wrapper
 * 
 * \param base base address of the ADC module
 * \param soc SOC wrapper number
 * \param trigger interrupt source that will cause the SOC
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
ADC_setInterruptSOCTrigger(uint32_t base, ADC_SOCNumber soc, ADC_IntSOCTrigger trigger)
{
    ASSERT(ADC_isBaseValid(base));

    uint32_t offset = (uint32_t) soc >> 3;
    uint16_t shift = ((uint16_t) soc & 0x7U) << 1;
    
    uint16_t reg = HW_REG_GETO(base, offset, ADC, INTSOCSELx);

    uint16_t mask = 0x3U << shift;
    uint16_t value = ((uint16_t) trigger << shift) & mask;

    EALLOW;

    HW_REG_SETO(base, offset, ADC, INTSOCSELx, ((reg & ~mask) | value));

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Sets the timing of the EOC pulse
 * 
 * \param base base address of the ADC module
 * \param mode generation mode of the EOC pulse
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
ADC_setInterruptPulseMode(uint32_t base, ADC_PulseMode mode)
{
    ASSERT(ADC_isBaseValid(base));

    EALLOW;

    if (mode == ADC_PULSE_END_OF_ACQ_WIN) {
        HW_BIT_CLR(base, ADC, CTL1, INTPULSEPOS);
    } else { /* mode == ADC_PULSE_END_OF_CONV */
        HW_BIT_SET(base, ADC, CTL1, INTPULSEPOS);
    }

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Powers up all analog circuitry inside the analog core
 * 
 * \param base base address of the ADC module
 * 
 * \details This function powers up all analog circuitry inside the analog core. Allow at least
 * 1000 us delay before sampling after calling this function. If you enable multiple ADCs, you
 * can delay after they all have begun powering up.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
ADC_powerUp(uint32_t base)
{
    ASSERT(ADC_isBaseValid(base));

    uint16_t mask = ADC_CTL1_ADCPWDN | ADC_CTL1_ADCBGPWD | ADC_CTL1_ADCREFPWD;

    EALLOW;

    HW_REG_MASK_SET(base, ADC, CTL1, mask);

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Powers down all analog circuitry inside the analog core
 * 
 * \param base base address of the ADC module
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
ADC_powerDown(uint32_t base)
{
    ASSERT(ADC_isBaseValid(base));

    uint16_t mask = ADC_CTL1_ADCPWDN | ADC_CTL1_ADCBGPWD | ADC_CTL1_ADCREFPWD;

    EALLOW;

    HW_REG_MASK_CLR(base, ADC, CTL1, mask);

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Enables ADC module
 * 
 * \param base base address of the ADC module
 * 
 * \details This function enables ADC module. Module must be enabled before an ADC conversion. It
 * is recommended that ADC module is enabled after setting ADC power-up bits in \e ADC_powerUp().
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
ADC_enableConverter(uint32_t base)
{
    ASSERT(ADC_isBaseValid(base));

    EALLOW;

    HW_BIT_SET(base, ADC, CTL1, ADCENABLE);

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Disables ADC module
 * 
 * \param base base address of the ADC module
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
ADC_disableConverter(uint32_t base)
{
    ASSERT(ADC_isBaseValid(base));

    EALLOW;

    HW_BIT_CLR(base, ADC, CTL1, ADCENABLE);

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Reset ADC module
 * 
 * \param base base address of the ADC module
 * 
 * \details This function causes a master reset on the entire ADC module. All register bits and
 * state machines are reset to the initial state as occurs when the device reset pin is pulled
 * low. The reset of ADC has latency of two clock cycles, ADC registers should not be modified
 * during that time. This bit automatically returns to 0, it does not need to be cleared. Once
 * ADC module has been reset, calibration values are restored from OTP.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
ADC_resetModule(uint32_t base)
{
    ASSERT(ADC_isBaseValid(base));

    EALLOW;

    HW_BIT_SET(base, ADC, CTL1, RESET);

    SysCtl_deviceCal();

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Forces single SOC flag
 * 
 * \param base base address of the ADC module
 * \param soc SOC wrapper number
 * 
 * \details This function forces the SOC flag associated with the SOC wrapper specified by the
 * \e soc parameter, which initiates a conversion once the SOC is given priority. The software
 * trigger can be used whether or not the SOC wrapper has been configured to accept some other
 * specific trigger.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
ADC_forceSOC(uint32_t base, ADC_SOCNumber soc)
{
    ASSERT(ADC_isBaseValid(base));

    HW_REG_SET_BIT(base, ADC, SOCFRC1, (uint16_t) soc);
}

/**************************************************************************************************
 * 
 * \brief Forces multiple SOC flags
 * 
 * \param base base address of the ADC module
 * \param mask SOCs to be forced through software
 * 
 * \details This function forces multiple SOC flags associated with SOC wrappers specified by the
 * \e mask parameter, which initiates a conversion once desired SOC wrappers are given priority.
 * The software trigger can be used whether or not SOC wrappers have been configured to accept
 * some other trigger. Valid values for the \e mask parameter is logical OR between any of the
 * \b ADC_FORCE_SOCX values.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
ADC_forceMultipleSOC(uint32_t base, uint16_t mask)
{
    ASSERT(ADC_isBaseValid(base));

    HW_REG_SET(base, ADC, SOCFRC1, mask);
}

/**************************************************************************************************
 * 
 * \brief Gets the ADC interrupt flag
 * 
 * \param base base address of the ADC module
 * \param number ADC interrupt number
 * 
 * \return \b true if the interrupt flag is set; \b false otherwise
 * 
 *************************************************************************************************/
static inline bool
ADC_getInterruptStatus(uint32_t base, ADC_IntNumber number)
{
    ASSERT(ADC_isBaseValid(base));

    return HW_REG_BIT_GET(base, ADC, INTFLG, (uint16_t) number);
}

/**************************************************************************************************
 * 
 * \brief Clears the ADC interrupt flag
 * 
 * \param base base address of the ADC module
 * \param number ADC interrupt number
 * 
 * \details This function clears the specified ADC interrupt flag so it is no longer asserted.
 * If not in continuous mode, this function must be called before any further interrupt pulses
 * may occur.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
ADC_clearInterruptStatus(uint32_t base, ADC_IntNumber number)
{
    ASSERT(ADC_isBaseValid(base));

    HW_REG_SET_BIT(base, ADC, INTFLGCLR, (uint16_t) number);
}

/**************************************************************************************************
 * 
 * \brief Gets the current ADC interrupt overflow status
 * 
 * \param base base address of the ADC module
 * \param number ADC interrupt number
 * 
 * \details This function returns the interrupt overflow status for the ADC converter. An overflow
 * condition is generated irrespective of the continuous mode.
 * 
 * \return \b true if the interrupt overflow flag is set; \b false otherwise
 * 
 *************************************************************************************************/
static inline bool
ADC_getInterruptOverflowStatus(uint32_t base, ADC_IntNumber number)
{
    ASSERT(ADC_isBaseValid(base));

    return HW_REG_BIT_GET(base, ADC, INTOVF, (uint16_t) number);
}

/**************************************************************************************************
 * 
 * \brief Clears the ADC interrupt overflow flag
 * 
 * \param base base address of the ADC module
 * \param number ADC interrupt number
 * 
 * \details This function clears the specified ADC interrupt overflow flag so it is no longer
 * asserted. If software tries to clear the flag in the same cycle that hardware tries to set
 * the flag, the hardware has priority.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
ADC_clearInterruptOverflowStatus(uint32_t base, ADC_IntNumber number)
{
    ASSERT(ADC_isBaseValid(base));

    HW_REG_SET_BIT(base, ADC, INTOVFCLR, (uint16_t) number);
}

/**************************************************************************************************
 * 
 * \brief Reads the ADC conversion result
 * 
 * \param base base address of the ADC result
 * \param soc SOC wrapper number
 * 
 * \note Make sure that you are using the result register base address \b ADCxRESULT_BASE and not
 * the control register base address.
 * 
 * \return 12-bit conversion result
 * 
 *************************************************************************************************/
static inline uint16_t
ADC_readResult(uint32_t base, ADC_SOCNumber soc)
{
    ASSERT(ADC_isResultBaseValid(base));

    uint32_t offset = (uint32_t) soc;

    return HW_REG_GETO(base, offset, ADC, RESULTx);
}

/**************************************************************************************************
 * 
 * \brief Returns ADC busy status
 * 
 * \param base base address of the ADC module
 * 
 * \details This function allows the caller to determine whether or not the ADC is available or
 * busy. The ADC becomes available after:
 * - 4 ADC clocks after negative edge of S/H pulse in \b sequential mode
 * - 14 ADC clocks after negative edge of S/H pulse in \b simultaneous mode
 * 
 * \return \b true if busy and cannot sample another channel; \b false if available to sample
 * next channel
 * 
 *************************************************************************************************/
static inline bool
ADC_isBusy(uint32_t base)
{
    ASSERT(ADC_isBaseValid(base));

    return HW_BIT_GET(base, ADC, CTL1, ADCBSY);
}

/**************************************************************************************************
 * 
 * \brief Returns last or current SOC being processed
 * 
 * \param base base address of the ADC module
 * 
 * \details If ADC is busy it returns SOC currently being processed. If ADC is available, it
 * returns last converted SOC.
 * 
 * \return SOC wrapper number
 * 
 *************************************************************************************************/
static inline uint16_t
ADC_getLastSOC(uint32_t base)
{
    ASSERT(ADC_isBaseValid(base));

    return HW_FLD_GET(base, ADC, CTL1, ADCBSYCHN);
}

/**************************************************************************************************
 * 
 * \brief Returns SOC status
 * 
 * \param base base address of the ADC module
 * \param soc SOC wrapper number
 * 
 * \return \b true when trigger has been received and sample is pending for SOC; \b false otherwise
 * 
 *************************************************************************************************/
static inline bool
ADC_getSOCStatus(uint32_t base, ADC_SOCNumber soc)
{
    ASSERT(ADC_isBaseValid(base));

    return HW_REG_BIT_GET(base, ADC, SOCFLG1, (uint16_t) soc);
}

/**************************************************************************************************
 * 
 * \brief Returns SOC overflow flag status
 * 
 * \param base base address of the ADC module
 * \param soc SOC wrapper number
 * 
 * \details This function returns SOC overflow flag status. The flag is set if new trigger occurs
 * while previous SOC is still pending. It does not stop SOC events from being processed.
 * 
 * \return \b true when overflow flag is set; \b false otherwise
 * 
 *************************************************************************************************/
static inline bool
ADC_getSOCOverflowStatus(uint32_t base, ADC_SOCNumber soc)
{
    ASSERT(ADC_isBaseValid(base));

    return HW_REG_BIT_GET(base, ADC, SOCOVF1, (uint16_t) soc);
}

/**************************************************************************************************
 * 
 * \brief Clears SOC overflow flag
 * 
 * \param base base address of the ADC module
 * \param soc SOC wrapper number
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
ADC_clearSOCOverflowStatus(uint32_t base, ADC_SOCNumber soc)
{
    ASSERT(ADC_isBaseValid(base));

    HW_REG_SET_BIT(base, ADC, SOCOVFCLR1, (uint16_t) soc);
}

/**************************************************************************************************
 * 
 * \brief Sets the priority mode of the SOC wrappers
 * 
 * \param base base address of the ADC module
 * \param mode SOC wrapper priority mode
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
ADC_setSOCPriority(uint32_t base, ADC_PriorityMode mode)
{
    ASSERT(ADC_isBaseValid(base));

    EALLOW;

    HW_FLD_SET(base, ADC, SOCPRICTL, SOCPRIORITY, mode);

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Returns the last converted round robin SOC wrapper
 * 
 * \param base base address of the ADC module
 * 
 * \note Reset value to indicate no SOC has been converted is 0x20U. In that case, SOC0 is highest
 * round robin priority.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline uint16_t
ADC_getSOCRoundRobin(uint32_t base)
{
    ASSERT(ADC_isBaseValid(base));

    return HW_FLD_GET(base, ADC, SOCPRICTL, RRPOINTER);
}

/**************************************************************************************************
 * 
 * \brief Enable one shot mode
 * 
 * \param base base address of the ADC module
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
ADC_enableSOCOneShot(uint32_t base)
{
    ASSERT(ADC_isBaseValid(base));

    EALLOW;

    HW_BIT_SET(base, ADC, SOCPRICTL, ONESHOT);

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Disable one shot mode
 * 
 * \param base base address of the ADC module
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
ADC_disableSOCOneShot(uint32_t base)
{
    ASSERT(ADC_isBaseValid(base));

    EALLOW;

    HW_BIT_CLR(base, ADC, SOCPRICTL, ONESHOT);

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Enables simultaneous sampling on the selected SOC pairs
 * 
 * \param base base address of the ADC module
 * \param mask SOC wrapper pairs
 * 
 * \details This function enables simultaneous sampling on the selected SOC pairs. The \e mask
 * parameter is logical OR between any of the \b ADC_SAMPLE_PAIR_SOCX_Y constants.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
ADC_enableSimultaneousSampling(uint32_t base, uint16_t mask)
{
    ASSERT(ADC_isBaseValid(base));

    EALLOW;

    HW_REG_MASK_SET(base, ADC, SAMPLEMODE, mask);

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Disables simultaneous sampling on the selected SOC pair
 * 
 * \param base base address of the ADC module
 * \param mask SOC wrapper pairs
 * 
 * \details This function disables simultaneous sampling on the selected SOC pairs. The \e mask
 * parameter is logical OR between any of the \b ADC_SAMPLE_PAIR_SOCX_Y constants.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
ADC_disableSimultaneousSampling(uint32_t base, uint16_t mask)
{
    ASSERT(ADC_isBaseValid(base));

    EALLOW;

    HW_REG_MASK_CLR(base, ADC, SAMPLEMODE, mask);

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Enables ADC interrupt source
 * 
 * \param base base address of the ADC module
 * \param number ADC interrupt number
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
ADC_enableInterrupt(uint32_t base, ADC_IntNumber number)
{
    ASSERT(ADC_isBaseValid(base));

    uint32_t offset = (uint32_t) number >> 1;
    
    bool xy = ((uint16_t) number & 0x1U) == 0U;

    EALLOW;

    if (xy) {
        HW_BIT_SETO(base, offset, ADC, INTSELxNy, INTxE);
    } else { /* !xy */
        HW_BIT_SETO(base, offset, ADC, INTSELxNy, INTyE);
    }

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Disables ADC interrupt source
 * 
 * \param base base address of the ADC module
 * \param number ADC interrupt number
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
ADC_disableInterrupt(uint32_t base, ADC_IntNumber number)
{
    ASSERT(ADC_isBaseValid(base));

    uint32_t offset = (uint32_t) number >> 1;
    
    bool xy = ((uint16_t) number & 0x1U) == 0U;

    EALLOW;

    if (xy) {
        HW_BIT_CLRO(base, offset, ADC, INTSELxNy, INTxE);
    } else { /* !xy */
        HW_BIT_CLRO(base, offset, ADC, INTSELxNy, INTyE);
    }

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Sets the source EOC for ADC converter interrupt
 * 
 * \param base base address of the ADC module
 * \param number ADC interrupt number
 * \param soc SOC wrapper number
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
ADC_setInterruptSource(uint32_t base, ADC_IntNumber number, ADC_SOCNumber soc)
{
    ASSERT(ADC_isBaseValid(base));

    uint32_t offset = (uint32_t) number >> 1;

    bool xy = ((uint16_t) number & 0x1U) == 0U;

    EALLOW;

    if (xy) {
        HW_FLD_SETO(base, offset, ADC, INTSELxNy, INTxSEL, soc);
    } else { /* !xy */
        HW_FLD_SETO(base, offset, ADC, INTSELxNy, INTySEL, soc);
    }

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Enables continuous mode for an ADC interrupt
 * 
 * \param base base address of the ADC module
 * \param number ADC interrupt number
 * 
 * \details This function enables continuous mode for an ADC interrupt defined by the \e number
 * parameter. This means that pulses will be generated for the specified ADC interrupt whenever
 * an EOC pulse is generated irrespective of whether or not the flag bit is set.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
ADC_enableContinuousMode(uint32_t base, ADC_IntNumber number)
{
    ASSERT(ADC_isBaseValid(base));

    uint32_t offset = (uint32_t) number >> 1;

    bool xy = ((uint16_t) number & 0x1U) == 0U;

    EALLOW;

    if (xy) {
        HW_BIT_SETO(base, offset, ADC, INTSELxNy, INTxCONT);
    } else { /* !xy */
        HW_BIT_SETO(base, offset, ADC, INTSELxNy, INTyCONT);
    }

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Disables continuous mode for an ADC interrupt
 * 
 * \param base base address of the ADC module
 * \param number ADC interrupt number
 * 
 * \details This function disables continuous mode for an ADC interrupt defined by the \e number
 * parameter. This means that pulses will not be generated until the corresponding \b ADCINTx
 * flag is cleared by software.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
ADC_disableContinuousMode(uint32_t base, ADC_IntNumber number)
{
    ASSERT(ADC_isBaseValid(base));

    uint32_t offset = (uint32_t) number >> 1;

    bool xy = ((uint16_t) number & 0x1U) == 0U;

    EALLOW;

    if (xy) {
        HW_BIT_CLRO(base, offset, ADC, INTSELxNy, INTxCONT);
    } else { /* !xy */
        HW_BIT_CLRO(base, offset, ADC, INTSELxNy, INTyCONT);
    }

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Sets offset trim
 * 
 * \param base base address of the ADC module
 * \param offset offset trim value
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
ADC_setOffsetTrim(uint32_t base, uint16_t offset)
{
    ASSERT(ADC_isBaseValid(base));

    EALLOW;

    HW_FLD_SET(base, ADC, OFFTRIM, OFFTRIM, offset);

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Internally connects VREFLO to the ADC channel B5
 * 
 * \param base base address of the ADC module
 * 
 * \details This function internally connects VREFLO to the ADC channel B5 and disconnects the
 * ADCINB5 pin from the ADC.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
ADC_connectVREFLO(uint32_t base)
{
    ASSERT(ADC_isBaseValid(base));

    EALLOW;

    HW_BIT_SET(base, ADC, CTL1, VREFLOCONV);

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Internally disconnects VREFLO from the ADC channel B5
 * 
 * \param base base address of the ADC module
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
ADC_disconnectVREFLO(uint32_t base)
{
    ASSERT(ADC_isBaseValid(base));

    EALLOW;

    HW_BIT_CLR(base, ADC, CTL1, VREFLOCONV);

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Internally connects temperature sensor to the ADC channel A5
 * 
 * \param base base address of the ADC module
 * 
 * \details This function internally connects temperature sensor to the ADC channel A5 and
 * disconnects the ADCINA5 pin from the ADC.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
ADC_connectTempSensor(uint32_t base)
{
    ASSERT(ADC_isBaseValid(base));

    EALLOW;

    HW_BIT_SET(base, ADC, CTL1, TEMPCONV);

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Internally disconnects temperature sensor from the ADC channel A5
 * 
 * \param base base address of the ADC module
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
ADC_disconnectTempSensor(uint32_t base)
{
    ASSERT(ADC_isBaseValid(base));

    EALLOW;

    HW_BIT_CLR(base, ADC, CTL1, TEMPCONV);

    EDIS;
}

/**************************************************************************************************
 * 
 * \brief Returns ADC module revision
 * 
 * \param base base address of the ADC module
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline uint16_t
ADC_getRevision(uint32_t base)
{
    ASSERT(ADC_isBaseValid(base));

    return HW_FLD_GET(base, ADC, REV, REV);
}

/**************************************************************************************************
 * 
 * \brief Returns ADC module type
 * 
 * \param base base address of the ADC module
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline uint16_t
ADC_getType(uint32_t base)
{
    ASSERT(ADC_isBaseValid(base));

    return HW_FLD_GET(base, ADC, REV, TYPE);
}

/**************************************************************************************************
 * 
 * \brief Configures ADC module reference mode
 * 
 * \param base base address of the ADC module
 * \param mode ADC reference mode
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
ADC_setVREF(uint32_t base, ADC_ReferenceMode mode);

#endif /* ADC_H */
