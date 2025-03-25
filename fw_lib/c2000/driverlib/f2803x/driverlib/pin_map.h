/**************************************************************************************************
 * 
 * \file pin_map.h
 * 
 * \brief Definitions of pin functions for GPIO driverlib and I/O driver
 * 
 * \details The data is encoded as follows:
 * - 0x00000003     MUX register value
 * - 0x0000000C     GMUX register value (always 0)
 * - 0x0000FF00     Shift amount within mux registers
 * - 0xFFFF0000     MUX register offset
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef PIN_MAP_H
#define PIN_MAP_H

#define GPIO_0_GPIO0                    0x00060000U
#define GPIO_0_EPWM1_A                  0x00060001U

#define GPIO_1_GPIO1                    0x00060200U
#define GPIO_1_EPWM1_B                  0x00060201U
#define GPIO_1_COMP1_OUT                0x00060203U

#define GPIO_2_GPIO2                    0x00060400U
#define GPIO_2_EPWM2_A                  0x00060401U

#define GPIO_3_GPIO3                    0x00060600U
#define GPIO_3_EPWM2_B                  0x00060601U
#define GPIO_3_SPIA_SOMI                0x00060602U
#define GPIO_3_COMP2_OUT                0x00060603U

#define GPIO_4_GPIO4                    0x00060800U
#define GPIO_4_EPWM3_A                  0x00060801U

#define GPIO_5_GPIO5                    0x00060A00U
#define GPIO_5_EPWM3_B                  0x00060A01U
#define GPIO_5_SPIA_SIMO                0x00060A02U
#define GPIO_5_ECAP1                    0x00060A03U

#define GPIO_6_GPIO6                    0x00060C00U
#define GPIO_6_EPWM4_A                  0x00060C01U
#define GPIO_6_SYNCIN                   0x00060C02U
#define GPIO_6_SYNCOUT                  0x00060C03U

#define GPIO_7_GPIO7                    0x00060E00U
#define GPIO_7_EPWM4_B                  0x00060E01U
#define GPIO_7_SCIA_RX                  0x00060E02U

#define GPIO_8_GPIO8                    0x00061000U
#define GPIO_8_EPWM5_A                  0x00061001U
#define GPIO_8_ADCSOCAO                 0x00061003U

#define GPIO_9_GPIO9                    0x00061200U
#define GPIO_9_EPWM5_B                  0x00061201U
#define GPIO_9_LINA_TX                  0x00061202U
#define GPIO_9_HRCAP1                   0x00061203U

#define GPIO_10_GPIO10                  0x00061400U
#define GPIO_10_EPWM6_A                 0x00061401U
#define GPIO_10_ADCSOCBO                0x00061403U

#define GPIO_11_GPIO11                  0x00061600U
#define GPIO_11_EPWM6_B                 0x00061601U
#define GPIO_11_LINA_RX                 0x00061602U
#define GPIO_11_HRCAP2                  0x00061603U

#define GPIO_12_GPIO12                  0x00061800U
#define GPIO_12_TZ1                     0x00061801U
#define GPIO_12_SCIA_TX                 0x00061802U
#define GPIO_12_SPIB_SIMO               0x00061803U

#define GPIO_13_GPIO13                  0x00061A00U
#define GPIO_13_TZ2                     0x00061A01U
#define GPIO_13_SPIB_SOMI               0x00061A03U

#define GPIO_14_GPIO14                  0x00061C00U
#define GPIO_14_TZ3                     0x00061C01U
#define GPIO_14_LINA_TX                 0x00061C02U
#define GPIO_14_SPIB_CLK                0x00061C03U

#define GPIO_15_GPIO15                  0x00061E00U
#define GPIO_15_TZ1                     0x00061E01U
#define GPIO_15_LINA_RX                 0x00061E02U
#define GPIO_15_SPIB_STE                0x00061E03U

#define GPIO_16_GPIO16                  0x00080000U
#define GPIO_16_SPIA_SIMO               0x00080001U
#define GPIO_16_TZ2                     0x00080003U

#define GPIO_17_GPIO17                  0x00080200U
#define GPIO_17_SPIA_SOMI               0x00080201U
#define GPIO_17_TZ3                     0x00080203U

#define GPIO_18_GPIO18                  0x00080400U
#define GPIO_18_SPIA_CLK                0x00080401U
#define GPIO_18_LINA_TX                 0x00080402U
#define GPIO_18_XCLKOUT                 0x00080403U

#define GPIO_19_GPIO19_XCLKIN           0x00080600U
#define GPIO_19_SPIA_STE                0x00080601U
#define GPIO_19_LINA_RX                 0x00080602U
#define GPIO_19_ECAP1                   0x00080603U

#define GPIO_20_GPIO20                  0x00080800U
#define GPIO_20_EQEP1_A                 0x00080801U
#define GPIO_20_COMP1_OUT               0x00080803U

#define GPIO_21_GPIO21                  0x00080A00U
#define GPIO_21_EQEP1_B                 0x00080A01U
#define GPIO_21_COMP2_OUT               0x00080A03U

#define GPIO_22_GPIO22                  0x00080C00U
#define GPIO_22_EQEP1_STROBE            0x00080C01U
#define GPIO_22_LINA_TX                 0x00080C03U

#define GPIO_23_GPIO23                  0x00080E00U
#define GPIO_23_EQEP1_INDEX             0x00080E01U
#define GPIO_23_LINA_RX                 0x00080E03U

#define GPIO_24_GPIO24                  0x00081000U
#define GPIO_24_ECAP1                   0x00081001U
#define GPIO_24_SPIB_SIMO               0x00081003U

#define GPIO_25_GPIO25                  0x00081200U
#define GPIO_25_SPIB_SOMI               0x00081203U

#define GPIO_26_GPIO26                  0x00081400U
#define GPIO_26_HRCAP1                  0x00081401U
#define GPIO_26_SPIB_CLK                0x00081403U

#define GPIO_27_GPIO27                  0x00081600U
#define GPIO_27_HRCAP2                  0x00081601U
#define GPIO_27_SPIB_STE                0x00081603U

#define GPIO_28_GPIO28                  0x00081800U
#define GPIO_28_SCIA_RX                 0x00081801U
#define GPIO_28_I2CA_SDA                0x00081802U
#define GPIO_28_TZ2                     0x00081803U

#define GPIO_29_GPIO29                  0x00081A00U
#define GPIO_29_SCIA_TX                 0x00081A01U
#define GPIO_29_I2CA_SCL                0x00081A02U
#define GPIO_29_TZ3                     0x00081A03U

#define GPIO_30_GPIO30                  0x00081C00U
#define GPIO_30_CANA_RX                 0x00081C01U

#define GPIO_31_GPIO31                  0x00081E00U
#define GPIO_31_CANA_TX                 0x00081E01U

#define GPIO_32_GPIO32                  0x00160000U
#define GPIO_32_I2CA_SDA                0x00160001U
#define GPIO_32_SYNCIN                  0x00160002U
#define GPIO_32_ADCSOCAO                0x00160003U

#define GPIO_33_GPIO33                  0x00160200U
#define GPIO_33_I2CA_SCL                0x00160201U
#define GPIO_33_SYNCOUT                 0x00160202U
#define GPIO_33_ADCSOCBO                0x00160203U

#define GPIO_34_GPIO34                  0x00160400U
#define GPIO_34_COMP2_OUT               0x00160401U
#define GPIO_34_COMP3_OUT               0x00160403U

#define GPIO_35_GPIO35_TDI              0x00160600U

#define GPIO_36_GPIO36_TMS              0x00160800U

#define GPIO_37_GPIO37_TDO              0x00160A00U

#define GPIO_38_GPIO38_XCLKIN_TCK       0x00160C00U

#define GPIO_39_GPIO39                  0x00160E00U

#define GPIO_40_GPIO40                  0x00161000U
#define GPIO_40_EPWM7_A                 0x00161001U

#define GPIO_41_GPIO41                  0x00161200U
#define GPIO_41_EPWM7_B                 0x00161201U

#define GPIO_42_GPIO42                  0x00161400U
#define GPIO_42_COMP1_OUT               0x00161403U

#define GPIO_43_GPIO43                  0x00161600U
#define GPIO_43_COMP2_OUT               0x00161603U

#define GPIO_44_GPIO44                  0x00161800U

#define GPIO_98_GPIO98                  0x00360400U

#define GPIO_100_GPIO100                0x00360800U

#define GPIO_102_GPIO102                0x00360C00U

#define GPIO_106_GPIO106                0x00361400U

#define GPIO_108_GPIO108                0x00361800U

#define GPIO_110_GPIO110                0x00361C00U

#endif /* PIN_MAP_H */
