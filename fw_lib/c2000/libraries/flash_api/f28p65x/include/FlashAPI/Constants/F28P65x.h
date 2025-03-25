/*--------------------------------------------------------*/
/* F65\Constants\F28P65x.h                                */
/*                                                        */
/* Copyright (c) 2022-2023 Texas Instruments Incorporated */
/*                                                        */
/*--------------------------------------------------------*/

/*!
    \file F65\Constants\F28P65x.h
    \brief A set of Constant Values for the F28P65x Family.
*/
#ifndef CONSTANTS_F28P65x_H_
#define CONSTANTS_F28P65x_H_
//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

/*!
 *  FMC memory map defines
 */
#if defined (_F28P65x) 
	#define FLASH_MAP_BEGIN            (uint32)(0x80000)
	#define FLASH_MAP_END              (uint32)(0x11FFFF)
	#define OTP_MAP_BEGIN              (uint32)(0x78000) //Customer OTP start
	#define OTP_MAP_END                (uint32)(0x7A3FF) //Customer OTP End
    #define BANK0_OTP_START            (uint32)(0x78000)
    #define BANK0_OTP_END              (uint32)(0x783FF)
    #define BANK1_OTP_START            (uint32)(0x78800)
    #define BANK1_OTP_END              (uint32)(0x78BFF)
    #define BANK2_OTP_START            (uint32)(0x79000)
    #define BANK2_OTP_END              (uint32)(0x793FF)
    #define BANK3_OTP_START            (uint32)(0x79800)
    #define BANK3_OTP_END              (uint32)(0x79BFF)
    #define BANK4_OTP_START            (uint32)(0x7A000)
    #define BANK4_OTP_END              (uint32)(0x7A3FF)
	#define OTPECC_MAP_BEGIN           (uint32)(0x1071000)
	#define OTPECC_MAP_END             (uint32)(0x107127F)
    #define OTPECC_MAP_END_PLUS1       (uint32)(0x1071280)
    #define BANK0_OTPECC_START         (uint32)(0x1071000)
    #define BANK0_OTPECC_END_PLUS1     (uint32)(0x1071080)
    #define BANK1_OTPECC_START         (uint32)(0x1071080)
    #define BANK1_OTPECC_END_PLUS1     (uint32)(0x1071100)
    #define BANK2_OTPECC_START         (uint32)(0x1071100)
    #define BANK2_OTPECC_END_PLUS1     (uint32)(0x1071180)
    #define BANK3_OTPECC_START         (uint32)(0x1071180)
    #define BANK3_OTPECC_END_PLUS1     (uint32)(0x1071200)
    #define BANK4_OTPECC_START         (uint32)(0x1071200)
    #define BANK4_OTPECC_END_PLUS1     (uint32)(0x1071280)
	#define FLASHECC_MAP_BEGIN         (uint32)(0x1080000)
	#define FLASHECC_MAP_END           (uint32)(0x1093FFF)
    #define FLASHECC_MAP_END_PLUS1     (uint32)(0x1094000)
    #define BANK0_START                (uint32)(0x80000)
    #define BANK4_END_PLUS1            (uint32)(0x120000)
    #define BANK0_DCSMOTP_START        (uint32)(0x78000)
    #define BANK4_DCSMOTP_END_PLUS1    (uint32)(0x7A400)
	
#endif



//Flash wrapper base address
#define  FLASH_WRAPPER_PROGRAM_BASE         0x57000U /*(FLASH0CMD_BASE - 0x1000UL)*/

/*!
    \brief Define to map the direct access to the FMC registers.
*/
#define FlashTech_CPU0_BASE_ADDRESS ((Fapi_FmcRegistersType *)FLASH_WRAPPER_PROGRAM_BASE)


//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* CONSTANTS_F28P65x_H_ */
