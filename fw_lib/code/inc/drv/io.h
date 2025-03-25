/**************************************************************************************************
 * 
 * \file io.h
 * 
 * \brief I/O driver interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_DRV_IO_H
#define _INC_DRV_IO_H

#include <stdint.h>
#include <stdbool.h>

/**************************************************************************************************
 * 
 * I/O pin configuration code definition. The data is encoded as follows:
 * - 0x000000FF     GPIO pin number
 * - 0x00000F00     (G)MUX register value
 * - 0x0FFFF000     Peripheral function code (see 'enum io_fun')
 * - 0x40000000     True when I/O pin is connected
 * - 0x80000000     True when I/O pin is available
 * 
 *************************************************************************************************/

#if defined(F28P65X)
enum io {
        IOX                  = 0x00000000U,

        IO0_DIN              = 0xC0020000U,
        IO0_DOUT             = 0xC0021000U,
        IO0_EPWM1_A          = 0xC0080100U,
        IO0_I2CA_SDA         = 0xC0040600U,
        IO0_FSITXA_D0        = 0xC0100D00U,

        IO1_DIN              = 0xC0020001U,
        IO1_DOUT             = 0xC0021001U,
        IO1_EPWM1_B          = 0xC0081101U,
        IO1_I2CA_SCL         = 0xC0041601U,
        IO1_FSITXA_D1        = 0xC0101D01U,

        IO2_DIN              = 0xC0020002U,
        IO2_DOUT             = 0xC0021002U,
        IO2_EPWM2_A          = 0xC0082102U,
        IO2_FSITXA_CLK       = 0xC0102D02U,

        IO3_DIN              = 0xC0020003U,
        IO3_DOUT             = 0xC0021003U,
        IO3_EPWM2_B          = 0xC0083103U,
        IO3_FSIRXA_D0        = 0xC0103D03U,

        IO4_DIN              = 0xC0020004U,
        IO4_DOUT             = 0xC0021004U,
        IO4_EPWM3_A          = 0xC0084104U,
        IO4_CANA_TX          = 0xC0030604U,
        IO4_FSIRXA_D1        = 0xC0104D04U,

        IO5_DIN              = 0xC0020005U,
        IO5_DOUT             = 0xC0021005U,
        IO5_EPWM3_B          = 0xC0085105U,
        IO5_CANA_RX          = 0xC0031605U,
        IO5_FSIRXA_CLK       = 0xC0105D05U,

        IO6_DIN              = 0xC0020006U,
        IO6_DOUT             = 0xC0021006U,
        IO6_EPWM4_A          = 0xC0086106U,
        IO6_LINA_TX          = 0xC0060706U,

        IO7_DIN              = 0xC0020007U,
        IO7_DOUT             = 0xC0021007U,
        IO7_EPWM4_B          = 0xC0087107U,
        IO7_LINA_RX          = 0xC0061707U,

        IO8_DIN              = 0xC0020008U,
        IO8_DOUT             = 0xC0021008U,
        IO8_EPWM5_A          = 0xC0088108U,
        IO8_ADCSOCAO         = 0xC0007308U,
        IO8_SCIA_TX          = 0xC0050608U,
        IO8_FSITXA_D1        = 0xC0101E08U,
        IO8_FSIRXA_D0        = 0xC0103F08U,

        IO9_DIN              = 0xC0020009U,
        IO9_DOUT             = 0xC0021009U,
        IO9_EPWM5_B          = 0xC0089109U,
        IO9_SCIB_TX          = 0xC0052209U,
        IO9_SCIA_RX          = 0xC0051609U,
        IO9_FSITXA_D0        = 0xC0100E09U,
        IO9_FSIRXA_CLK       = 0xC0105F09U,

        IO10_DIN             = 0xC002000AU,
        IO10_DOUT            = 0xC002100AU,
        IO10_EPWM6_A         = 0xC008A10AU,
        IO10_ADCSOCBO        = 0xC000830AU,
        IO10_EQEP1_A         = 0xC00D050AU,
        IO10_SCIB_TX         = 0xC005260AU,
        IO10_FSITXA_CLK      = 0xC0102E0AU,
        IO10_FSIRXA_D1       = 0xC0104F0AU,

        IO11_DIN             = 0xC002000BU,
        IO11_DOUT            = 0xC002100BU,
        IO11_EPWM6_B         = 0xC008B10BU,
        IO11_SCIB_RX         = 0xC005320BU,
        IO11_EQEP1_B         = 0xC00D150BU,
        IO11_FSIRXA_D1       = 0xC0104E0BU,
        IO11_PMBUSA_ALERT    = 0xC0113F0BU,

        IO12_DIN             = 0xC002000CU,
        IO12_DOUT            = 0xC002100CU,
        IO12_EPWM7_A         = 0xC008C10CU,
        IO12_ADCSOCAO        = 0xC000730CU,
        IO12_EQEP1_STROBE    = 0xC00D350CU,
        IO12_SCIA_TX         = 0xC005060CU,
        IO12_FSIRXA_D0       = 0xC0103E0CU,
        IO12_PMBUSA_CTL      = 0xC0112F0CU,

        IO13_DIN             = 0xC002000DU,
        IO13_DOUT            = 0xC002100DU,
        IO13_EPWM7_B         = 0xC008D10DU,
        IO13_EQEP1_INDEX     = 0xC00D250DU,
        IO13_SCIA_RX         = 0xC005160DU,
        IO13_FSIRXA_CLK      = 0xC0105E0DU,
        IO13_PMBUSA_SDA      = 0xC0110F0DU,

        IO14_DIN             = 0xC002000EU,
        IO14_DOUT            = 0xC002100EU,
        IO14_EPWM8_A         = 0xC008E10EU,
        IO14_SCIB_TX         = 0xC005220EU,
        IO14_LINA_TX         = 0xC006050EU,
        IO14_PMBUSA_SCL      = 0xC0111F0EU,

        IO15_DIN             = 0xC002000FU,
        IO15_DOUT            = 0xC002100FU,
        IO15_EPWM8_B         = 0xC008F10FU,
        IO15_SCIB_RX         = 0xC005320FU,
        IO15_LINA_RX         = 0xC006150FU,

        IO16_DIN             = 0xC0020010U,
        IO16_DOUT            = 0xC0021010U,
        IO16_SPIA_SIMO       = 0xC0070110U,
        IO16_SD1_D1          = 0xC0090710U,

        IO17_DIN             = 0xC0020011U,
        IO17_DOUT            = 0xC0021011U,
        IO17_SPIA_SOMI       = 0xC0071111U,
        IO17_SD1_C1          = 0xC0091711U,

        IO18_DIN             = 0xC0020012U,
        IO18_DOUT            = 0xC0021012U,
        IO18_SPIA_CLK        = 0xC0072112U,
        IO18_SCIB_TX         = 0xC0052212U,
        IO18_CANA_RX         = 0xC0031312U,
        IO18_SD1_D2          = 0xC0092712U,

        IO19_DIN             = 0xC0020013U,
        IO19_DOUT            = 0xC0021013U,
        IO19_SPIA_STE        = 0xC0073113U,
        IO19_SCIB_RX         = 0xC0053213U,
        IO19_CANA_TX         = 0xC0030313U,
        IO19_SD1_C2          = 0xC0093713U,

        IO20_DIN             = 0xC0020014U,
        IO20_DOUT            = 0xC0021014U,
        IO20_EQEP1_A         = 0xC00D0114U,
        IO20_SD1_D3          = 0xC0094714U,

        IO21_DIN             = 0xC0020015U,
        IO21_DOUT            = 0xC0021015U,
        IO21_EQEP1_B         = 0xC00D1115U,
        IO21_SD1_C3          = 0xC0095715U,

        IO22_DIN             = 0xC0020016U,
        IO22_DOUT            = 0xC0021016U,
        IO22_EQEP1_STROBE    = 0xC00D3116U,
        IO22_SCIB_TX         = 0xC0052316U,
        IO22_SPIB_CLK        = 0xC0076616U,
        IO22_SD1_D4          = 0xC0096716U,

        IO23_DIN             = 0xC0020017U,
        IO23_DOUT            = 0xC0021017U,
        IO23_EQEP1_INDEX     = 0xC00D2117U,
        IO23_SCIB_RX         = 0xC0053317U,
        IO23_SPIB_STE        = 0xC0077617U,
        IO23_SD1_C4          = 0xC0097717U,

        IO24_DIN             = 0xC0020018U,
        IO24_DOUT            = 0xC0021018U,
        IO24_EQEP2_A         = 0xC00D4218U,
        IO24_SPIB_SIMO       = 0xC0074618U,
        IO24_PMBUSA_SCL      = 0xC0111918U,

        IO25_DIN             = 0xC0020019U,
        IO25_DOUT            = 0xC0021019U,
        IO25_EQEP2_B         = 0xC00D5219U,
        IO25_SPIB_SOMI       = 0xC0075619U,
        IO25_PMBUSA_SDA      = 0xC0110919U,
        IO25_FSITXA_D1       = 0xC0101E19U,

        IO26_DIN             = 0xC002001AU,
        IO26_DOUT            = 0xC002101AU,
        IO26_EQEP2_INDEX     = 0xC00D621AU,
        IO26_SPIB_CLK        = 0xC007661AU,
        IO26_PMBUSA_ALERT    = 0xC011391AU,
        IO26_FSITXA_D0       = 0xC0100E1AU,

        IO27_DIN             = 0xC002001BU,
        IO27_DOUT            = 0xC002101BU,
        IO27_EQEP2_STROBE    = 0xC00D721BU,
        IO27_SPIB_STE        = 0xC007761BU,
        IO27_PMBUSA_CTL      = 0xC011291BU,
        IO27_FSITXA_CLK      = 0xC0102E1BU,

        IO28_DIN             = 0xC002001CU,
        IO28_DOUT            = 0xC002101CU,
        IO28_SCIA_RX         = 0xC005111CU,

        IO29_DIN             = 0xC002001DU,
        IO29_DOUT            = 0xC002101DU,
        IO29_SCIA_TX         = 0xC005011DU,

        IO30_DIN             = 0xC002001EU,
        IO30_DOUT            = 0xC002101EU,
        IO30_CANA_RX         = 0xC003111EU,

        IO31_DIN             = 0xC002001FU,
        IO31_DOUT            = 0xC002101FU,
        IO31_CANA_TX         = 0xC003011FU,
        IO31_I2CA_SDA        = 0xC0040A1FU,

        IO32_DIN             = 0xC0020020U,
        IO32_DOUT            = 0xC0021020U,
        IO32_I2CA_SDA        = 0xC0040120U,
        IO32_SPIA_SIMO       = 0xC0070320U,
        IO32_I2CA_SCL        = 0xC0041A20U,

        IO33_DIN             = 0xC0020021U,
        IO33_DOUT            = 0xC0021021U,
        IO33_I2CA_SCL        = 0xC0041121U,
        IO33_SPIA_SOMI       = 0xC0071321U,

        IO34_DIN             = 0xC0020022U,
        IO34_DOUT            = 0xC0021022U,
        IO34_SPIA_CLK        = 0xC0072322U,
        IO34_SCIA_TX         = 0xC0050D22U,

        IO35_DIN             = 0xC0020023U,
        IO35_DOUT            = 0xC0021023U,
        IO35_SCIA_RX         = 0xC0051123U,
        IO35_SPIA_STE        = 0xC0073323U,

