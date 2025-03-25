/**************************************************************************************************
 * 
 * \file adc.h
 * 
 * \brief ADC driver interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_DRV_ADC_H
#define _INC_DRV_ADC_H

#include "driverlib/adc.h"

#include "cgen/drv/adc.h"
#include "inc/api/obj.h"

#include <stdint.h>

/**************************************************************************************************
 * 
 * Forward declarations
 * 
 *************************************************************************************************/

struct _adc;

/**************************************************************************************************
 * 
 * Private variables definition
 * 
 *************************************************************************************************/
OBJ_PRIV_STRUCT(_adc_priv,
    uint16_t init;                      /* Bit mask which indicates if ADC core has been setup  */
);


/**************************************************************************************************
 * 
 * Trigger source + SOC slot definition
 * This enum must be used to by the user to pass to the ADC module the different combinations of
 * SOC source + SOC number allocation for a given ADC variable.
 * 
 * MSB byte is the trigger source, and LSB byte is the soc number allocation
 * 
 *************************************************************************************************/
enum trigger_soc{
    TRIG_EPWM1_SOCA_N0 = 0x0500,
    TRIG_EPWM1_SOCA_N1 = 0x0501,
    TRIG_EPWM1_SOCA_N2 = 0x0502,
    TRIG_EPWM1_SOCA_N3 = 0x0503,
    TRIG_EPWM1_SOCA_N4 = 0x0504,
    TRIG_EPWM1_SOCA_N5 = 0x0505,
    TRIG_EPWM1_SOCA_N6 = 0x0506,
    TRIG_EPWM1_SOCA_N7 = 0x0507,
    TRIG_EPWM1_SOCA_N8 = 0x0508,
    TRIG_EPWM1_SOCA_N9 = 0x0509,
    TRIG_EPWM1_SOCA_N10 = 0x050A,
    TRIG_EPWM1_SOCA_N11 = 0x050B,
    TRIG_EPWM1_SOCA_N12 = 0x050C,
    TRIG_EPWM1_SOCA_N13 = 0x050D,
    TRIG_EPWM1_SOCA_N14 = 0x050E,
    TRIG_EPWM1_SOCA_N15 = 0x050F,

    TRIG_EPWM1_SOCB_N0 = 0x0600,
    TRIG_EPWM1_SOCB_N1 = 0x0601,
    TRIG_EPWM1_SOCB_N2 = 0x0602,
    TRIG_EPWM1_SOCB_N3 = 0x0603,
    TRIG_EPWM1_SOCB_N4 = 0x0604,
    TRIG_EPWM1_SOCB_N5 = 0x0605,
    TRIG_EPWM1_SOCB_N6 = 0x0606,
    TRIG_EPWM1_SOCB_N7 = 0x0607,
    TRIG_EPWM1_SOCB_N8 = 0x0608,
    TRIG_EPWM1_SOCB_N9 = 0x0609,
    TRIG_EPWM1_SOCB_N10 = 0x060A,
    TRIG_EPWM1_SOCB_N11 = 0x060B,
    TRIG_EPWM1_SOCB_N12 = 0x060C,
    TRIG_EPWM1_SOCB_N13 = 0x060D,
    TRIG_EPWM1_SOCB_N14 = 0x060E,
    TRIG_EPWM1_SOCB_N15 = 0x060F,

    TRIG_EPWM2_SOCA_N0 = 0x0700,
    TRIG_EPWM2_SOCA_N1 = 0x0701,
    TRIG_EPWM2_SOCA_N2 = 0x0702,
    TRIG_EPWM2_SOCA_N3 = 0x0703,
    TRIG_EPWM2_SOCA_N4 = 0x0704,
    TRIG_EPWM2_SOCA_N5 = 0x0705,
    TRIG_EPWM2_SOCA_N6 = 0x0706,
    TRIG_EPWM2_SOCA_N7 = 0x0707,
    TRIG_EPWM2_SOCA_N8 = 0x0708,
    TRIG_EPWM2_SOCA_N9 = 0x0709,
    TRIG_EPWM2_SOCA_N10 = 0x070A,
    TRIG_EPWM2_SOCA_N11 = 0x070B,
    TRIG_EPWM2_SOCA_N12 = 0x070C,
    TRIG_EPWM2_SOCA_N13 = 0x070D,
    TRIG_EPWM2_SOCA_N14 = 0x070E,
    TRIG_EPWM2_SOCA_N15 = 0x070F,

