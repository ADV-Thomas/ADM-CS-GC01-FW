/**************************************************************************************************
 * 
 * \file hw_i2c.h
 * 
 * \brief Definitions for I2C registers
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef HW_I2C_H
#define HW_I2C_H

/**************************************************************************************************
 * 
 * The following are defines for I2C registers (Sec. 13.6.2).
 * BASE REGISTER    I2CA_BASE
 * 
 *************************************************************************************************/

#define I2C_O_OAR       0x0U        /* Own address register                                     */
#define I2C_O_IER       0x1U        /* Interrupt enable register                                */
#define I2C_O_STR       0x2U        /* Status register                                          */
#define I2C_O_CLKL      0x3U        /* Clock low-time divider register                          */
#define I2C_O_CLKH      0x4U        /* Clock high-time divider register                         */
#define I2C_O_CNT       0x5U        /* Data count register                                      */
#define I2C_O_DRR       0x6U        /* Data receive register                                    */
#define I2C_O_SAR       0x7U        /* Slave address register                                   */
#define I2C_O_DXR       0x8U        /* Data transmit register                                   */
#define I2C_O_MDR       0x9U        /* Mode register                                            */
#define I2C_O_ISRC      0xAU        /* Interrupt source register                                */
#define I2C_O_EMDR      0xBU        /* Extended mode register                                   */
#define I2C_O_PSC       0xCU        /* Prescaler register                                       */
#define I2C_O_FFTX      0x20U       /* FIFO transmit register                                   */
#define I2C_O_FFRX      0x21U       /* FIFO receive register                                    */

#define I2C_S_OAR       1
#define I2C_S_IER       1
#define I2C_S_STR       1
#define I2C_S_CLKL      1
#define I2C_S_CLKH      1
#define I2C_S_CNT       1
#define I2C_S_DRR       1
#define I2C_S_SAR       1
#define I2C_S_DXR       1
#define I2C_S_MDR       1
#define I2C_S_ISRC      1
#define I2C_S_EMDR      1
#define I2C_S_PSC       1
#define I2C_S_FFTX      1
#define I2C_S_FFRX      1

#define I2C_OAR_OAR_S           0U
#define I2C_OAR_OAR_M           0x03FFU

#define I2C_IER_ARBL            0x0001U
#define I2C_IER_NACK            0x0002U
#define I2C_IER_ARDY            0x0004U
#define I2C_IER_RRDY            0x0008U
#define I2C_IER_XRDY            0x0010U
#define I2C_IER_SCD             0x0020U
#define I2C_IER_AAS             0x0040U

#define I2C_STR_ARBL            0x0001U
#define I2C_STR_NACK            0x0002U
#define I2C_STR_ARDY            0x0004U
#define I2C_STR_RRDY            0x0008U
#define I2C_STR_XRDY            0x0010U
#define I2C_STR_SCD             0x0020U
#define I2C_STR_AD0             0x0100U
#define I2C_STR_AAS             0x0200U
#define I2C_STR_XSMT            0x0400U
#define I2C_STR_RSFULL          0x0800U
#define I2C_STR_BB              0x1000U
#define I2C_STR_NACKSNT         0x2000U
#define I2C_STR_SDIR            0x4000U

#define I2C_CLKL_I2CCLKL_S      0U
#define I2C_CLKL_I2CCLKL_M      0xFFFFU

#define I2C_CLKH_I2CCLKH_S      0U
#define I2C_CLKH_I2CCLKH_M      0xFFFFU

#define I2C_CNT_I2CCNT_S        0U
#define I2C_CNT_I2CCNT_M        0xFFFFU

#define I2C_DRR_DATA_S          0U
#define I2C_DRR_DATA_M          0x00FFU

#define I2C_SAR_SAR_S           0U
#define I2C_SAR_SAR_M           0x03FFU

#define I2C_DXR_DATA_S          0U
#define I2C_DXR_DATA_M          0x00FFU

#define I2C_MDR_BC_S            0U
#define I2C_MDR_BC_M            0x0007U
#define I2C_MDR_FDF             0x0008U
#define I2C_MDR_STB             0x0010U
#define I2C_MDR_IRS             0x0020U
#define I2C_MDR_DLB             0x0040U
#define I2C_MDR_RM              0x0080U
#define I2C_MDR_XA              0x0100U
#define I2C_MDR_TRX             0x0200U
#define I2C_MDR_MST             0x0400U
#define I2C_MDR_STP             0x0800U
#define I2C_MDR_STT             0x2000U
#define I2C_MDR_FREE            0x4000U
#define I2C_MDR_NACKMOD         0x8000U

#define I2C_ISRC_INTCODE_S      0U
#define I2C_ISRC_INTCODE_M      0x0007U
#define I2C_ISRC_WRITEZEROS_S   8U
#define I2C_ISRC_WRITEZEROS_M   0x0F00U

#define I2C_EMDR_BC             0x0001U

#define I2C_PSC_IPSC_S          0U
#define I2C_PSC_IPSC_M          0x00FFU

#define I2C_FFTX_TXFFIL_S       0U
#define I2C_FFTX_TXFFIL_M       0x001FU
#define I2C_FFTX_TXFFIENA       0x0020U
#define I2C_FFTX_TXFFINTCLR     0x0040U
#define I2C_FFTX_TXFFINT        0x0080U
#define I2C_FFTX_TXFFST_S       8U
#define I2C_FFTX_TXFFST_M       0x1F00U
#define I2C_FFTX_TXFFRST        0x2000U
#define I2C_FFTX_I2CFFEN        0x4000U

#define I2C_FFRX_RXFFIL_S       0U
#define I2C_FFRX_RXFFIL_M       0x001FU
#define I2C_FFRX_RXFFIENA       0x0020U
#define I2C_FFRX_RXFFINTCLR     0x0040U
#define I2C_FFRX_RXFFINT        0x0080U
#define I2C_FFRX_RXFFST_S       8U
#define I2C_FFRX_RXFFST_M       0x1F00U
#define I2C_FFRX_RXFFRST        0x2000U

#endif /* HW_I2C_H */