        IO36_DIN             = 0xC0020024U,
        IO36_DOUT            = 0xC0021024U,
        IO36_SCIA_TX         = 0xC0050124U,
        IO36_CANA_RX         = 0xC0031624U,
        IO36_SD1_D1          = 0xC0090D24U,

        IO37_DIN             = 0xC0020025U,
        IO37_DOUT            = 0xC0021025U,
        IO37_CANA_TX         = 0xC0030625U,
        IO37_SD1_D2          = 0xC0092D25U,

        IO38_DIN             = 0xC0020026U,
        IO38_DOUT            = 0xC0021026U,
        IO38_SCIB_TX         = 0xC0052626U,
        IO38_SD1_D3          = 0xC0094D26U,

        IO39_DIN             = 0xC0020027U,
        IO39_DOUT            = 0xC0021027U,
        IO39_SCIB_RX         = 0xC0053627U,
        IO39_SD1_D4          = 0xC0096D27U,

        IO40_DIN             = 0xC0020028U,
        IO40_DOUT            = 0xC0021028U,

        IO41_DIN             = 0xC0020029U,
        IO41_DOUT            = 0xC0021029U,

        IO42_DIN             = 0xC002002AU,
        IO42_DOUT            = 0xC002102AU,
        IO42_I2CA_SDA        = 0xC004062AU,
        IO42_SCIA_TX         = 0xC0050F2AU,

        IO43_DIN             = 0xC002002BU,
        IO43_DOUT            = 0xC002102BU,
        IO43_I2CA_SCL        = 0xC004162BU,
        IO43_SCIA_RX         = 0xC0051F2BU,

        IO44_DIN             = 0xC002002CU,
        IO44_DOUT            = 0xC002102CU,

        IO45_DIN             = 0xC002002DU,
        IO45_DOUT            = 0xC002102DU,

        IO46_DIN             = 0xC002002EU,
        IO46_DOUT            = 0xC002102EU,
        IO46_EPWM4_A         = 0xC008612EU,
        IO46_SCIA_RX         = 0xC005162EU,

        IO47_DIN             = 0xC002002FU,
        IO47_DOUT            = 0xC002102FU,
        IO47_EPWM4_B         = 0xC008712FU,
        IO47_SCIA_TX         = 0xC005062FU,

        IO48_DIN             = 0xC0020030U,
        IO48_DOUT            = 0xC0021030U,
        IO48_SCIA_TX         = 0xC0050630U,
        IO48_SD1_D1          = 0xC0090730U,

        IO49_DIN             = 0xC0020031U,
        IO49_DOUT            = 0xC0021031U,
        IO49_SCIA_RX         = 0xC0051631U,
        IO49_SD1_C1          = 0xC0091731U,
        IO49_FSITXA_D0       = 0xC0100E31U,

        IO50_DIN             = 0xC0020032U,
        IO50_DOUT            = 0xC0021032U,
        IO50_EQEP1_A         = 0xC00D0132U,
        IO50_SD1_D2          = 0xC0092732U,
        IO50_FSITXA_D1       = 0xC0101E32U,

        IO51_DIN             = 0xC0020033U,
        IO51_DOUT            = 0xC0021033U,
        IO51_EQEP1_B         = 0xC00D1133U,
        IO51_SD1_C2          = 0xC0093733U,
        IO51_FSITXA_CLK      = 0xC0102E33U,

        IO52_DIN             = 0xC0020034U,
        IO52_DOUT            = 0xC0021034U,
        IO52_EQEP1_STROBE    = 0xC00D3134U,
        IO52_SD1_D3          = 0xC0094734U,
        IO52_FSIRXA_D0       = 0xC0103E34U,

        IO53_DIN             = 0xC0020035U,
        IO53_DOUT            = 0xC0021035U,
        IO53_EQEP1_INDEX     = 0xC00D2135U,
        IO53_SD1_C3          = 0xC0095735U,
        IO53_SD1_C1          = 0xC0091D35U,
        IO53_FSIRXA_D1       = 0xC0104E35U,

        IO54_DIN             = 0xC0020036U,
        IO54_DOUT            = 0xC0021036U,
        IO54_SPIA_SIMO       = 0xC0070136U,
        IO54_EQEP2_A         = 0xC00D4536U,
        IO54_SCIB_TX         = 0xC0052636U,
        IO54_SD1_D4          = 0xC0096736U,
        IO54_SD1_C2          = 0xC0093D36U,
        IO54_FSIRXA_CLK      = 0xC0105E36U,

        IO55_DIN             = 0xC0020037U,
        IO55_DOUT            = 0xC0021037U,
        IO55_SPIA_SOMI       = 0xC0071137U,
        IO55_EQEP2_B         = 0xC00D5537U,
        IO55_SCIB_RX         = 0xC0053637U,
        IO55_SD1_C4          = 0xC0097737U,
        IO55_SD1_C3          = 0xC0095D37U,

        IO56_DIN             = 0xC0020038U,
        IO56_DOUT            = 0xC0021038U,
        IO56_SPIA_CLK        = 0xC0072138U,
        IO56_EQEP2_STROBE    = 0xC00D7538U,
        IO56_I2CA_SDA        = 0xC0040A38U,
        IO56_SD1_C4          = 0xC0097D38U,

        IO57_DIN             = 0xC0020039U,
        IO57_DOUT            = 0xC0021039U,
        IO57_SPIA_STE        = 0xC0073139U,
        IO57_EQEP2_INDEX     = 0xC00D6539U,
        IO57_I2CA_SCL        = 0xC0041A39U,

        IO58_DIN             = 0xC002003AU,
        IO58_DOUT            = 0xC002103AU,
        IO58_SPIA_SIMO       = 0xC007013AU,
        IO58_EPWM8_A         = 0xC008E33AU,
        IO58_SPIB_CLK        = 0xC007663AU,
        IO58_CANA_RX         = 0xC0031B3AU,

        IO59_DIN             = 0xC002003BU,
        IO59_DOUT            = 0xC002103BU,
        IO59_EPWM5_A         = 0xC008813BU,
        IO59_EPWM8_B         = 0xC008F33BU,
        IO59_SPIB_STE        = 0xC007763BU,
        IO59_CANA_TX         = 0xC0030B3BU,
        IO59_SPIA_SOMI       = 0xC0071F3BU,

        IO60_DIN             = 0xC002003CU,
        IO60_DOUT            = 0xC002103CU,
        IO60_EPWM3_B         = 0xC008513CU,
        IO60_SPIB_SIMO       = 0xC007463CU,
        IO60_SPIA_CLK        = 0xC0072F3CU,

        IO61_DIN             = 0xC002003DU,
        IO61_DOUT            = 0xC002103DU,
        IO61_SPIB_SOMI       = 0xC007563DU,
        IO61_CANA_RX         = 0xC0031E3DU,
        IO61_SPIA_STE        = 0xC0073F3DU,

        IO62_DIN             = 0xC002003EU,
        IO62_DOUT            = 0xC002103EU,
        IO62_SCIA_RX         = 0xC005113EU,
        IO62_CANA_RX         = 0xC003163EU,
        IO62_CANA_TX         = 0xC0030E3EU,

        IO63_DIN             = 0xC002003FU,
        IO63_DOUT            = 0xC002103FU,
        IO63_SCIA_TX         = 0xC005013FU,
        IO63_CANA_TX         = 0xC003063FU,
        IO63_SD1_D1          = 0xC0090D3FU,
        IO63_SPIB_SIMO       = 0xC0074F3FU,

        IO64_DIN             = 0xC0020040U,
        IO64_DOUT            = 0xC0021040U,
        IO64_SCIA_RX         = 0xC0051640U,
        IO64_SD1_C1          = 0xC0091D40U,
        IO64_SPIB_SOMI       = 0xC0075F40U,

        IO65_DIN             = 0xC0020041U,
        IO65_DOUT            = 0xC0021041U,
        IO65_SCIA_TX         = 0xC0050641U,
        IO65_SD1_D2          = 0xC0092D41U,
        IO65_SPIB_CLK        = 0xC0076F41U,

        IO66_DIN             = 0xC0020042U,
        IO66_DOUT            = 0xC0021042U,
        IO66_SD1_C2          = 0xC0093D42U,
        IO66_SPIB_STE        = 0xC0077F42U,

        IO67_DIN             = 0xC0020043U,
        IO67_DOUT            = 0xC0021043U,
        IO67_SD1_D3          = 0xC0094D43U,

        IO68_DIN             = 0xC0020044U,
        IO68_DOUT            = 0xC0021044U,
        IO68_SD1_C3          = 0xC0095D44U,

        IO69_DIN             = 0xC0020045U,
        IO69_DOUT            = 0xC0021045U,
        IO69_SD1_D4          = 0xC0096D45U,

        IO70_DIN             = 0xC0020046U,
        IO70_DOUT            = 0xC0021046U,
        IO70_CANA_RX         = 0xC0031546U,
        IO70_SCIB_TX         = 0xC0052646U,
        IO70_SD1_C4          = 0xC0097D46U,

        IO71_DIN             = 0xC0020047U,
        IO71_DOUT            = 0xC0021047U,
        IO71_CANA_TX         = 0xC0030547U,
        IO71_SCIB_RX         = 0xC0053647U,

        IO72_DIN             = 0xC0020048U,
        IO72_DOUT            = 0xC0021048U,

        IO73_DIN             = 0xC0020049U,
        IO73_DOUT            = 0xC0021049U,
        IO73_XCLKOUT         = 0xC0005349U,
        IO73_EPWM5_B         = 0xC0089749U,

        IO74_DIN             = 0xC002004AU,
        IO74_DOUT            = 0xC002104AU,
        IO74_EPWM8_A         = 0xC008E14AU,
        IO74_SD1_D4          = 0xC0096A4AU,

        IO75_DIN             = 0xC002004BU,
        IO75_DOUT            = 0xC002104BU,
        IO75_EPWM8_B         = 0xC008F14BU,

        IO76_DIN             = 0xC002004CU,
        IO76_DOUT            = 0xC002104CU,

        IO77_DIN             = 0xC002004DU,
        IO77_DOUT            = 0xC002104DU,
        IO77_SD1_D4          = 0xC0096A4DU,

        IO78_DIN             = 0xC002004EU,
        IO78_DOUT            = 0xC002104EU,
        IO78_EQEP2_A         = 0xC00D464EU,

        IO79_DIN             = 0xC002004FU,
        IO79_DOUT            = 0xC002104FU,
        IO79_ERRORSTS        = 0xC000654FU,
        IO79_EQEP2_B         = 0xC00D564FU,

        IO80_DIN             = 0xC0020050U,
        IO80_DOUT            = 0xC0021050U,
        IO80_ERRORSTS        = 0xC0006550U,
        IO80_EQEP2_STROBE    = 0xC00D7650U,
        IO80_SD1_D4          = 0xC0096A50U,

        IO81_DIN             = 0xC0020051U,
        IO81_DOUT            = 0xC0021051U,
        IO81_EQEP2_INDEX     = 0xC00D6651U,

        IO82_DIN             = 0xC0020052U,
        IO82_DOUT            = 0xC0021052U,

        IO83_DIN             = 0xC0020053U,
        IO83_DOUT            = 0xC0021053U,

        IO84_DIN             = 0xC0020054U,
        IO84_DOUT            = 0xC0021054U,
        IO84_SCIA_TX         = 0xC0050554U,

        IO85_DIN             = 0xC0020055U,
        IO85_DOUT            = 0xC0021055U,
        IO85_SCIA_RX         = 0xC0051555U,