    TRIG_EPWM2_SOCB_N0 = 0x0800,
    TRIG_EPWM2_SOCB_N1 = 0x0801,
    TRIG_EPWM2_SOCB_N2 = 0x0802,
    TRIG_EPWM2_SOCB_N3 = 0x0803,
    TRIG_EPWM2_SOCB_N4 = 0x0804,
    TRIG_EPWM2_SOCB_N5 = 0x0805,
    TRIG_EPWM2_SOCB_N6 = 0x0806,
    TRIG_EPWM2_SOCB_N7 = 0x0807,
    TRIG_EPWM2_SOCB_N8 = 0x0808,
    TRIG_EPWM2_SOCB_N9 = 0x0809,
    TRIG_EPWM2_SOCB_N10 = 0x080A,
    TRIG_EPWM2_SOCB_N11 = 0x080B,
    TRIG_EPWM2_SOCB_N12 = 0x080C,
    TRIG_EPWM2_SOCB_N13 = 0x080D,
    TRIG_EPWM2_SOCB_N14 = 0x080E,
    TRIG_EPWM2_SOCB_N15 = 0x080F,

    TRIG_EPWM3_SOCA_N0 = 0x0900,
    TRIG_EPWM3_SOCA_N1 = 0x0901,
    TRIG_EPWM3_SOCA_N2 = 0x0902,
    TRIG_EPWM3_SOCA_N3 = 0x0903,
    TRIG_EPWM3_SOCA_N4 = 0x0904,
    TRIG_EPWM3_SOCA_N5 = 0x0905,
    TRIG_EPWM3_SOCA_N6 = 0x0906,
    TRIG_EPWM3_SOCA_N7 = 0x0907,
    TRIG_EPWM3_SOCA_N8 = 0x0908,
    TRIG_EPWM3_SOCA_N9 = 0x0909,
    TRIG_EPWM3_SOCA_N10 = 0x090A,
    TRIG_EPWM3_SOCA_N11 = 0x090B,
    TRIG_EPWM3_SOCA_N12 = 0x090C,
    TRIG_EPWM3_SOCA_N13 = 0x090D,
    TRIG_EPWM3_SOCA_N14 = 0x090E,
    TRIG_EPWM3_SOCA_N15 = 0x090F,

    TRIG_EPWM3_SOCB_N0 = 0x0A00,
    TRIG_EPWM3_SOCB_N1 = 0x0A01,
    TRIG_EPWM3_SOCB_N2 = 0x0A02,
    TRIG_EPWM3_SOCB_N3 = 0x0A03,
    TRIG_EPWM3_SOCB_N4 = 0x0A04,
    TRIG_EPWM3_SOCB_N5 = 0x0A05,
    TRIG_EPWM3_SOCB_N6 = 0x0A06,
    TRIG_EPWM3_SOCB_N7 = 0x0A07,
    TRIG_EPWM3_SOCB_N8 = 0x0A08,
    TRIG_EPWM3_SOCB_N9 = 0x0A09,
    TRIG_EPWM3_SOCB_N10 = 0x0A0A,
    TRIG_EPWM3_SOCB_N11 = 0x0A0B,
    TRIG_EPWM3_SOCB_N12 = 0x0A0C,
    TRIG_EPWM3_SOCB_N13 = 0x0A0D,
    TRIG_EPWM3_SOCB_N14 = 0x0A0E,
    TRIG_EPWM3_SOCB_N15 = 0x0A0F,

