/**************************************************************************************************
 * 
 * \file clb.c
 * 
 * \brief CLB driver implementation
 * 
 * \author Jorge Sola
 * 
 *************************************************************************************************/

#include "inc/drv/clb.h"
#include "driverlib/clb.h"
#include "driverlib/sysctl.h"

/**************************************************************************************************
 * 
 * The DEFINES here are extracted from the driverlib examples library for FSI PWM synchronization
 * 
 *************************************************************************************************/

// HLC Instruction Register Field definitions
#define HLC_OPCODE_R0 0x0
#define HLC_OPCODE_R1 0x1
#define HLC_OPCODE_R2 0x2
#define HLC_OPCODE_R3 0x3
#define HLC_OPCODE_C0 0x4
#define HLC_OPCODE_C1 0x5
#define HLC_OPCODE_C2 0x6

#define HLC_OPCODE_MOV    0x00
#define HLC_OPCODE_MOV_T1 0x01
#define HLC_OPCODE_MOV_T2 0x02
#define HLC_OPCODE_PUSH   0x03
#define HLC_OPCODE_PULL   0x04
#define HLC_OPCODE_ADD    0x05
#define HLC_OPCODE_SUB    0x06
#define HLC_OPCODE_INTR   0x07

//---------------------------------------------------------------------------
// TILE1
//---------------------------------------------------------------------------
#define TILE1_CFG_OUTLUT_0 0x0
#define TILE1_CFG_OUTLUT_1 0x0
#define TILE1_CFG_OUTLUT_2 0x0
#define TILE1_CFG_OUTLUT_3 0x0
#define TILE1_CFG_OUTLUT_4 0x550018
#define TILE1_CFG_OUTLUT_5 0x550003
#define TILE1_CFG_OUTLUT_6 0x0
#define TILE1_CFG_OUTLUT_7 0x0

#define TILE1_CFG_LUT4_IN0   0x0
#define TILE1_CFG_LUT4_IN1   0x0
#define TILE1_CFG_LUT4_IN2   0x0
#define TILE1_CFG_LUT4_IN3   0x0
#define TILE1_CFG_LUT4_FN10  ((0x00000) | 0x0)
#define TILE1_CFG_LUT4_FN2   0x0

#define TILE1_CFG_FSM_EXT_IN0      0x18
#define TILE1_CFG_FSM_EXT_IN1      0x3
#define TILE1_CFG_FSM_EXTRA_IN0    0x0
#define TILE1_CFG_FSM_EXTRA_IN1    0x0
#define TILE1_CFG_FSM_NEXT_STATE_0 ((0xfc0000) | 0x0)
#define TILE1_CFG_FSM_NEXT_STATE_1 ((0x00000) | 0x0)
#define TILE1_CFG_FSM_NEXT_STATE_2 ((0x00000) | 0x0)
#define TILE1_CFG_FSM_LUT_FN10     ((0x00000) | 0x0)
#define TILE1_CFG_FSM_LUT_FN2      0x0
#define TILE1_FSM_MISC_CONTROL     0x0

#define TILE1_CFG_COUNTER_RESET   0x3
#define TILE1_CFG_COUNTER_EVENT   0x0
#define TILE1_CFG_COUNTER_MODE_0  0x5
#define TILE1_CFG_COUNTER_MODE_1  0x8
#define TILE1_CFG_TAP_SEL          0x0
#define TILE1_CFG_MISC_CONTROL    (0x0 | TILE1_FSM_MISC_CONTROL)

#define TILE1_COUNTER_0_MATCH1_VAL  37
#define TILE1_COUNTER_0_MATCH2_VAL  0
#define TILE1_COUNTER_0_LOAD_VAL    0
#define TILE1_COUNTER_1_MATCH1_VAL  0
#define TILE1_COUNTER_1_MATCH2_VAL  0
#define TILE1_COUNTER_1_LOAD_VAL    0
#define TILE1_COUNTER_2_MATCH1_VAL  0
#define TILE1_COUNTER_2_MATCH2_VAL  0
#define TILE1_COUNTER_2_LOAD_VAL    0