        IO86_DIN             = 0xC0020056U,
        IO86_DOUT            = 0xC0021056U,
        IO86_SCIB_TX         = 0xC0052556U,

        IO87_DIN             = 0xC0020057U,
        IO87_DOUT            = 0xC0021057U,
        IO87_SCIB_RX         = 0xC0053557U,

        IO88_DIN             = 0xC0020058U,
        IO88_DOUT            = 0xC0021058U,

        IO89_DIN             = 0xC0020059U,
        IO89_DOUT            = 0xC0021059U,
        IO89_SD1_D3          = 0xC0094759U,

        IO90_DIN             = 0xC002005AU,
        IO90_DOUT            = 0xC002105AU,
        IO90_SD1_C3          = 0xC009575AU,

        IO91_DIN             = 0xC002005BU,
        IO91_DOUT            = 0xC002105BU,
        IO91_I2CA_SDA        = 0xC004065BU,
        IO91_PMBUSA_SCL      = 0xC0111A5BU,

        IO92_DIN             = 0xC002005CU,
        IO92_DOUT            = 0xC002105CU,
        IO92_I2CA_SCL        = 0xC004165CU,
        IO92_PMBUSA_SDA      = 0xC0110A5CU,

        IO93_DIN             = 0xC002005DU,
        IO93_DOUT            = 0xC002105DU,
        IO93_PMBUSA_ALERT    = 0xC0113A5DU,

        IO94_DIN             = 0xC002005EU,
        IO94_DOUT            = 0xC002105EU,
        IO94_PMBUSA_CTL      = 0xC0112A5EU,

        IO95_DIN             = 0xC002005FU,
        IO95_DOUT            = 0xC002105FU,
        IO95_SD1_D1          = 0xC009065FU,

        IO96_DIN             = 0xC0020060U,
        IO96_DOUT            = 0xC0021060U,
        IO96_EQEP1_A         = 0xC00D0560U,
        IO96_SD1_C1          = 0xC0091660U,

        IO97_DIN             = 0xC0020061U,
        IO97_DOUT            = 0xC0021061U,
        IO97_EQEP1_B         = 0xC00D1561U,
        IO97_SD1_D2          = 0xC0092661U,

        IO98_DIN             = 0xC0020062U,
        IO98_DOUT            = 0xC0021062U,
        IO98_EQEP1_STROBE    = 0xC00D3562U,
        IO98_SD1_C2          = 0xC0093662U,

        IO99_DIN             = 0xC0020063U,
        IO99_DOUT            = 0xC0021063U,
        IO99_EPWM8_A         = 0xC008E363U,
        IO99_EQEP1_INDEX     = 0xC00D2563U,

        IO100_DIN            = 0xC0020064U,
        IO100_DOUT           = 0xC0021064U,
        IO100_SPIA_SIMO      = 0xC0070164U,
        IO100_EQEP2_A        = 0xC00D4564U,
        IO100_SD1_D1         = 0xC0090964U,
        IO100_FSITXA_D0      = 0xC0100D64U,

        IO101_DIN            = 0xC0020065U,
        IO101_DOUT           = 0xC0021065U,
        IO101_EQEP2_B        = 0xC00D5565U,
        IO101_FSITXA_D1      = 0xC0101D65U,

        IO102_DIN            = 0xC0020066U,
        IO102_DOUT           = 0xC0021066U,
        IO102_EQEP2_STROBE   = 0xC00D7566U,
        IO102_FSITXA_CLK     = 0xC0102D66U,

        IO103_DIN            = 0xC0020067U,
        IO103_DOUT           = 0xC0021067U,
        IO103_EPWM8_B        = 0xC008F367U,
        IO103_EQEP2_INDEX    = 0xC00D6567U,
        IO103_FSIRXA_D0      = 0xC0103D67U,

        IO104_DIN            = 0xC0020068U,
        IO104_DOUT           = 0xC0021068U,
        IO104_I2CA_SDA       = 0xC0040168U,
        IO104_FSIRXA_D1      = 0xC0104D68U,

        IO105_DIN            = 0xC0020069U,
        IO105_DOUT           = 0xC0021069U,
        IO105_I2CA_SCL       = 0xC0041169U,
        IO105_FSIRXA_CLK     = 0xC0105D69U,

        IO106_DIN            = 0xC002006AU,
        IO106_DOUT           = 0xC002106AU,

        IO107_DIN            = 0xC002006BU,
        IO107_DOUT           = 0xC002106BU,

        IO108_DIN            = 0xC002006CU,
        IO108_DOUT           = 0xC002106CU,

        IO109_DIN            = 0xC002006DU,
        IO109_DOUT           = 0xC002106DU,

        IO110_DIN            = 0xC002006EU,
        IO110_DOUT           = 0xC002106EU,

        IO111_DIN            = 0xC002006FU,
        IO111_DOUT           = 0xC002106FU,

        IO112_DIN            = 0xC0020070U,
        IO112_DOUT           = 0xC0021070U,
        IO112_SD1_D3         = 0xC0094770U,

        IO113_DIN            = 0xC0020071U,
        IO113_DOUT           = 0xC0021071U,
        IO113_SD1_C3         = 0xC0095771U,

        IO114_DIN            = 0xC0020072U,
        IO114_DOUT           = 0xC0021072U,
        IO114_SD1_D4         = 0xC0096772U,

        IO115_DIN            = 0xC0020073U,
        IO115_DOUT           = 0xC0021073U,
        IO115_SD1_C4         = 0xC0097773U,

        IO116_DIN            = 0xC0020074U,
        IO116_DOUT           = 0xC0021074U,

        IO119_DIN            = 0xC0020077U,
        IO119_DOUT           = 0xC0021077U,

        IO120_DIN            = 0xC0020078U,
        IO120_DOUT           = 0xC0021078U,

        IO122_DIN            = 0xC002007AU,
        IO122_DOUT           = 0xC002107AU,
        IO122_SD1_D1         = 0xC009077AU,

        IO123_DIN            = 0xC002007BU,
        IO123_DOUT           = 0xC002107BU,
        IO123_SD1_C1         = 0xC009177BU,

        IO124_DIN            = 0xC002007CU,
        IO124_DOUT           = 0xC002107CU,
        IO124_SD1_D2         = 0xC009277CU,

        IO125_DIN            = 0xC002007DU,
        IO125_DOUT           = 0xC002107DU,
        IO125_SD1_C2         = 0xC009377DU,

        IO126_DIN            = 0xC002007EU,
        IO126_DOUT           = 0xC002107EU,
        IO126_SD1_D3         = 0xC009477EU,

        IO127_DIN            = 0xC002007FU,
        IO127_DOUT           = 0xC002107FU,
        IO127_SD1_C3         = 0xC009577FU,

        IO128_DIN            = 0xC0020080U,
        IO128_DOUT           = 0xC0021080U,
        IO128_SD1_D4         = 0xC0096780U,

        IO129_DIN            = 0xC0020081U,
        IO129_DOUT           = 0xC0021081U,
        IO129_SD1_C4         = 0xC0097781U,

        IO130_DIN            = 0xC0020082U,
        IO130_DOUT           = 0xC0021082U,

        IO131_DIN            = 0xC0020083U,
        IO131_DOUT           = 0xC0021083U,

        IO132_DIN            = 0xC0020084U,
        IO132_DOUT           = 0xC0021084U,

        IO133_DIN            = 0xC0020085U,
        IO133_DOUT           = 0xC0021085U,

        IO134_DIN            = 0xC0020086U,
        IO134_DOUT           = 0xC0021086U,

        IO141_DIN            = 0xC002008DU,
        IO141_DOUT           = 0xC002108DU,
        IO141_SCIB_TX        = 0xC005268DU,

        IO142_DIN            = 0xC002008EU,
        IO142_DOUT           = 0xC002108EU,
        IO142_SCIB_RX        = 0xC005368EU,

        IO145_DIN            = 0xC0020091U,
        IO145_DOUT           = 0xC0021091U,
        IO145_EPWM1_A        = 0xC0080191U,

        IO146_DIN            = 0xC0020092U,
        IO146_DOUT           = 0xC0021092U,
        IO146_EPWM1_B        = 0xC0081192U,

        IO147_DIN            = 0xC0020093U,
        IO147_DOUT           = 0xC0021093U,
        IO147_EPWM2_A        = 0xC0082193U,

        IO148_DIN            = 0xC0020094U,
        IO148_DOUT           = 0xC0021094U,
        IO148_EPWM2_B        = 0xC0083194U,

        IO149_DIN            = 0xC0020095U,
        IO149_DOUT           = 0xC0021095U,
        IO149_EPWM3_A        = 0xC0084195U,

        IO150_DIN            = 0xC0020096U,
        IO150_DOUT           = 0xC0021096U,
        IO150_EPWM3_B        = 0xC0085196U,

        IO151_DIN            = 0xC0020097U,
        IO151_DOUT           = 0xC0021097U,
        IO151_EPWM4_A        = 0xC0086197U,
        IO151_PMBUSA_SCL     = 0xC0111697U,
        IO151_FSITXA_D0      = 0xC0100D97U,

        IO152_DIN            = 0xC0020098U,
        IO152_DOUT           = 0xC0021098U,
        IO152_EPWM4_B        = 0xC0087198U,
        IO152_PMBUSA_SDA     = 0xC0110698U,
        IO152_FSITXA_D1      = 0xC0101D98U,

        IO153_DIN            = 0xC0020099U,
        IO153_DOUT           = 0xC0021099U,
        IO153_EPWM5_A        = 0xC0088199U,
        IO153_PMBUSA_ALERT   = 0xC0113699U,
        IO153_FSITXA_CLK     = 0xC0102D99U,

        IO154_DIN            = 0xC002009AU,
        IO154_DOUT           = 0xC002109AU,
        IO154_EPWM5_B        = 0xC008919AU,
        IO154_PMBUSA_CTL     = 0xC011269AU,
        IO154_FSIRXA_D0      = 0xC0103D9AU,

        IO155_DIN            = 0xC002009BU,
        IO155_DOUT           = 0xC002109BU,
        IO155_EPWM6_A        = 0xC008A19BU,
        IO155_FSIRXA_D1      = 0xC0104D9BU,

        IO156_DIN            = 0xC002009CU,
        IO156_DOUT           = 0xC002109CU,
        IO156_EPWM6_B        = 0xC008B19CU,
        IO156_FSIRXA_CLK     = 0xC0105D9CU,

        IO157_DIN            = 0xC002009DU,
        IO157_DOUT           = 0xC002109DU,
        IO157_EPWM7_A        = 0xC008C19DU,

        IO158_DIN            = 0xC002009EU,
        IO158_DOUT           = 0xC002109EU,
        IO158_EPWM7_B        = 0xC008D19EU,

        IO159_DIN            = 0xC002009FU,
        IO159_DOUT           = 0xC002109FU,
        IO159_EPWM8_A        = 0xC008E19FU,

        IO160_DIN            = 0xC00200A0U,
        IO160_DOUT           = 0xC00210A0U,
        IO160_EPWM8_B        = 0xC008F1A0U,

        IO161_DIN            = 0xC00200A1U,
        IO161_DOUT           = 0xC00210A1U,

        IO162_DIN            = 0xC00200A2U,
        IO162_DOUT           = 0xC00210A2U,

        IO163_DIN            = 0xC00200A3U,
        IO163_DOUT           = 0xC00210A3U,

        IO164_DIN            = 0xC00200A4U,
        IO164_DOUT           = 0xC00210A4U,