    TRIG_EPWM4_SOCA_N0 = 0x0B00,
    TRIG_EPWM4_SOCA_N1 = 0x0B01,
    TRIG_EPWM4_SOCA_N2 = 0x0B02,
    TRIG_EPWM4_SOCA_N3 = 0x0B03,
    TRIG_EPWM4_SOCA_N4 = 0x0B04,
    TRIG_EPWM4_SOCA_N5 = 0x0B05,
    TRIG_EPWM4_SOCA_N6 = 0x0B06,
    TRIG_EPWM4_SOCA_N7 = 0x0B07,
    TRIG_EPWM4_SOCA_N8 = 0x0B08,
    TRIG_EPWM4_SOCA_N9 = 0x0B09,
    TRIG_EPWM4_SOCA_N10 = 0x0B0A,
    TRIG_EPWM4_SOCA_N11 = 0x0B0B,
    TRIG_EPWM4_SOCA_N12 = 0x0B0C,
    TRIG_EPWM4_SOCA_N13 = 0x0B0D,
    TRIG_EPWM4_SOCA_N14 = 0x0B0E,
    TRIG_EPWM4_SOCA_N15 = 0x0B0F,

    TRIG_EPWM4_SOCB_N0 = 0x0C00,
    TRIG_EPWM4_SOCB_N1 = 0x0C01,
    TRIG_EPWM4_SOCB_N2 = 0x0C02,
    TRIG_EPWM4_SOCB_N3 = 0x0C03,
    TRIG_EPWM4_SOCB_N4 = 0x0C04,
    TRIG_EPWM4_SOCB_N5 = 0x0C05,
    TRIG_EPWM4_SOCB_N6 = 0x0C06,
    TRIG_EPWM4_SOCB_N7 = 0x0C07,
    TRIG_EPWM4_SOCB_N8 = 0x0C08,
    TRIG_EPWM4_SOCB_N9 = 0x0C09,
    TRIG_EPWM4_SOCB_N10 = 0x0C0A,
    TRIG_EPWM4_SOCB_N11 = 0x0C0B,
    TRIG_EPWM4_SOCB_N12 = 0x0C0C,
    TRIG_EPWM4_SOCB_N13 = 0x0C0D,
    TRIG_EPWM4_SOCB_N14 = 0x0C0E,
    TRIG_EPWM4_SOCB_N15 = 0x0C0F,

    TRIG_EPWM5_SOCA_N0 = 0x0D00,
    TRIG_EPWM5_SOCA_N1 = 0x0D01,
    TRIG_EPWM5_SOCA_N2 = 0x0D02,
    TRIG_EPWM5_SOCA_N3 = 0x0D03,
    TRIG_EPWM5_SOCA_N4 = 0x0D04,
    TRIG_EPWM5_SOCA_N5 = 0x0D05,
    TRIG_EPWM5_SOCA_N6 = 0x0D06,
    TRIG_EPWM5_SOCA_N7 = 0x0D07,
    TRIG_EPWM5_SOCA_N8 = 0x0D08,
    TRIG_EPWM5_SOCA_N9 = 0x0D09,
    TRIG_EPWM5_SOCA_N10 = 0x0D0A,
    TRIG_EPWM5_SOCA_N11 = 0x0D0B,
    TRIG_EPWM5_SOCA_N12 = 0x0D0C,
    TRIG_EPWM5_SOCA_N13 = 0x0D0D,
    TRIG_EPWM5_SOCA_N14 = 0x0D0E,
    TRIG_EPWM5_SOCA_N15 = 0x0D0F,

    TRIG_EPWM5_SOCB_N0 = 0x0E00,
    TRIG_EPWM5_SOCB_N1 = 0x0E01,
    TRIG_EPWM5_SOCB_N2 = 0x0E02,
    TRIG_EPWM5_SOCB_N3 = 0x0E03,
    TRIG_EPWM5_SOCB_N4 = 0x0E04,
    TRIG_EPWM5_SOCB_N5 = 0x0E05,
    TRIG_EPWM5_SOCB_N6 = 0x0E06,
    TRIG_EPWM5_SOCB_N7 = 0x0E07,
    TRIG_EPWM5_SOCB_N8 = 0x0E08,
    TRIG_EPWM5_SOCB_N9 = 0x0E09,
    TRIG_EPWM5_SOCB_N10 = 0x0E0A,
    TRIG_EPWM5_SOCB_N11 = 0x0E0B,
    TRIG_EPWM5_SOCB_N12 = 0x0E0C,
    TRIG_EPWM5_SOCB_N13 = 0x0E0D,
    TRIG_EPWM5_SOCB_N14 = 0x0E0E,
    TRIG_EPWM5_SOCB_N15 = 0x0E0F,