#define TILE1_HLC_EVENT_SEL 0x0
#define TILE1_HLC_R0_INIT 0
#define TILE1_HLC_R1_INIT 0
#define TILE1_HLC_R2_INIT 0
#define TILE1_HLC_R3_INIT 0

#define TILE1_HLCINSTR_0	(0 << 11 | HLC_OPCODE_MOV << 6 | HLC_OPCODE_C0<<3 | HLC_OPCODE_R0)
#define TILE1_HLCINSTR_1	(0 << 11 | HLC_OPCODE_ADD << 6 | HLC_OPCODE_R0<<3 | HLC_OPCODE_R1)
#define TILE1_HLCINSTR_2	(1 << 11 | HLC_OPCODE_INTR << 6 | 0X7)
#define TILE1_HLCINSTR_3	(1 << 11 | HLC_OPCODE_MOV << 6 | HLC_OPCODE_R0<<3 | HLC_OPCODE_R0)
#define TILE1_HLCINSTR_4	(1 << 11 | HLC_OPCODE_MOV << 6 | HLC_OPCODE_R0<<3 | HLC_OPCODE_R0)
#define TILE1_HLCINSTR_5	(1 << 11 | HLC_OPCODE_MOV << 6 | HLC_OPCODE_R0<<3 | HLC_OPCODE_R0)
#define TILE1_HLCINSTR_6	(1 << 11 | HLC_OPCODE_MOV << 6 | HLC_OPCODE_R0<<3 | HLC_OPCODE_R0)
#define TILE1_HLCINSTR_7	(1 << 11 | HLC_OPCODE_MOV << 6 | HLC_OPCODE_R0<<3 | HLC_OPCODE_R0)
#define TILE1_HLCINSTR_8	(1 << 11 | HLC_OPCODE_MOV << 6 | HLC_OPCODE_R0<<3 | HLC_OPCODE_R0)
#define TILE1_HLCINSTR_9	(1 << 11 | HLC_OPCODE_MOV << 6 | HLC_OPCODE_R0<<3 | HLC_OPCODE_R0)
#define TILE1_HLCINSTR_10	(1 << 11 | HLC_OPCODE_MOV << 6 | HLC_OPCODE_R0<<3 | HLC_OPCODE_R0)
#define TILE1_HLCINSTR_11	(1 << 11 | HLC_OPCODE_MOV << 6 | HLC_OPCODE_R0<<3 | HLC_OPCODE_R0)
#define TILE1_HLCINSTR_12	(1 << 11 | HLC_OPCODE_MOV << 6 | HLC_OPCODE_R0<<3 | HLC_OPCODE_R0)
#define TILE1_HLCINSTR_13	(1 << 11 | HLC_OPCODE_MOV << 6 | HLC_OPCODE_R0<<3 | HLC_OPCODE_R0)
#define TILE1_HLCINSTR_14	(1 << 11 | HLC_OPCODE_MOV << 6 | HLC_OPCODE_R0<<3 | HLC_OPCODE_R0)
#define TILE1_HLCINSTR_15	(1 << 11 | HLC_OPCODE_MOV << 6 | HLC_OPCODE_R0<<3 | HLC_OPCODE_R0)
#define TILE1_HLCINSTR_16	(0 << 11 | HLC_OPCODE_INTR << 6 | 2)
#define TILE1_HLCINSTR_17	(1 << 11 | HLC_OPCODE_MOV << 6 | HLC_OPCODE_R0<<3 | HLC_OPCODE_R0)
#define TILE1_HLCINSTR_18	(1 << 11 | HLC_OPCODE_MOV << 6 | HLC_OPCODE_R0<<3 | HLC_OPCODE_R0)
#define TILE1_HLCINSTR_19	(1 << 11 | HLC_OPCODE_MOV << 6 | HLC_OPCODE_R0<<3 | HLC_OPCODE_R0)
#define TILE1_HLCINSTR_20	(1 << 11 | HLC_OPCODE_MOV << 6 | HLC_OPCODE_R0<<3 | HLC_OPCODE_R0)
#define TILE1_HLCINSTR_21	(1 << 11 | HLC_OPCODE_MOV << 6 | HLC_OPCODE_R0<<3 | HLC_OPCODE_R0)
#define TILE1_HLCINSTR_22	(1 << 11 | HLC_OPCODE_MOV << 6 | HLC_OPCODE_R0<<3 | HLC_OPCODE_R0)
#define TILE1_HLCINSTR_23	(1 << 11 | HLC_OPCODE_MOV << 6 | HLC_OPCODE_R0<<3 | HLC_OPCODE_R0)
#define TILE1_HLCINSTR_24	(1 << 11 | HLC_OPCODE_MOV << 6 | HLC_OPCODE_R0<<3 | HLC_OPCODE_R0)
#define TILE1_HLCINSTR_25	(1 << 11 | HLC_OPCODE_MOV << 6 | HLC_OPCODE_R0<<3 | HLC_OPCODE_R0)
#define TILE1_HLCINSTR_26	(1 << 11 | HLC_OPCODE_MOV << 6 | HLC_OPCODE_R0<<3 | HLC_OPCODE_R0)
#define TILE1_HLCINSTR_27	(1 << 11 | HLC_OPCODE_MOV << 6 | HLC_OPCODE_R0<<3 | HLC_OPCODE_R0)
#define TILE1_HLCINSTR_28	(1 << 11 | HLC_OPCODE_MOV << 6 | HLC_OPCODE_R0<<3 | HLC_OPCODE_R0)
#define TILE1_HLCINSTR_29	(1 << 11 | HLC_OPCODE_MOV << 6 | HLC_OPCODE_R0<<3 | HLC_OPCODE_R0)
#define TILE1_HLCINSTR_30	(1 << 11 | HLC_OPCODE_MOV << 6 | HLC_OPCODE_R0<<3 | HLC_OPCODE_R0)
#define TILE1_HLCINSTR_31	(1 << 11 | HLC_OPCODE_MOV << 6 | HLC_OPCODE_R0<<3 | HLC_OPCODE_R0)