        IO165_DIN            = 0xC00200A5U,
        IO165_DOUT           = 0xC00210A5U,

        IO166_DIN            = 0xC00200A6U,
        IO166_DOUT           = 0xC00210A6U,

        IO167_DIN            = 0xC00200A7U,
        IO167_DOUT           = 0xC00210A7U,

        IO168_DIN            = 0xC00200A8U,
        IO168_DOUT           = 0xC00210A8U,

        IO198_DIN            = 0xC00200C6U,
        IO198_DOUT           = 0xC00210C6U,
        IO198_EQEP1_A        = 0xC00D01C6U,
        IO198_SPIA_SIMO      = 0xC00703C6U,

        IO199_DIN            = 0xC00200C7U,
        IO199_DOUT           = 0xC00210C7U,
        IO199_EQEP1_STROBE   = 0xC00D31C7U,
        IO199_SCIB_TX        = 0xC00523C7U,
        IO199_SPIB_CLK       = 0xC00766C7U,
        IO199_SD1_D4         = 0xC00967C7U,

        IO200_DIN            = 0xC00200C8U,
        IO200_DOUT           = 0xC00210C8U,
        IO200_EQEP1_INDEX    = 0xC00D21C8U,
        IO200_SCIB_RX        = 0xC00533C8U,
        IO200_SPIB_STE       = 0xC00776C8U,
        IO200_SD1_C4         = 0xC00977C8U,

        IO201_DIN            = 0xC00200C9U,
        IO201_DOUT           = 0xC00210C9U,
        IO201_EQEP2_A        = 0xC00D42C9U,
        IO201_SPIB_SIMO      = 0xC00746C9U,
        IO201_PMBUSA_SCL     = 0xC01119C9U,

        IO202_DIN            = 0xC00200CAU,
        IO202_DOUT           = 0xC00210CAU,
        IO202_EQEP2_B        = 0xC00D52CAU,
        IO202_SPIB_SOMI      = 0xC00756CAU,
        IO202_PMBUSA_SDA     = 0xC01109CAU,
        IO202_FSITXA_D1      = 0xC0101ECAU,

        IO203_DIN            = 0xC00200CBU,
        IO203_DOUT           = 0xC00210CBU,
        IO203_EQEP2_INDEX    = 0xC00D62CBU,
        IO203_SPIA_SOMI      = 0xC00713CBU,
        IO203_SPIB_CLK       = 0xC00766CBU,
        IO203_PMBUSA_ALERT   = 0xC01139CBU,
        IO203_FSITXA_D0      = 0xC0100ECBU,
        IO203_EPWM8_B        = 0xC008FFCBU,

        IO204_DIN            = 0xC00200CCU,
        IO204_DOUT           = 0xC00210CCU,
        IO204_EQEP2_STROBE   = 0xC00D72CCU,
        IO204_SPIA_CLK       = 0xC00723CCU,
        IO204_SPIB_STE       = 0xC00776CCU,
        IO204_PMBUSA_CTL     = 0xC01129CCU,
        IO204_FSITXA_CLK     = 0xC0102ECCU,
        IO204_SD1_D3         = 0xC0094FCCU,

        IO205_DIN            = 0xC00200CDU,
        IO205_DOUT           = 0xC00210CDU,
        IO205_EQEP1_INDEX    = 0xC00D21CDU,
        IO205_SPIA_STE       = 0xC00733CDU,
        IO205_SD1_C3         = 0xC0095FCDU,

        IO206_DIN            = 0xC00200CEU,
        IO206_DOUT           = 0xC00210CEU,

        IO207_DIN            = 0xC00200CFU,
        IO207_DOUT           = 0xC00210CFU,
        IO207_EQEP2_A        = 0xC00D41CFU,
        IO207_CANA_TX        = 0xC00305CFU,
        IO207_SCIA_RX        = 0xC00517CFU,
        IO207_LINA_RX        = 0xC00619CFU,
        IO207_PMBUSA_ALERT   = 0xC0113FCFU,

        IO208_DIN            = 0xC00200D0U,
        IO208_DOUT           = 0xC00210D0U,
        IO208_EQEP2_B        = 0xC00D51D0U,
        IO208_SPIB_SIMO      = 0xC00745D0U,
        IO208_SCIA_TX        = 0xC00507D0U,
        IO208_PMBUSA_CTL     = 0xC0112FD0U,

        IO209_DIN            = 0xC00200D1U,
        IO209_DOUT           = 0xC00210D1U,
        IO209_EQEP2_STROBE   = 0xC00D71D1U,
        IO209_SPIB_SOMI      = 0xC00755D1U,
        IO209_PMBUSA_SDA     = 0xC0110FD1U,

        IO210_DIN            = 0xC00200D2U,
        IO210_DOUT           = 0xC00210D2U,
        IO210_EQEP2_INDEX    = 0xC00D61D2U,
        IO210_PMBUSA_SCL     = 0xC0111FD2U,

        IO211_DIN            = 0xC00200D3U,
        IO211_DOUT           = 0xC00210D3U,

        IO212_DIN            = 0xC00200D4U,
        IO212_DOUT           = 0xC00210D4U,

        IO213_DIN            = 0xC00200D5U,
        IO213_DOUT           = 0xC00210D5U,
        IO213_EPWM8_A        = 0xC008E2D5U,

        IO214_DIN            = 0xC00200D6U,
        IO214_DOUT           = 0xC00210D6U,
        IO214_CANA_RX        = 0xC00311D6U,

        IO215_DIN            = 0xC00200D7U,
        IO215_DOUT           = 0xC00210D7U,
        IO215_SCIA_RX        = 0xC00511D7U,
        IO215_CANA_RX        = 0xC00313D7U,
        IO215_LINA_TX        = 0xC0060ED7U,

        IO216_DIN            = 0xC00200D8U,
        IO216_DOUT           = 0xC00210D8U,
        IO216_SCIA_TX        = 0xC00501D8U,

        IO217_DIN            = 0xC00200D9U,
        IO217_DOUT           = 0xC00210D9U,
        IO217_CANA_TX        = 0xC00301D9U,
        IO217_I2CA_SDA       = 0xC0040AD9U,

        IO218_DIN            = 0xC00200DAU,
        IO218_DOUT           = 0xC00210DAU,
        IO218_I2CA_SDA       = 0xC00401DAU,
        IO218_SPIA_SIMO      = 0xC00703DAU,
        IO218_I2CA_SCL       = 0xC0041ADAU,

        IO219_DIN            = 0xC00200DBU,
        IO219_DOUT           = 0xC00210DBU,
        IO219_EPWM8_B        = 0xC008F2DBU,

        IO220_DIN            = 0xC00200DCU,
        IO220_DOUT           = 0xC00210DCU,
        IO220_EPWM6_A        = 0xC008A2DCU,
        IO220_SCIB_TX        = 0xC00526DCU,
        IO220_PMBUSA_ALERT   = 0xC0113FDCU,

        IO221_DIN            = 0xC00200DDU,
        IO221_DOUT           = 0xC00210DDU,
        IO221_EPWM6_B        = 0xC008B2DDU,
        IO221_SCIB_RX        = 0xC00536DDU,
        IO221_PMBUSA_CTL     = 0xC0112FDDU,
        IO221_X2             = 0xC00090DDU,

        IO222_DIN            = 0xC00200DEU,
        IO222_DOUT           = 0xC00210DEU,
        IO222_JTAG_TDI       = 0xC01201DEU,
        IO222_EPWM7_A        = 0xC008C2DEU,
        IO222_SPIA_SIMO      = 0xC00703DEU,
        IO222_SCIA_RX        = 0xC00516DEU,
        IO222_I2CA_SDA       = 0xC00409DEU,
        IO222_PMBUSA_SDA     = 0xC0110FDEU,

        IO223_DIN            = 0xC00200DFU,
        IO223_DOUT           = 0xC00210DFU,
        IO223_JTAG_TDO       = 0xC01211DFU,
        IO223_EPWM7_B        = 0xC008D2DFU,
        IO223_SCIA_TX        = 0xC00506DFU,
        IO223_I2CA_SCL       = 0xC00419DFU,
        IO223_PMBUSA_SCL     = 0xC0111FDFU,

        IO224_DIN            = 0xC00200E0U,
        IO224_DOUT           = 0xC00210E0U,
        IO224_ERRORSTS       = 0xC00061E0U,
        IO224_XCLKOUT        = 0xC00053E0U,

        IO225_AIN            = 0xC00100E1U,
        IO225_DIN            = 0xC00200E1U,

        IO226_AIN            = 0xC00100E2U,
        IO226_DIN            = 0xC00200E2U,

        IO227_AIN            = 0xC00100E3U,
        IO227_DIN            = 0xC00200E3U,

        IO228_AIN            = 0xC00100E4U,
        IO228_DIN            = 0xC00200E4U,

        IO229_AIN            = 0xC00100E5U,
        IO229_DIN            = 0xC00200E5U,

        IO230_AIN            = 0xC00100E6U,
        IO230_DIN            = 0xC00200E6U,

        IO231_AIN            = 0xC00100E7U,
        IO231_DIN            = 0xC00200E7U,

        IO232_AIN            = 0xC00100E8U,
        IO232_DIN            = 0xC00200E8U,

        IO233_AIN            = 0xC00100E9U,
        IO233_DIN            = 0xC00200E9U,

        IO234_AIN            = 0xC00100EAU,
        IO234_DIN            = 0xC00200EAU,

        IO235_AIN            = 0xC00100EBU,
        IO235_DIN            = 0xC00200EBU,

        IO236_AIN            = 0xC00100ECU,
        IO236_DIN            = 0xC00200ECU,

        IO237_AIN            = 0xC00100EDU,
        IO237_DIN            = 0xC00200EDU,

        IO238_AIN            = 0xC00100EEU,
        IO238_DIN            = 0xC00200EEU,

        IO239_AIN            = 0xC00100EFU,
        IO239_DIN            = 0xC00200EFU,

        IO240_AIN            = 0xC00100F0U,
        IO240_DIN            = 0xC00200F0U,

        IO241_AIN            = 0xC00100F1U,
        IO241_DIN            = 0xC00200F1U
};

#elif defined(F2803X)
enum io {
	IOX                  = 0x00000000U,

	IO0_DIN              = 0xC0020000U,
	IO0_DOUT             = 0xC0021000U,
	IO0_EPWM1_A          = 0xC0080100U,

	IO1_DIN              = 0xC0020001U,
	IO1_DOUT             = 0xC0021001U,
	IO1_EPWM1_B          = 0xC0081101U,
	IO1_COMP1_OUT        = 0xC00C0301U,

	IO2_DIN              = 0xC0020002U,
	IO2_DOUT             = 0xC0021002U,
	IO2_EPWM2_A          = 0xC0082102U,

	IO3_DIN              = 0xC0020003U,
	IO3_DOUT             = 0xC0021003U,
	IO3_EPWM2_B          = 0xC0083103U,
	IO3_SPIA_SOMI        = 0xC0071203U,
	IO3_COMP2_OUT        = 0xC00C1303U,

	IO4_DIN              = 0xC0020004U,
	IO4_DOUT             = 0xC0021004U,
	IO4_EPWM3_A          = 0xC0084104U,

	IO5_DIN              = 0xC0020005U,
	IO5_DOUT             = 0xC0021005U,
	IO5_EPWM3_B          = 0xC0085105U,
	IO5_SPIA_SIMO        = 0xC0070205U,
	IO5_ECAP1            = 0xC00E0305U,