    TRIG_EPWM6_SOCA_N0 = 0x0F00,
    TRIG_EPWM6_SOCA_N1 = 0x0F01,
    TRIG_EPWM6_SOCA_N2 = 0x0F02,
    TRIG_EPWM6_SOCA_N3 = 0x0F03,
    TRIG_EPWM6_SOCA_N4 = 0x0F04,
    TRIG_EPWM6_SOCA_N5 = 0x0F05,
    TRIG_EPWM6_SOCA_N6 = 0x0F06,
    TRIG_EPWM6_SOCA_N7 = 0x0F07,
    TRIG_EPWM6_SOCA_N8 = 0x0F08,
    TRIG_EPWM6_SOCA_N9 = 0x0F09,
    TRIG_EPWM6_SOCA_N10 = 0x0F0A,
    TRIG_EPWM6_SOCA_N11 = 0x0F0B,
    TRIG_EPWM6_SOCA_N12 = 0x0F0C,
    TRIG_EPWM6_SOCA_N13 = 0x0F0D,
    TRIG_EPWM6_SOCA_N14 = 0x0F0E,
    TRIG_EPWM6_SOCA_N15 = 0x0F0F,

    TRIG_EPWM6_SOCB_N0 = 0x1000,
    TRIG_EPWM6_SOCB_N1 = 0x1001,
    TRIG_EPWM6_SOCB_N2 = 0x1002,
    TRIG_EPWM6_SOCB_N3 = 0x1003,
    TRIG_EPWM6_SOCB_N4 = 0x1004,
    TRIG_EPWM6_SOCB_N5 = 0x1005,
    TRIG_EPWM6_SOCB_N6 = 0x1006,
    TRIG_EPWM6_SOCB_N7 = 0x1007,
    TRIG_EPWM6_SOCB_N8 = 0x1008,
    TRIG_EPWM6_SOCB_N9 = 0x1009,
    TRIG_EPWM6_SOCB_N10 = 0x100A,
    TRIG_EPWM6_SOCB_N11 = 0x100B,
    TRIG_EPWM6_SOCB_N12 = 0x100C,
    TRIG_EPWM6_SOCB_N13 = 0x100D,
    TRIG_EPWM6_SOCB_N14 = 0x100E,
    TRIG_EPWM6_SOCB_N15 = 0x100F,

    TRIG_EPWM7_SOCA_N0 = 0x1100,
    TRIG_EPWM7_SOCA_N1 = 0x1101,
    TRIG_EPWM7_SOCA_N2 = 0x1102,
    TRIG_EPWM7_SOCA_N3 = 0x1103,
    TRIG_EPWM7_SOCA_N4 = 0x1104,
    TRIG_EPWM7_SOCA_N5 = 0x1105,
    TRIG_EPWM7_SOCA_N6 = 0x1106,
    TRIG_EPWM7_SOCA_N7 = 0x1107,
    TRIG_EPWM7_SOCA_N8 = 0x1108,
    TRIG_EPWM7_SOCA_N9 = 0x1109,
    TRIG_EPWM7_SOCA_N10 = 0x110A,
    TRIG_EPWM7_SOCA_N11 = 0x110B,
    TRIG_EPWM7_SOCA_N12 = 0x110C,
    TRIG_EPWM7_SOCA_N13 = 0x110D,
    TRIG_EPWM7_SOCA_N14 = 0x110E,
    TRIG_EPWM7_SOCA_N15 = 0x110F,

