/**************************************************************************************************
 * 
 * \file can.h
 * 
 * \brief CAN driverlib implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "can.h"

/**************************************************************************************************
 * 
 * CAN_setBitTiming()
 * 
 *************************************************************************************************/
void
CAN_setBitTiming(uint32_t base, uint16_t brp, uint16_t tSeg1, uint16_t tSeg2, uint16_t sjw)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT((brp > 0U) && (brp <= 255U));
    ASSERT((tSeg1 > 0U) && (tSeg1 <= 15U));
    ASSERT((tSeg2 > 0U) && (tSeg2 <= 7U));
    ASSERT(tSeg1 >= tSeg2);
    ASSERT((sjw <= 3U) && (sjw <= tSeg2));

    CAN_startConfigChange(base);

    EALLOW;

    HW_FLD_SET(base, CAN, BTC, BRP, brp);
    HW_FLD_SET(base, CAN, BTC, SJW, sjw);
    HW_FLD_SET(base, CAN, BTC, TSEG1, tSeg1);
    HW_FLD_SET(base, CAN, BTC, TSEG2, tSeg2);

    if (brp > 3U) {
        HW_BIT_SET(base, CAN, BTC, SAM);
    } else { /* brp < 3U */
        HW_BIT_CLR(base, CAN, BTC, SAM);
    }

    EDIS;

    CAN_stopConfigChange(base);
}

/**************************************************************************************************
 * 
 * CAN_readData()
 * 
 *************************************************************************************************/
void
CAN_readData(uint32_t base, uint16_t mbx, uint16_t *data, uint16_t length)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT(mbx >= 1U && mbx <= 32U);
    ASSERT(data && length <= 8U);

    uint16_t offset = CAN_MBX_OFFSET(mbx);

    bool dbo = HW_BIT_GET(base, CAN, MC, DBO);

    uint32_t mdlReg = HW_REG_GETO(base, offset, CAN, MDL);
    uint32_t mdhReg = HW_REG_GETO(base, offset, CAN, MDH);

    uint16_t i;
    for (i = 0U; i < length; i++) {
        uint16_t shift = ((dbo ? i : (7U - i)) & 0x3U) << 3U;

        uint32_t dataReg = (i <= 3U) ? mdlReg : mdhReg;

        data[i] = (uint16_t) ((dataReg >> shift) & 0x00FFU);
    }
}

/**************************************************************************************************
 * 
 * CAN_writeData()
 * 
 *************************************************************************************************/
void
CAN_writeData(uint32_t base, uint16_t mbx, const uint16_t *data, uint16_t length)
{
    ASSERT(CAN_isBaseValid(base));
    ASSERT(mbx >= 1U && mbx <= 32U);
    ASSERT(data && length <= 8U);

    uint16_t offset = CAN_MBX_OFFSET(mbx);

    bool dbo = HW_BIT_GET(base, CAN, MC, DBO);

    uint32_t mdlReg = HW_REG_GETO(base, offset, CAN, MDL);
    uint32_t mdhReg = HW_REG_GETO(base, offset, CAN, MDH);

    uint16_t i;
    for (i = 0U; i < length; i++) {
        uint16_t shift = ((dbo ? i : (7U - i)) & 0x3U) << 3U;

        uint32_t mask = 0x000000FFUL << shift;
        uint32_t byte = (uint32_t) (data[i] & 0x00FFU) << shift;

        uint32_t *dataReg = (i <= 3U) ? &mdlReg : &mdhReg;

        *dataReg &= ~mask;
        *dataReg |= byte;
    }

    HW_REG_SETO(base, offset, CAN, MDL, mdlReg);
    HW_REG_SETO(base, offset, CAN, MDH, mdhReg);
}