	IO6_DIN              = 0xC0020006U,
	IO6_DOUT             = 0xC0021006U,
	IO6_EPWM4_A          = 0xC0086106U,
	IO6_SYNCIN           = 0xC0002206U,
	IO6_SYNCOUT          = 0xC0003306U,

	IO7_DIN              = 0xC0020007U,
	IO7_DOUT             = 0xC0021007U,
	IO7_EPWM4_B          = 0xC0087107U,
	IO7_SCIA_RX          = 0xC0051207U,

	IO8_DIN              = 0xC0020008U,
	IO8_DOUT             = 0xC0021008U,
	IO8_EPWM5_A          = 0xC0088108U,
	IO8_ADCSOCAO         = 0xC0007308U,

	IO9_DIN              = 0xC0020009U,
	IO9_DOUT             = 0xC0021009U,
	IO9_EPWM5_B          = 0xC0089109U,
	IO9_LINA_TX          = 0xC0060209U,
	IO9_HRCAP1           = 0xC00F0309U,

	IO10_DIN             = 0xC002000AU,
	IO10_DOUT            = 0xC002100AU,
	IO10_EPWM6_A         = 0xC008A10AU,
	IO10_ADCSOCBO        = 0xC000830AU,

	IO11_DIN             = 0xC002000BU,
	IO11_DOUT            = 0xC002100BU,
	IO11_EPWM6_B         = 0xC008B10BU,
	IO11_LINA_RX         = 0xC006120BU,
	IO11_HRCAP2          = 0xC00F130BU,

	IO12_DIN             = 0xC002000CU,
	IO12_DOUT            = 0xC002100CU,
	IO12_TZ1             = 0xC00B010CU,
	IO12_SCIA_TX         = 0xC005020CU,
	IO12_SPIB_SIMO       = 0xC007430CU,

	IO13_DIN             = 0xC002000DU,
	IO13_DOUT            = 0xC002100DU,
	IO13_TZ2             = 0xC00B110DU,
	IO13_SPIB_SOMI       = 0xC007530DU,

	IO14_DIN             = 0xC002000EU,
	IO14_DOUT            = 0xC002100EU,
	IO14_TZ3             = 0xC00B210EU,
	IO14_LINA_TX         = 0xC006020EU,
	IO14_SPIB_CLK        = 0xC007630EU,

	IO15_DIN             = 0xC002000FU,
	IO15_DOUT            = 0xC002100FU,
	IO15_TZ1             = 0xC00B010FU,
	IO15_LINA_RX         = 0xC006120FU,
	IO15_SPIB_STE        = 0xC007730FU,

	IO16_DIN             = 0xC0020010U,
	IO16_DOUT            = 0xC0021010U,
	IO16_SPIA_SIMO       = 0xC0070110U,
	IO16_TZ2             = 0xC00B1310U,

	IO17_DIN             = 0xC0020011U,
	IO17_DOUT            = 0xC0021011U,
	IO17_SPIA_SOMI       = 0xC0071111U,
	IO17_TZ3             = 0xC00B2311U,

	IO18_DIN             = 0xC0020012U,
	IO18_DOUT            = 0xC0021012U,
	IO18_SPIA_CLK        = 0xC0072112U,
	IO18_LINA_TX         = 0xC0060212U,
	IO18_XCLKOUT         = 0xC0005312U,

	IO19_DIN             = 0xC0020013U,
	IO19_DOUT            = 0xC0021013U,
	IO19_XCLKIN          = 0xC0004013U,
	IO19_SPIA_STE        = 0xC0073113U,
	IO19_LINA_RX         = 0xC0061213U,
	IO19_ECAP1           = 0xC00E0313U,

	IO20_DIN             = 0xC0020014U,
	IO20_DOUT            = 0xC0021014U,
	IO20_EQEP1_A         = 0xC00D0114U,
	IO20_COMP1_OUT       = 0xC00C0314U,

	IO21_DIN             = 0xC0020015U,
	IO21_DOUT            = 0xC0021015U,
	IO21_EQEP1_B         = 0xC00D1115U,
	IO21_COMP2_OUT       = 0xC00C1315U,

	IO22_DIN             = 0xC0020016U,
	IO22_DOUT            = 0xC0021016U,
	IO22_EQEP1_STROBE    = 0xC00D3116U,
	IO22_LINA_TX         = 0xC0060316U,

	IO23_DIN             = 0xC0020017U,
	IO23_DOUT            = 0xC0021017U,
	IO23_EQEP1_INDEX     = 0xC00D2117U,
	IO23_LINA_RX         = 0xC0061317U,

	IO24_DIN             = 0xC0020018U,
	IO24_DOUT            = 0xC0021018U,
	IO24_ECAP1           = 0xC00E0118U,
	IO24_SPIB_SIMO       = 0xC0074318U,

	IO25_DIN             = 0xC0020019U,
	IO25_DOUT            = 0xC0021019U,
	IO25_SPIB_SOMI       = 0xC0075319U,

	IO26_DIN             = 0xC002001AU,
	IO26_DOUT            = 0xC002101AU,
	IO26_HRCAP1          = 0xC00F011AU,
	IO26_SPIB_CLK        = 0xC007631AU,

	IO27_DIN             = 0xC002001BU,
	IO27_DOUT            = 0xC002101BU,
	IO27_HRCAP2          = 0xC00F111BU,
	IO27_SPIB_STE        = 0xC007731BU,

	IO28_DIN             = 0xC002001CU,
	IO28_DOUT            = 0xC002101CU,
	IO28_SCIA_RX         = 0xC005111CU,
	IO28_I2CA_SDA        = 0xC004021CU,
	IO28_TZ2             = 0xC00B131CU,

	IO29_DIN             = 0xC002001DU,
	IO29_DOUT            = 0xC002101DU,
	IO29_SCIA_TX         = 0xC005011DU,
	IO29_I2CA_SCL        = 0xC004121DU,
	IO29_TZ3             = 0xC00B231DU,

	IO30_DIN             = 0xC002001EU,
	IO30_DOUT            = 0xC002101EU,
	IO30_CANA_RX         = 0xC003111EU,

	IO31_DIN             = 0xC002001FU,
	IO31_DOUT            = 0xC002101FU,
	IO31_CANA_TX         = 0xC003011FU,

	IO32_DIN             = 0xC0020020U,
	IO32_DOUT            = 0xC0021020U,
	IO32_I2CA_SDA        = 0xC0040120U,
	IO32_SYNCIN          = 0xC0002220U,
	IO32_ADCSOCAO        = 0xC0007320U,

	IO33_DIN             = 0xC0020021U,
	IO33_DOUT            = 0xC0021021U,
	IO33_I2CA_SCL        = 0xC0041121U,
	IO33_SYNCOUT         = 0xC0003221U,
	IO33_ADCSOCBO        = 0xC0008321U,

	IO34_DIN             = 0xC0020022U,
	IO34_DOUT            = 0xC0021022U,
	IO34_COMP2_OUT       = 0xC00C1122U,
	IO34_COMP3_OUT       = 0xC00C2322U,

	IO35_DIN             = 0xC0020023U,
	IO35_DOUT            = 0xC0021023U,
	IO35_JTAG_TDI        = 0xC0120023U,

	IO36_DIN             = 0xC0020024U,
	IO36_DOUT            = 0xC0021024U,
	IO36_JTAG_TMS        = 0xC0122024U,

	IO37_DIN             = 0xC0020025U,
	IO37_DOUT            = 0xC0021025U,
	IO37_JTAG_TDO        = 0xC0121025U,

	IO38_DIN             = 0xC0020026U,
	IO38_DOUT            = 0xC0021026U,
	IO38_JTAG_TCK        = 0xC0123026U,
	IO38_XCLKIN          = 0xC0004026U,

	IO39_DIN             = 0xC0020027U,
	IO39_DOUT            = 0xC0021027U,

	IO40_DIN             = 0xC0020028U,
	IO40_DOUT            = 0xC0021028U,
	IO40_EPWM7_A         = 0xC008C128U,

	IO41_DIN             = 0xC0020029U,
	IO41_DOUT            = 0xC0021029U,
	IO41_EPWM7_B         = 0xC008D129U,

	IO42_DIN             = 0xC002002AU,
	IO42_DOUT            = 0xC002102AU,
	IO42_COMP1_OUT       = 0xC00C032AU,

	IO43_DIN             = 0xC002002BU,
	IO43_DOUT            = 0xC002102BU,
	IO43_COMP2_OUT       = 0xC00C132BU,

	IO44_DIN             = 0xC002002CU,
	IO44_DOUT            = 0xC002102CU,

	IO98_AIN             = 0xC0010062U,
	IO98_AOUT            = 0xC0011062U,
	IO98_DIN             = 0xC0020062U,
	IO98_DOUT            = 0xC0021062U,

	IO100_AIN            = 0xC0010064U,
	IO100_AOUT           = 0xC0011064U,
	IO100_DIN            = 0xC0020064U,
	IO100_DOUT           = 0xC0021064U,

	IO102_AIN            = 0xC0010066U,
	IO102_AOUT           = 0xC0011066U,
	IO102_DIN            = 0xC0020066U,
	IO102_DOUT           = 0xC0021066U,

	IO106_AIN            = 0xC001006AU,
	IO106_AOUT           = 0xC001106AU,
	IO106_DIN            = 0xC002006AU,
	IO106_DOUT           = 0xC002106AU,

	IO108_AIN            = 0xC001006CU,
	IO108_AOUT           = 0xC001106CU,
	IO108_DIN            = 0xC002006CU,
	IO108_DOUT           = 0xC002106CU,

	IO110_AIN            = 0xC001006EU,
	IO110_AOUT           = 0xC001106EU,
	IO110_DIN            = 0xC002006EU,
	IO110_DOUT           = 0xC002106EU
	
};
#elif defined(F28004X)
enum io {
	IOX                  = 0x00000000U,

	IO0_DIN              = 0xC0020000U,
	IO0_DOUT             = 0xC0021000U,
	IO0_EPWM1_A          = 0xC0080100U,
	IO0_I2CA_SDA         = 0xC0040600U,

	IO1_DIN              = 0xC0020001U,
	IO1_DOUT             = 0xC0021001U,
	IO1_EPWM1_B          = 0xC0081101U,
	IO1_I2CA_SCL         = 0xC0041601U,

	IO2_DIN              = 0xC0020002U,
	IO2_DOUT             = 0xC0021002U,
	IO2_EPWM2_A          = 0xC0082102U,
	IO2_OUTXBAR1         = 0xC00A0502U,
	IO2_PMBUSA_SDA       = 0xC0110602U,
	IO2_SCIA_TX          = 0xC0050902U,
	IO2_FSIRXA_D1        = 0xC0104A02U,

	IO3_DIN              = 0xC0020003U,
	IO3_DOUT             = 0xC0021003U,
	IO3_EPWM2_B          = 0xC0083103U,
	IO3_OUTXBAR2         = 0xC00A1203U,
	IO3_PMBUSA_SCL       = 0xC0111603U,
	IO3_SPIA_CLK         = 0xC0072703U,
	IO3_SCIA_RX          = 0xC0051903U,
	IO3_FSIRXA_D0        = 0xC0103A03U,

	IO4_DIN              = 0xC0020004U,
	IO4_DOUT             = 0xC0021004U,
	IO4_EPWM3_A          = 0xC0084104U,
	IO4_OUTXBAR3         = 0xC00A2504U,
	IO4_CANA_TX          = 0xC0030604U,
	IO4_FSIRXA_CLK       = 0xC0105A04U,