    TRIG_EPWM7_SOCB_N0 = 0x1200,
    TRIG_EPWM7_SOCB_N1 = 0x1201,
    TRIG_EPWM7_SOCB_N2 = 0x1202,
    TRIG_EPWM7_SOCB_N3 = 0x1203,
    TRIG_EPWM7_SOCB_N4 = 0x1204,
    TRIG_EPWM7_SOCB_N5 = 0x1205,
    TRIG_EPWM7_SOCB_N6 = 0x1206,
    TRIG_EPWM7_SOCB_N7 = 0x1207,
    TRIG_EPWM7_SOCB_N8 = 0x1208,
    TRIG_EPWM7_SOCB_N9 = 0x1209,
    TRIG_EPWM7_SOCB_N10 = 0x120A,
    TRIG_EPWM7_SOCB_N11 = 0x120B,
    TRIG_EPWM7_SOCB_N12 = 0x120C,
    TRIG_EPWM7_SOCB_N13 = 0x120D,
    TRIG_EPWM7_SOCB_N14 = 0x120E,
    TRIG_EPWM7_SOCB_N15 = 0x120F,

    #ifdef F28004X
    TRIG_EPWM8_SOCA_N0 = 0x1300,
    TRIG_EPWM8_SOCA_N1 = 0x1301,
    TRIG_EPWM8_SOCA_N2 = 0x1302,
    TRIG_EPWM8_SOCA_N3 = 0x1303,
    TRIG_EPWM8_SOCA_N4 = 0x1304,
    TRIG_EPWM8_SOCA_N5 = 0x1305,
    TRIG_EPWM8_SOCA_N6 = 0x1306,
    TRIG_EPWM8_SOCA_N7 = 0x1307,
    TRIG_EPWM8_SOCA_N8 = 0x1308,
    TRIG_EPWM8_SOCA_N9 = 0x1309,
    TRIG_EPWM8_SOCA_N10 = 0x130A,
    TRIG_EPWM8_SOCA_N11 = 0x130B,
    TRIG_EPWM8_SOCA_N12 = 0x130C,
    TRIG_EPWM8_SOCA_N13 = 0x130D,
    TRIG_EPWM8_SOCA_N14 = 0x130E,
    TRIG_EPWM8_SOCA_N15 = 0x130F,

    TRIG_EPWM8_SOCB_N0 = 0x1400,
    TRIG_EPWM8_SOCB_N1 = 0x1401,
    TRIG_EPWM8_SOCB_N2 = 0x1402,
    TRIG_EPWM8_SOCB_N3 = 0x1403,
    TRIG_EPWM8_SOCB_N4 = 0x1404,
    TRIG_EPWM8_SOCB_N5 = 0x1405,
    TRIG_EPWM8_SOCB_N6 = 0x1406,
    TRIG_EPWM8_SOCB_N7 = 0x1407,
    TRIG_EPWM8_SOCB_N8 = 0x1408,
    TRIG_EPWM8_SOCB_N9 = 0x1409,
    TRIG_EPWM8_SOCB_N10 = 0x140A,
    TRIG_EPWM8_SOCB_N11 = 0x140B,
    TRIG_EPWM8_SOCB_N12 = 0x140C,
    TRIG_EPWM8_SOCB_N13 = 0x140D,
    TRIG_EPWM8_SOCB_N14 = 0x140E,
    TRIG_EPWM8_SOCB_N15 = 0x140F
    #endif
};

/**************************************************************************************************
 * 
 * ADC variable definition
 * 
 *************************************************************************************************/
struct _adc_var {
    bool enable;                        /* Enables ADC variable if true                         */
    uint16_t core;                      /* ADC core index (1 is ADCA, 2 is ADCB etc.)           */
    uint32_t base;                      /* ADC module base address                              */
    uint32_t result;                    /* ADC result base address                              */
    ADC_Channel channel;                /* Channel associated with ADC variable                 */
    ADC_Trigger trigger;                /* Start of conversion trigger                          */
    uint32_t sample;                    /* Sampling time (ns)                                   */
    uint16_t int_n;                     /* Interrupt line number                                */
    uint16_t count;                     /* Number of SOC wrappers in the list                   */
    const uint16_t *soc;                /* List with SOC wrappers for the channel               */
};

/**************************************************************************************************
 * 
 * \brief Configures all ADC variables in the ADC object
 * 
 * \param adc ADC object handler
 * 
 * \return 0 if operations is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
adc_setup(const struct _adc *adc);

#endif /* _INC_DRV_ADC_H */