#define TILE1_OUTPUT_COND_CTR_0 0x0
#define TILE1_OUTPUT_COND_CTR_1 0x0
#define TILE1_OUTPUT_COND_CTR_2 0x0
#define TILE1_OUTPUT_COND_CTR_3 0x0
#define TILE1_OUTPUT_COND_CTR_4 0x0
#define TILE1_OUTPUT_COND_CTR_5 0x0
#define TILE1_OUTPUT_COND_CTR_6 0x0
#define TILE1_OUTPUT_COND_CTR_7 0x0

uint16_t TILE1HLCInstr[CLB_NUM_HLC_INSTR + 1] =
{
    TILE1_HLCINSTR_0,
    TILE1_HLCINSTR_1,
    TILE1_HLCINSTR_2,
    TILE1_HLCINSTR_3,
    TILE1_HLCINSTR_4,
    TILE1_HLCINSTR_5,
    TILE1_HLCINSTR_6,
    TILE1_HLCINSTR_7,
    TILE1_HLCINSTR_8,
    TILE1_HLCINSTR_9,
    TILE1_HLCINSTR_10,
    TILE1_HLCINSTR_11,
    TILE1_HLCINSTR_12,
    TILE1_HLCINSTR_13,
    TILE1_HLCINSTR_14,
    TILE1_HLCINSTR_15,
    TILE1_HLCINSTR_16,
    TILE1_HLCINSTR_17,
    TILE1_HLCINSTR_18,
    TILE1_HLCINSTR_19,
    TILE1_HLCINSTR_20,
    TILE1_HLCINSTR_21,
    TILE1_HLCINSTR_22,
    TILE1_HLCINSTR_23,
    TILE1_HLCINSTR_24,
    TILE1_HLCINSTR_25,
    TILE1_HLCINSTR_26,
    TILE1_HLCINSTR_27,
    TILE1_HLCINSTR_28,
    TILE1_HLCINSTR_29,
    TILE1_HLCINSTR_30,
    TILE1_HLCINSTR_31
};

/**************************************************************************************************
 * 
 * This function is MOSTLY copied from the TI driverlib example for FSI EPWM synchronization,
 * but it has some additions.
 * 
 *************************************************************************************************/