	IO5_DIN              = 0xC0020005U,
	IO5_DOUT             = 0xC0021005U,
	IO5_EPWM3_B          = 0xC0085105U,
	IO5_OUTXBAR3         = 0xC00A2305U,
	IO5_CANA_RX          = 0xC0031605U,
	IO5_SPIA_STE         = 0xC0073705U,
	IO5_FSITXA_D1        = 0xC0101905U,

	IO6_DIN              = 0xC0020006U,
	IO6_DOUT             = 0xC0021006U,
	IO6_EPWM4_A          = 0xC0086106U,
	IO6_OUTXBAR4         = 0xC00A3206U,
	IO6_SYNCOUT          = 0xC0003306U,
	IO6_EQEP1_A          = 0xC00D0506U,
	IO6_CANB_TX          = 0xC0032606U,
	IO6_SPIB_SOMI        = 0xC0075706U,
	IO6_FSITXA_D0        = 0xC0100906U,

	IO7_DIN              = 0xC0020007U,
	IO7_DOUT             = 0xC0021007U,
	IO7_EPWM4_B          = 0xC0087107U,
	IO7_OUTXBAR5         = 0xC00A4307U,
	IO7_EQEP1_B          = 0xC00D1507U,
	IO7_CANB_RX          = 0xC0033607U,
	IO7_SPIB_SIMO        = 0xC0074707U,
	IO7_FSITXA_CLK       = 0xC0102907U,

	IO8_DIN              = 0xC0020008U,
	IO8_DOUT             = 0xC0021008U,
	IO8_EPWM5_A          = 0xC0088108U,
	IO8_CANB_TX          = 0xC0032208U,
	IO8_ADCSOCAO         = 0xC0007308U,
	IO8_EQEP1_STROBE     = 0xC00D3508U,
	IO8_SCIA_TX          = 0xC0050608U,
	IO8_SPIA_SIMO        = 0xC0070708U,
	IO8_I2CA_SCL         = 0xC0041908U,
	IO8_FSITXA_D1        = 0xC0101A08U,

	IO9_DIN              = 0xC0020009U,
	IO9_DOUT             = 0xC0021009U,
	IO9_EPWM5_B          = 0xC0089109U,
	IO9_SCIB_TX          = 0xC0052209U,
	IO9_OUTXBAR6         = 0xC00A5309U,
	IO9_EQEP1_INDEX      = 0xC00D2509U,
	IO9_SCIA_RX          = 0xC0051609U,
	IO9_SPIA_CLK         = 0xC0072709U,
	IO9_FSITXA_D0        = 0xC0100A09U,

	IO10_DIN             = 0xC002000AU,
	IO10_DOUT            = 0xC002100AU,
	IO10_EPWM6_A         = 0xC008A10AU,
	IO10_CANB_RX         = 0xC003320AU,
	IO10_ADCSOCBO        = 0xC000830AU,
	IO10_EQEP1_A         = 0xC00D050AU,
	IO10_SCIB_TX         = 0xC005260AU,
	IO10_SPIA_SOMI       = 0xC007170AU,
	IO10_I2CA_SDA        = 0xC004090AU,
	IO10_FSITXA_CLK      = 0xC0102A0AU,

	IO11_DIN             = 0xC002000BU,
	IO11_DOUT            = 0xC002100BU,
	IO11_EPWM6_B         = 0xC008B10BU,
	IO11_SCIB_RX         = 0xC005320BU,
	IO11_OUTXBAR7        = 0xC00A630BU,
	IO11_EQEP1_B         = 0xC00D150BU,
	IO11_SPIA_STE        = 0xC007370BU,
	IO11_FSIRXA_D1       = 0xC010490BU,

	IO12_DIN             = 0xC002000CU,
	IO12_DOUT            = 0xC002100CU,
	IO12_EPWM7_A         = 0xC008C10CU,
	IO12_CANB_TX         = 0xC003220CU,
	IO12_EQEP1_STROBE    = 0xC00D350CU,
	IO12_SCIB_TX         = 0xC005260CU,
	IO12_PMBUSA_CTL      = 0xC011270CU,
	IO12_FSIRXA_D0       = 0xC010390CU,

	IO13_DIN             = 0xC002000DU,
	IO13_DOUT            = 0xC002100DU,
	IO13_EPWM7_B         = 0xC008D10DU,
	IO13_CANB_RX         = 0xC003320DU,
	IO13_EQEP1_INDEX     = 0xC00D250DU,
	IO13_SCIB_RX         = 0xC005360DU,
	IO13_PMBUSA_ALERT    = 0xC011370DU,
	IO13_FSIRXA_CLK      = 0xC010590DU,

	IO14_DIN             = 0xC002000EU,
	IO14_DOUT            = 0xC002100EU,
	IO14_EPWM8_A         = 0xC008E10EU,
	IO14_SCIB_TX         = 0xC005220EU,
	IO14_OUTXBAR3        = 0xC00A260EU,
	IO14_PMBUSA_SDA      = 0xC011070EU,
	IO14_SPIB_CLK        = 0xC007690EU,
	IO14_EQEP2_A         = 0xC00D4A0EU,

	IO15_DIN             = 0xC002000FU,
	IO15_DOUT            = 0xC002100FU,
	IO15_EPWM8_B         = 0xC008F10FU,
	IO15_SCIB_RX         = 0xC005320FU,
	IO15_OUTXBAR4        = 0xC00A360FU,
	IO15_PMBUSA_SCL      = 0xC011170FU,
	IO15_SPIB_STE        = 0xC007790FU,
	IO15_EQEP2_B         = 0xC00D5A0FU,

	IO16_DIN             = 0xC0020010U,
	IO16_DOUT            = 0xC0021010U,
	IO16_SPIA_SIMO       = 0xC0070110U,
	IO16_CANB_TX         = 0xC0032210U,
	IO16_OUTXBAR7        = 0xC00A6310U,
	IO16_EPWM5_A         = 0xC0088510U,
	IO16_SCIA_TX         = 0xC0050610U,
	IO16_SD1_D1          = 0xC0090710U,
	IO16_EQEP1_STROBE    = 0xC00D3910U,
	IO16_PMBUSA_SCL      = 0xC0111A10U,
	IO16_XCLKOUT         = 0xC0005B10U,

	IO17_DIN             = 0xC0020011U,
	IO17_DOUT            = 0xC0021011U,
	IO17_SPIA_SOMI       = 0xC0071111U,
	IO17_CANB_RX         = 0xC0033211U,
	IO17_OUTXBAR8        = 0xC00A7311U,
	IO17_EPWM5_B         = 0xC0089511U,
	IO17_SCIA_RX         = 0xC0051611U,
	IO17_SD1_C1          = 0xC0091711U,
	IO17_EQEP1_INDEX     = 0xC00D2911U,
	IO17_PMBUSA_SDA      = 0xC0110A11U,

	IO18_DIN             = 0xC0020012U,
	IO18_DOUT            = 0xC0021012U,
	IO18_X2              = 0xC0009012U,
	IO18_SPIA_CLK        = 0xC0072112U,
	IO18_SCIB_TX         = 0xC0052212U,
	IO18_CANA_RX         = 0xC0031312U,
	IO18_EPWM6_A         = 0xC008A512U,
	IO18_I2CA_SCL        = 0xC0041612U,
	IO18_SD1_D2          = 0xC0092712U,
	IO18_EQEP2_A         = 0xC00D4912U,
	IO18_PMBUSA_CTL      = 0xC0112A12U,
	IO18_XCLKOUT         = 0xC0005B12U,

	IO22_AIN             = 0xC0010016U,
	IO22_DIN             = 0xC0020016U,
	IO22_DOUT            = 0xC0021016U,
	IO22_VFBSW           = 0xC000B016U,
	IO22_EQEP1_STROBE    = 0xC00D3116U,
	IO22_SCIB_TX         = 0xC0052316U,
	IO22_SPIB_CLK        = 0xC0076616U,
	IO22_SD1_D4          = 0xC0096716U,
	IO22_LINA_TX         = 0xC0060916U,

	IO23_AIN             = 0xC0010017U,
	IO23_DIN             = 0xC0020017U,
	IO23_DOUT            = 0xC0021017U,
	IO23_VSW             = 0xC000A017U,

	IO24_DIN             = 0xC0020018U,
	IO24_DOUT            = 0xC0021018U,
	IO24_OUTXBAR1        = 0xC00A0118U,
	IO24_EQEP2_A         = 0xC00D4218U,
	IO24_EPWM8_A         = 0xC008E518U,
	IO24_SPIB_SIMO       = 0xC0074618U,
	IO24_SD1_D1          = 0xC0090718U,
	IO24_PMBUSA_SCL      = 0xC0111A18U,
	IO24_SCIA_TX         = 0xC0050B18U,
	IO24_ERRORSTS        = 0xC0006D18U,

	IO25_DIN             = 0xC0020019U,
	IO25_DOUT            = 0xC0021019U,
	IO25_OUTXBAR2        = 0xC00A1119U,
	IO25_EQEP2_B         = 0xC00D5219U,
	IO25_SPIB_SOMI       = 0xC0075619U,
	IO25_SD1_C1          = 0xC0091719U,
	IO25_FSITXA_D1       = 0xC0101919U,
	IO25_PMBUSA_SDA      = 0xC0110A19U,
	IO25_SCIA_RX         = 0xC0051B19U,

	IO26_DIN             = 0xC002001AU,
	IO26_DOUT            = 0xC002101AU,
	IO26_OUTXBAR3        = 0xC00A211AU,
	IO26_EQEP2_INDEX     = 0xC00D621AU,
	IO26_SPIB_CLK        = 0xC007661AU,
	IO26_SD1_D2          = 0xC009271AU,
	IO26_FSITXA_D0       = 0xC010091AU,
	IO26_PMBUSA_CTL      = 0xC0112A1AU,
	IO26_I2CA_SDA        = 0xC0040B1AU,

	IO27_DIN             = 0xC002001BU,
	IO27_DOUT            = 0xC002101BU,
	IO27_OUTXBAR4        = 0xC00A311BU,
	IO27_EQEP2_STROBE    = 0xC00D721BU,
	IO27_SPIB_STE        = 0xC007761BU,
	IO27_SD1_C2          = 0xC009371BU,
	IO27_FSITXA_CLK      = 0xC010291BU,
	IO27_PMBUSA_ALERT    = 0xC0113A1BU,
	IO27_I2CA_SCL        = 0xC0041B1BU,

	IO28_DIN             = 0xC002001CU,
	IO28_DOUT            = 0xC002101CU,
	IO28_SCIA_RX         = 0xC005111CU,
	IO28_EPWM7_A         = 0xC008C31CU,
	IO28_OUTXBAR5        = 0xC00A451CU,
	IO28_EQEP1_A         = 0xC00D061CU,
	IO28_SD1_D3          = 0xC009471CU,
	IO28_EQEP2_STROBE    = 0xC00D791CU,
	IO28_LINA_TX         = 0xC0060A1CU,
	IO28_SPIB_CLK        = 0xC0076B1CU,
	IO28_ERRORSTS        = 0xC0006D1CU,

	IO29_DIN             = 0xC002001DU,
	IO29_DOUT            = 0xC002101DU,
	IO29_SCIA_TX         = 0xC005011DU,
	IO29_EPWM7_B         = 0xC008D31DU,
	IO29_OUTXBAR6        = 0xC00A551DU,
	IO29_EQEP1_B         = 0xC00D161DU,
	IO29_SD1_C3          = 0xC009571DU,
	IO29_EQEP2_INDEX     = 0xC00D691DU,
	IO29_LINA_RX         = 0xC0061A1DU,
	IO29_SPIB_STE        = 0xC0077B1DU,
	IO29_ERRORSTS        = 0xC0006D1DU,

	IO30_DIN             = 0xC002001EU,
	IO30_DOUT            = 0xC002101EU,
	IO30_CANA_RX         = 0xC003111EU,
	IO30_SPIB_SIMO       = 0xC007431EU,
	IO30_OUTXBAR7        = 0xC00A651EU,
	IO30_EQEP1_STROBE    = 0xC00D361EU,
	IO30_SD1_D4          = 0xC009671EU,

	IO31_DIN             = 0xC002001FU,
	IO31_DOUT            = 0xC002101FU,
	IO31_CANA_TX         = 0xC003011FU,
	IO31_SPIB_SOMI       = 0xC007531FU,
	IO31_OUTXBAR8        = 0xC00A751FU,
	IO31_EQEP1_INDEX     = 0xC00D261FU,
	IO31_SD1_C4          = 0xC009771FU,
	IO31_FSIRXA_D1       = 0xC010491FU,

	IO32_DIN             = 0xC0020020U,
	IO32_DOUT            = 0xC0021020U,
	IO32_I2CA_SDA        = 0xC0040120U,
	IO32_SPIB_CLK        = 0xC0076320U,
	IO32_EPWM8_B         = 0xC008F520U,
	IO32_LINA_TX         = 0xC0060620U,
	IO32_SD1_D3          = 0xC0094720U,
	IO32_FSIRXA_D0       = 0xC0103920U,
	IO32_CANA_TX         = 0xC0030A20U,

	IO33_DIN             = 0xC0020021U,
	IO33_DOUT            = 0xC0021021U,
	IO33_I2CA_SCL        = 0xC0041121U,
	IO33_SPIB_STE        = 0xC0077321U,
	IO33_OUTXBAR4        = 0xC00A3521U,
	IO33_LINA_RX         = 0xC0061621U,
	IO33_SD1_C3          = 0xC0095721U,
	IO33_FSIRXA_CLK      = 0xC0105921U,
	IO33_CANA_RX         = 0xC0031A21U,

	IO34_DIN             = 0xC0020022U,
	IO34_DOUT            = 0xC0021022U,
	IO34_OUTXBAR1        = 0xC00A0122U,
	IO34_PMBUSA_SDA      = 0xC0110622U,

	IO35_DIN             = 0xC0020023U,
	IO35_DOUT            = 0xC0021023U,
	IO35_SCIA_RX         = 0xC0051123U,
	IO35_I2CA_SDA        = 0xC0040323U,
	IO35_CANA_RX         = 0xC0031523U,
	IO35_PMBUSA_SCL      = 0xC0111623U,
	IO35_LINA_RX         = 0xC0061723U,
	IO35_EQEP1_A         = 0xC00D0923U,
	IO35_PMBUSA_CTL      = 0xC0112A23U,
	IO35_JTAG_TDI        = 0xC0120F23U,

	IO37_DIN             = 0xC0020025U,
	IO37_DOUT            = 0xC0021025U,
	IO37_OUTXBAR2        = 0xC00A1125U,
	IO37_I2CA_SCL        = 0xC0041325U,
	IO37_SCIA_TX         = 0xC0050525U,
	IO37_CANA_TX         = 0xC0030625U,
	IO37_LINA_TX         = 0xC0060725U,
	IO37_EQEP1_B         = 0xC00D1925U,
	IO37_PMBUSA_ALERT    = 0xC0113A25U,
	IO37_JTAG_TDO        = 0xC0121F25U,

	IO39_DIN             = 0xC0020027U,
	IO39_DOUT            = 0xC0021027U,
	IO39_CANB_RX         = 0xC0033627U,
	IO39_FSIRXA_CLK      = 0xC0105727U,

	IO40_DIN             = 0xC0020028U,
	IO40_DOUT            = 0xC0021028U,
	IO40_PMBUSA_SDA      = 0xC0110628U,
	IO40_FSIRXA_D0       = 0xC0103728U,
	IO40_SCIB_TX         = 0xC0052928U,
	IO40_EQEP1_A         = 0xC00D0A28U,

	IO56_DIN             = 0xC0020038U,
	IO56_DOUT            = 0xC0021038U,
	IO56_SPIA_CLK        = 0xC0072138U,
	IO56_EQEP2_STROBE    = 0xC00D7538U,
	IO56_SCIB_TX         = 0xC0052638U,
	IO56_SD1_D3          = 0xC0094738U,
	IO56_SPIB_SIMO       = 0xC0074938U,
	IO56_EQEP1_A         = 0xC00D0B38U,

	IO57_DIN             = 0xC0020039U,
	IO57_DOUT            = 0xC0021039U,
	IO57_SPIA_STE        = 0xC0073139U,
	IO57_EQEP2_INDEX     = 0xC00D6539U,
	IO57_SCIB_RX         = 0xC0053639U,
	IO57_SD1_C3          = 0xC0095739U,
	IO57_SPIB_SOMI       = 0xC0075939U,
	IO57_EQEP1_B         = 0xC00D1B39U,

	IO58_DIN             = 0xC002003AU,
	IO58_DOUT            = 0xC002103AU,
	IO58_OUTXBAR1        = 0xC00A053AU,
	IO58_SPIB_CLK        = 0xC007663AU,
	IO58_SD1_D4          = 0xC009673AU,
	IO58_LINA_TX         = 0xC006093AU,
	IO58_CANB_TX         = 0xC0032A3AU,
	IO58_EQEP1_STROBE    = 0xC00D3B3AU,

	IO59_DIN             = 0xC002003BU,
	IO59_DOUT            = 0xC002103BU,
	IO59_OUTXBAR2        = 0xC00A153BU,
	IO59_SPIB_STE        = 0xC007763BU,
	IO59_SD1_C4          = 0xC009773BU,
	IO59_LINA_RX         = 0xC006193BU,
	IO59_CANB_RX         = 0xC0033A3BU,
	IO59_EQEP1_INDEX     = 0xC00D2B3BU,

	IO224_AIN            = 0xC00100E0U,
	IO224_DIN            = 0xC00200E0U,

	IO225_AIN            = 0xC00100E1U,
	IO225_DIN            = 0xC00200E1U,

	IO226_AIN            = 0xC00100E2U,
	IO226_DIN            = 0xC00200E2U,

	IO227_AIN            = 0xC00100E3U,
	IO227_DIN            = 0xC00200E3U,

	IO228_AIN            = 0xC00100E4U,
	IO228_DIN            = 0xC00200E4U,

	IO229_AIN            = 0xC00100E5U,
	IO229_DIN            = 0xC00200E5U,

	IO230_AIN            = 0xC00100E6U,
	IO230_DIN            = 0xC00200E6U,

	IO231_AIN            = 0xC00100E7U,
	IO231_DIN            = 0xC00200E7U,

	IO232_AIN            = 0xC00100E8U,
	IO232_DIN            = 0xC00200E8U,

	IO233_AIN            = 0xC00100E9U,
	IO233_DIN            = 0xC00200E9U,

	IO234_AIN            = 0xC00100EAU,
	IO234_DIN            = 0xC00200EAU,

	IO235_AIN            = 0xC00100EBU,
	IO235_DIN            = 0xC00200EBU,

	IO236_AIN            = 0xC00100ECU,
	IO236_DIN            = 0xC00200ECU,

	IO237_AIN            = 0xC00100EDU,
	IO237_DIN            = 0xC00200EDU,

	IO238_AIN            = 0xC00100EEU,
	IO238_DIN            = 0xC00200EEU,

	IO239_AIN            = 0xC00100EFU,
	IO239_DIN            = 0xC00200EFU,

	IO240_AIN            = 0xC00100F0U,
	IO240_DIN            = 0xC00200F0U,

	IO241_AIN            = 0xC00100F1U,
	IO241_DIN            = 0xC00200F1U,

	IO242_AIN            = 0xC00100F2U,
	IO242_DIN            = 0xC00200F2U,

	IO243_AIN            = 0xC00100F3U,
	IO243_DIN            = 0xC00200F3U,

	IO244_AIN            = 0xC00100F4U,
	IO244_DIN            = 0xC00200F4U,

	IO245_AIN            = 0xC00100F5U,
	IO245_DIN            = 0xC00200F5U,

	IO246_AIN            = 0xC00100F6U,
	IO246_DIN            = 0xC00200F6U,

	IO247_AIN            = 0xC00100F7U,
	IO247_DIN            = 0xC00200F7U
};
#else
#error "io.h - dsp not defined"
#endif

/**************************************************************************************************
 * 
 * I/O pin function code definition. If CPU type is not explicitly defined in parenthesis then
 * the function is available on both f2803x and f28004x devices.
 * 
 *************************************************************************************************/