void clb_init_for_fsi_sync(uint16_t core, uint32_t delay)
{
    uint16_t i;
    uint16_t clk_cfg;
    uint32_t base;
    // Output LUT

    // Enable clock
    clk_cfg = SYSCTL_PERIPH_CLK_CLB1 | ((core - 1)<<8);
    SysCtl_enablePeripheral((SysCtl_PeripheralPCLOCKCR)clk_cfg);

    base = CLB1_BASE + ((core - 1U) * (CLB2_BASE - CLB1_BASE));

    CLB_enableCLB(base);

    CLB_configOutputLUT(base, CLB_OUT0, TILE1_CFG_OUTLUT_0);
    CLB_configOutputLUT(base, CLB_OUT1, TILE1_CFG_OUTLUT_1);
    CLB_configOutputLUT(base, CLB_OUT2, TILE1_CFG_OUTLUT_2);
    CLB_configOutputLUT(base, CLB_OUT3, TILE1_CFG_OUTLUT_3);
    CLB_configOutputLUT(base, CLB_OUT4, TILE1_CFG_OUTLUT_4);
    CLB_configOutputLUT(base, CLB_OUT5, TILE1_CFG_OUTLUT_5);
    CLB_configOutputLUT(base, CLB_OUT6, TILE1_CFG_OUTLUT_6);
    CLB_configOutputLUT(base, CLB_OUT7, TILE1_CFG_OUTLUT_7);
    // AOC
    CLB_configAOC(base, CLB_AOC0, TILE1_OUTPUT_COND_CTR_0);
    CLB_configAOC(base, CLB_AOC1, TILE1_OUTPUT_COND_CTR_1);
    CLB_configAOC(base, CLB_AOC2, TILE1_OUTPUT_COND_CTR_2);
    CLB_configAOC(base, CLB_AOC3, TILE1_OUTPUT_COND_CTR_3);
    CLB_configAOC(base, CLB_AOC4, TILE1_OUTPUT_COND_CTR_4);
    CLB_configAOC(base, CLB_AOC5, TILE1_OUTPUT_COND_CTR_5);
    CLB_configAOC(base, CLB_AOC6, TILE1_OUTPUT_COND_CTR_6);
    CLB_configAOC(base, CLB_AOC7, TILE1_OUTPUT_COND_CTR_7);

    // LUT4
    CLB_selectLUT4Inputs(base, TILE1_CFG_LUT4_IN0, TILE1_CFG_LUT4_IN1, TILE1_CFG_LUT4_IN2, TILE1_CFG_LUT4_IN3);
    CLB_configLUT4Function(base, TILE1_CFG_LUT4_FN10, TILE1_CFG_LUT4_FN2);

    // FSM
    CLB_selectFSMInputs(base, TILE1_CFG_FSM_EXT_IN0, TILE1_CFG_FSM_EXT_IN1, TILE1_CFG_FSM_EXTRA_IN0, TILE1_CFG_FSM_EXTRA_IN1);
    CLB_configFSMNextState(base, TILE1_CFG_FSM_NEXT_STATE_0, TILE1_CFG_FSM_NEXT_STATE_1, TILE1_CFG_FSM_NEXT_STATE_2);
    CLB_configFSMLUTFunction(base, TILE1_CFG_FSM_LUT_FN10, TILE1_CFG_FSM_LUT_FN2);

    // Counters
    CLB_selectCounterInputs(base, TILE1_CFG_COUNTER_RESET, TILE1_CFG_COUNTER_EVENT, TILE1_CFG_COUNTER_MODE_0, TILE1_CFG_COUNTER_MODE_1);
    CLB_configMiscCtrlModes(base, TILE1_CFG_MISC_CONTROL);
    CLB_configCounterLoadMatch(base, CLB_CTR0, TILE1_COUNTER_0_LOAD_VAL, delay, TILE1_COUNTER_0_MATCH2_VAL);
    CLB_configCounterLoadMatch(base, CLB_CTR1, TILE1_COUNTER_1_LOAD_VAL, TILE1_COUNTER_1_MATCH1_VAL, TILE1_COUNTER_1_MATCH2_VAL);
    CLB_configCounterLoadMatch(base, CLB_CTR2, TILE1_COUNTER_2_LOAD_VAL, TILE1_COUNTER_2_MATCH1_VAL, TILE1_COUNTER_2_MATCH2_VAL);
    CLB_configCounterTapSelects(base, TILE1_CFG_TAP_SEL);

    // HLC
    CLB_configHLCEventSelect(base, TILE1_HLC_EVENT_SEL);
    CLB_setHLCRegisters(base, TILE1_HLC_R0_INIT, TILE1_HLC_R1_INIT, TILE1_HLC_R2_INIT, TILE1_HLC_R3_INIT);

    for(i = 0; i <= CLB_NUM_HLC_INSTR; i++)
    {
        CLB_programHLCInstruction(base, i, TILE1HLCInstr[i]);
    }


    //
    // Select Global input instead of local input for all CLB IN
    //
    CLB_configLocalInputMux(base, CLB_IN0, CLB_LOCAL_IN_MUX_GLOBAL_IN);
    CLB_configLocalInputMux(base, CLB_IN1, CLB_LOCAL_IN_MUX_GLOBAL_IN);
    CLB_configLocalInputMux(base, CLB_IN2, CLB_LOCAL_IN_MUX_GLOBAL_IN);
    CLB_configLocalInputMux(base, CLB_IN3, CLB_LOCAL_IN_MUX_GLOBAL_IN);
    CLB_configLocalInputMux(base, CLB_IN4, CLB_LOCAL_IN_MUX_GLOBAL_IN);
    CLB_configLocalInputMux(base, CLB_IN5, CLB_LOCAL_IN_MUX_GLOBAL_IN);
    CLB_configLocalInputMux(base, CLB_IN6, CLB_LOCAL_IN_MUX_GLOBAL_IN);
    CLB_configLocalInputMux(base, CLB_IN7, CLB_LOCAL_IN_MUX_GLOBAL_IN);


    //
    // Select FSI_PING_PKT_RCVD for CLB1, IN0
    //
    CLB_configGlobalInputMux(base, CLB_IN0, CLB_GLOBAL_IN_MUX_FSIRXA_PING_PACKET_RCVD);

    //
    // Unused Inputs below
    //
    CLB_configGlobalInputMux(base, CLB_IN1, CLB_GLOBAL_IN_MUX_EPWM2A);
    CLB_configGlobalInputMux(base, CLB_IN2, CLB_GLOBAL_IN_MUX_EPWM2A);
    CLB_configGlobalInputMux(base, CLB_IN3, CLB_GLOBAL_IN_MUX_EPWM2A);
    CLB_configGlobalInputMux(base, CLB_IN4, CLB_GLOBAL_IN_MUX_EPWM2A);
    CLB_configGlobalInputMux(base, CLB_IN5, CLB_GLOBAL_IN_MUX_EPWM2A);
    CLB_configGlobalInputMux(base, CLB_IN6, CLB_GLOBAL_IN_MUX_EPWM2A);
    CLB_configGlobalInputMux(base, CLB_IN7, CLB_GLOBAL_IN_MUX_EPWM2A);

    //
    // Select External for CLB1, IN0
    //
    CLB_configGPInputMux(base, CLB_IN0, CLB_GP_IN_MUX_EXTERNAL);

    //
    // Unused inputs to GP register
    //
    CLB_configGPInputMux(base, CLB_IN1, CLB_GP_IN_MUX_GP_REG);
    CLB_configGPInputMux(base, CLB_IN2, CLB_GP_IN_MUX_GP_REG);
    CLB_configGPInputMux(base, CLB_IN3, CLB_GP_IN_MUX_GP_REG);
    CLB_configGPInputMux(base, CLB_IN4, CLB_GP_IN_MUX_GP_REG);
    CLB_configGPInputMux(base, CLB_IN5, CLB_GP_IN_MUX_GP_REG);
    CLB_configGPInputMux(base, CLB_IN6, CLB_GP_IN_MUX_GP_REG);
    CLB_configGPInputMux(base, CLB_IN7, CLB_GP_IN_MUX_GP_REG);

    /** Enable CLB outputs 12 and 13 (which corresponds to outputs 4 and 5).
     * 
     * Output 4 (12) is the delayed ping output
     * Output 5 (13) is the ping without delay
     * 
     * */
    CLB_setOutputMask(base, CLB_OUTPUT_12 | CLB_OUTPUT_13, true);
}