enum io_fun {
    IO_NONE         = 0x0000U,      /* I/O function not initalized                              */
    IO_TODO         = 0x0001U,      /* Peripheral function not supported by the driver          */
    IO_SYNCIN       = 0x0002U,      /* ePWM synchronization input line                 (f2803x) */
    IO_SYNCOUT      = 0x0003U,      /* ePWM synchronization output line                         */
    IO_XCLKIN       = 0x0004U,      /* External clock input line                       (f2803x) */
    IO_XCLKOUT      = 0x0005U,      /* External clock output line                               */
    IO_ERRORSTS     = 0x0006U,      /* Error status output line                       (f28004x) */
    IO_ADCSOCAO     = 0x0007U,      /* ADC start of conversion A output for external ADC        */
    IO_ADCSOCBO     = 0x0008U,      /* ADC start of conversion B output for external ADC        */
    IO_X2           = 0x0009U,      /* Crystal oscillator output                      (f28004x) */
    IO_VSW          = 0x000AU,      /* Switching output of internal DC/DC regulator   (f28004x) */
    IO_VFBSW        = 0x000BU,      /* Feedback signal of internal DC/DC regulator    (f28004x) */
    IO_AIN          = 0x0010U,      /* Analog input                                             */
    IO_AOUT         = 0x0011U,      /* Analog output                                   (f2803x) */
    IO_DIN          = 0x0020U,      /* Primary I/O function - digital input (default on reset)  */
    IO_DOUT         = 0x0021U,      /* Primary I/O function - digital output                    */
    IO_CANA_TX      = 0x0030U,      /* CAN-A transmit line                                      */
    IO_CANA_RX      = 0x0031U,      /* CAN-A receive line                                       */
    IO_CANB_TX      = 0x0032U,      /* CAN-B transmit line                            (f28004x) */
    IO_CANB_RX      = 0x0033U,      /* CAN-B receive line                             (f28004x) */
    IO_I2CA_SDA     = 0x0040U,      /* I2C-A data line                                          */
    IO_I2CA_SCL     = 0x0041U,      /* I2C-A clock line                                         */
    IO_SCIA_TX      = 0x0050U,      /* SCI-A transmit line                                      */
    IO_SCIA_RX      = 0x0051U,      /* SCI-A receive line                                       */
    IO_SCIB_TX      = 0x0052U,      /* SCI-B transmit line                            (f28004x) */
    IO_SCIB_RX      = 0x0053U,      /* SCI-B receive line                             (f28004x) */
    IO_LINA_TX      = 0x0060U,      /* LIN-A transmit line                                      */
    IO_LINA_RX      = 0x0061U,      /* LIN-A receive line                                       */
    IO_SPIA_SIMO    = 0x0070U,      /* SPI-A slave-in master-out line                           */
    IO_SPIA_SOMI    = 0x0071U,      /* SPI-A slave-out master-in line                           */
    IO_SPIA_CLK     = 0x0072U,      /* SPI-A serial-clock line                                  */
    IO_SPIA_STE     = 0x0073U,      /* SPI-A transmit-enable line                               */
    IO_SPIB_SIMO    = 0x0074U,      /* SPI-B slave-in master-out line                           */
    IO_SPIB_SOMI    = 0x0075U,      /* SPI-B slave-out master-in line                           */
    IO_SPIB_CLK     = 0x0076U,      /* SPI-B serial-clock line                                  */
    IO_SPIB_STE     = 0x0077U,      /* SPI-B transmit-enable line                               */
    IO_EPWM1_A      = 0x0080U,      /* ePWM1-A line                                             */
    IO_EPWM1_B      = 0x0081U,      /* ePWM1-B line                                             */
    IO_EPWM2_A      = 0x0082U,      /* ePWM2-A line                                             */
    IO_EPWM2_B      = 0x0083U,      /* ePWM2-B line                                             */
    IO_EPWM3_A      = 0x0084U,      /* ePWM3-A line                                             */
    IO_EPWM3_B      = 0x0085U,      /* ePWM3-B line                                             */
    IO_EPWM4_A      = 0x0086U,      /* ePWM4-A line                                             */
    IO_EPWM4_B      = 0x0087U,      /* ePWM4-B line                                             */
    IO_EPWM5_A      = 0x0088U,      /* ePWM5-A line                                             */
    IO_EPWM5_B      = 0x0089U,      /* ePWM5-B line                                             */
    IO_EPWM6_A      = 0x008AU,      /* ePWM6-A line                                             */
    IO_EPWM6_B      = 0x008BU,      /* ePWM6-B line                                             */
    IO_EPWM7_A      = 0x008CU,      /* ePWM7-A line                                             */
    IO_EPWM7_B      = 0x008DU,      /* ePWM7-B line                                             */
    IO_EPWM8_A      = 0x008EU,      /* ePWM8-A line                                   (f28004x) */
    IO_EPWM8_B      = 0x008FU,      /* ePWM8-B line                                   (f28004x) */
    IO_SD1_D1       = 0x0090U,      /* Sigma-delta 1 data 1 line                      (f28004x) */
    IO_SD1_C1       = 0x0091U,      /* Sigma-delta 1 clock 1 line                     (f28004x) */
    IO_SD1_D2       = 0x0092U,      /* Sigma-delta 1 data 2 line                      (f28004x) */
    IO_SD1_C2       = 0x0093U,      /* Sigma-delta 1 clock 2 line                     (f28004x) */
    IO_SD1_D3       = 0x0094U,      /* Sigma-delta 1 data 3 line                      (f28004x) */
    IO_SD1_C3       = 0x0095U,      /* Sigma-delta 1 clock 3 line                     (f28004x) */
    IO_SD1_D4       = 0x0096U,      /* Sigma-delta 1 data 4 line                      (f28004x) */
    IO_SD1_C4       = 0x0097U,      /* Sigma-delta 1 clock 4 line                     (f28004x) */
    IO_OUTXBAR1     = 0x00A0U,      /* Output XBAR1                                   (f28004x) */
    IO_OUTXBAR2     = 0x00A1U,      /* Output XBAR2                                   (f28004x) */
    IO_OUTXBAR3     = 0x00A2U,      /* Output XBAR3                                   (f28004x) */
    IO_OUTXBAR4     = 0x00A3U,      /* Output XBAR4                                   (f28004x) */
    IO_OUTXBAR5     = 0x00A4U,      /* Output XBAR5                                   (f28004x) */
    IO_OUTXBAR6     = 0x00A5U,      /* Output XBAR6                                   (f28004x) */
    IO_OUTXBAR7     = 0x00A6U,      /* Output XBAR7                                   (f28004x) */
    IO_OUTXBAR8     = 0x00A7U,      /* Output XBAR8                                   (f28004x) */
    IO_TZ1          = 0x00B0U,      /* Trip zone 1 line                                (f2803x) */
    IO_TZ2          = 0x00B1U,      /* Trip zone 2 line                                (f2803x) */
    IO_TZ3          = 0x00B2U,      /* Trip zone 3 line                                (f2803x) */
    IO_COMP1_OUT    = 0x00C0U,      /* Comparator 1 ouput                              (f2803x) */
    IO_COMP2_OUT    = 0x00C1U,      /* Comparator 2 ouput                              (f2803x) */
    IO_COMP3_OUT    = 0x00C2U,      /* Comparator 3 ouput                              (f2803x) */
    IO_EQEP1_A      = 0x00D0U,      /* eQEP1 A quadrature line                                  */
    IO_EQEP1_B      = 0x00D1U,      /* eQEP1 B quadrature line                                  */
    IO_EQEP1_INDEX  = 0x00D2U,      /* eQEP1 index line                                         */
    IO_EQEP1_STROBE = 0x00D3U,      /* eQEP1 strobe input line                                  */
    IO_EQEP2_A      = 0x00D4U,      /* eQEP2 A quadrature line                        (f28004x) */
    IO_EQEP2_B      = 0x00D5U,      /* eQEP2 B quadrature line                        (f28004x) */
    IO_EQEP2_INDEX  = 0x00D6U,      /* eQEP2 index line                               (f28004x) */
    IO_EQEP2_STROBE = 0x00D7U,      /* eQEP2 strobe input line                        (f28004x) */
    IO_ECAP1        = 0x00E0U,      /* eCAP1 line                                      (f2803x) */
    IO_HRCAP1       = 0x00F0U,      /* HRCAP1 line                                     (f2803x) */
    IO_HRCAP2       = 0x00F1U,      /* HRCAP2 line                                     (f2803x) */
    IO_FSITXA_D0    = 0x0100U,      /* FSI-A transmit data 0 line                     (f28004x) */
    IO_FSITXA_D1    = 0x0101U,      /* FSI-A transmit data 1 line                     (f28004x) */
    IO_FSITXA_CLK   = 0x0102U,      /* FSI-A transmit clock line                      (f28004x) */
    IO_FSIRXA_D0    = 0x0103U,      /* FSI-A receive data 0 line                      (f28004x) */
    IO_FSIRXA_D1    = 0x0104U,      /* FSI-A receive data 1 line                      (f28004x) */
    IO_FSIRXA_CLK   = 0x0105U,      /* FSI-A receive clock line                       (f28004x) */
    IO_PMBUSA_SDA   = 0x0110U,      /* PMBUS-A data line                              (f28004x) */
    IO_PMBUSA_SCL   = 0x0111U,      /* PMBUS-A clock line                             (f28004x) */
    IO_PMBUSA_CTL   = 0x0112U,      /* PMBUS-A control line                           (f28004x) */
    IO_PMBUSA_ALERT = 0x0113U,      /* PMBUS-A alert line                             (f28004x) */
    IO_JTAG_TDI     = 0x0120U,      /* JTAG test data input line                                */
    IO_JTAG_TDO     = 0x0121U,      /* JTAG test data output line                               */
    IO_JTAG_TMS     = 0x0122U,      /* JTAG test mode select line                      (f2803x) */
    IO_JTAG_TCK     = 0x0123U       /* JTAG test clock line                            (f2803x) */
};

/**************************************************************************************************
 * 
 * Group of functions from \e io_fun enumeration
 * 
 *************************************************************************************************/
enum io_group {
    IO_NO_GROUP = 0x0000U,          /* Unspecified peripheral function                          */
    IO_AIO      = 0x0010U,          /* Analog input or output                                   */
    IO_DIO      = 0x0020U,          /* Digital input or output                                  */
    IO_CAN      = 0x0030U,          /* Controller area network                                  */
    IO_I2C      = 0x0040U,          /* Inter-integrated circuit                                 */
    IO_SCI      = 0x0050U,          /* Serial communication interface                           */
    IO_LIN      = 0x0060U,          /* Local interconnect network                               */
    IO_SPI      = 0x0070U,          /* Serial peripheral interface                              */
    IO_EPWM     = 0x0080U,          /* Enhanced pulse width modulator                           */
    IO_SD       = 0x0090U,          /* Sigma-delta filter                             (f28004x) */
    IO_OUTXBAR  = 0x00A0U,          /* Output crossbar                                (f28004x) */
    IO_TZ       = 0x00B0U,          /* Trip zone                                       (f2803x) */
    IO_COMP     = 0x00C0U,          /* Comparator output                               (f2803x) */
    IO_EQEP     = 0x00D0U,          /* Enhanced quadrature encode pulse                         */
    IO_ECAP     = 0x00E0U,          /* Enhanced capture                                (f2803x) */
    IO_HRCAP    = 0x00F0U,          /* High resolution capture                         (f2803x) */
    IO_FSI      = 0x0100U,          /* Fast serial interface                          (f28004x) */
    IO_PMBUS    = 0x0110U,          /* Power management bus                           (f28004x) */
    IO_JTAG     = 0x0120U           /* JTAG communication                                       */
};

/**************************************************************************************************
 * 
 * \brief Resets all I/O pins to default state
 * 
 * \param None
 * 
 * \note Application must reset I/O driver before first use.
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
io_reset(void);

/**************************************************************************************************
 * 
 * \brief Connects peripheral function to the I/O pin
 * 
 * \param io I/O pin configuration code
 * \param exp expected I/O pin function
 * 
 * \details This function connects peripheral function to the I/O pin, as specified by the \e io
 * parameter. Pin must be disconnected before new peripheral function can be connected.
 * 
 * If expected I/O pin function is specified, i.e. if the \e exp parameter is not \e IO_NONE, this
 * function also compares I/O configuration code against expected function code before connecting
 * to the I/O pin.
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
io_connect(enum io io, enum io_fun exp);

/**************************************************************************************************
 * 
 * \brief Disconnects peripheral function from the I/O pin
 * 
 * \param io I/O pin configuration code
 * 
 * \details This function disconnects peripheral function from the I/O pin, as specified by the
 * \e io parameter. It returns I/O pin in its default configuration state.
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
io_disconnect(enum io io);

/**************************************************************************************************
 * 
 * \brief Returns I/O pin connection status
 * 
 * \param io I/O pin configuration code
 * \param pin storage for I/O pin number
 * \param fun storage for I/O peripheral function
 * \param group storage for I/O peripheral function group
 * 
 * \details This function returns I/O pin connection status. It also returns I/O pin number and
 * connected peripheral function. Note that all available I/O pins have some peripheral function
 * connected to it. The connection status only indicates if I/O pin has been configured by the
 * application. If I/O pin is not available, the function returns \e IO_NONE in \e fun parameter.
 * 
 * \return \b true when I/O pin is connected
 * 
 *************************************************************************************************/
extern bool
io_status(enum io io, uint32_t *pin, enum io_fun *fun, enum io_group *group);

/**************************************************************************************************
 * 
 * \brief Decodes I/O pin configuration code
 * 
 * \param io I/O pin configuration code
 * \param pin storage for I/O pin number
 * \param fun storage for I/O peripheral function
 * \param group storage for I/O peripheral function group
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
io_decode(enum io io, uint32_t *pin, enum io_fun *fun, enum io_group *group);

#endif /* _INC_DRV_IO_H */
