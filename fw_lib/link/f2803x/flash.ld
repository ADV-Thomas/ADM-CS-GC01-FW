/**
 * FILE:    flash.ld
 * ABOUT:   Linker command file
 * AUTHOR:  Marko Gulin (C) 2021
 */

MEMORY
{
    PAGE 0: /* PROGRAM SPACE */

#ifdef BOOTLOADER
        /* Boot-to-Flash program entry point */
        CODESTART       : origin = 0x3F7FF6, length = 0x000002      /* Part of FLASHA   */

        /* Code security module */
        CSM_RSVD        : origin = 0x3F7F80, length = 0x000076      /* Part of FLASHA   */
        CSM_PWL_P0      : origin = 0x3F7FF8, length = 0x000008      /* Part of FLASHA   */


        /* Program data */
        FLASH_PROG      : origin = 0x3F4000, length = 0x003F80      /* FLASHA..B        */
        FLASH_SHADOW    : origin = 0x3E8000, length = 0x004000      /* FLASHG..H        */
        RAM_FUNC        : origin = 0x008000, length = 0x002000      /* RAML0..3         */
#else
        /* Application program entry point */
        BENTRY          : origin = 0x3F3FF8, length = 0x000008      /* End of FLASHC   */

        /* Program data */
        FLASH_PROG      : origin = 0x3EC000, length = 0x007FF8      /* FLASHC..F        */
        RAM_FUNC        : origin = 0x009400, length = 0x000C00      /* RAML3            */

        /* IQ Math tables in Boot ROM */
        IQTABLES        : origin = 0x3FE000, length = 0x000B50
        IQTABLES2       : origin = 0x3FEB50, length = 0x00008C
        IQTABLES3       : origin = 0x3FEBDC, length = 0x0000AA
#endif

        RESET           : origin = 0x3FFFC0, length = 0x000002

    PAGE 1: /* DATA SPACE */
    
        /* NOTE: according to user manual, some space in RAM M0 must be reserved for 
        Boot ROM stack and variables, however, this location can be overwritten by
        application if jump to flash is used, according to this E2E forum thread:
        https://e2e.ti.com/support/microcontrollers/c2000-microcontrollers-group/c2000/f/c2000-microcontrollers-forum/1115563/tms320f280041-can-bootrom-reserved-m0-ram-be-reused-by-application-if-booting-to-flash/4135880#4135880 
        
        If something starts misbehaving consider changing this back to reserve 
        memory for the Boot ROM, although it should not be needed */

#ifdef BOOTLOADER
        /* Volatile data */
        RAM_STACK       : origin = 0x000000, length = 0x000200      /* RAMM0            */
        RAM_DATA        : origin = 0x000200, length = 0x000600      /* RAMM0..1         */
        FLASH_API       : origin = 0x000D02, length = 0x000004
#else
        /* Volatile data */
        RAM_STACK       : origin = 0x000000, length = 0x000800      /* RAMM0..1         */
        RAM_DATA        : origin = 0x008000, length = 0x001400      /* RAML0..2         */
#endif
}

SECTIONS
{
#ifdef BOOTLOADER
    codestart   : >CODESTART,   PAGE = 0
#else
    bentry      : >BENTRY,      PAGE = 0, ALIGN(4)
#endif

    .text       : >FLASH_PROG,  PAGE = 0, ALIGN(4)
    .cinit      : >FLASH_PROG,  PAGE = 0, ALIGN(4)
    .binit      : >FLASH_PROG,  PAGE = 0, ALIGN(4)

#ifdef BOOTLOADER
    GROUP
    {
        .econst
    }   LOAD = FLASH_PROG, PAGE = 0,
        RUN = RAM_DATA, PAGE = 1,
        TABLE(BINIT), ALIGN(4)
#else
    .econst     : >FLASH_PROG,  PAGE = 0, ALIGN(4)
#endif

    .reset      : >RESET,       PAGE = 0, TYPE = DSECT

    .stack      : >RAM_STACK,   PAGE = 1
    .ebss       : >RAM_DATA,    PAGE = 1
    
#ifdef BOOTLOADER
    GROUP
    {
        FlashCallbackVar
        FlashScalingVar
    }   RUN = FLASH_API, PAGE = 1
#else
    IQmathTables    : >IQTABLES,   PAGE = 0, TYPE = NOLOAD
    
    /* The following section is only linked if we are linking for the Filter
    module (adm-pc-lf45), because it is the only one using the IQmath exponential
    function. Otherwise, the linking would throw an error. This flag is conditionally
    defined in the 'toolchain_c2000.cmake' file */
    #ifdef USE_IQ_EXP
    IQmathTables2   : >IQTABLES2,  PAGE = 0, TYPE = NOLOAD
    {
        -l IQmath_coff.lib<IQNexpTable.obj> (IQmathTablesRam)
    }
    #endif

#if 0
    /**
     * LF45 module does not use these lookup tables, which makes linker to produce warning. This
     * would not have been a problem except that in Appveyor this warning is somehow reported as
     * an error! TODO: Fix this if necessary..
     */
    IQmathTables3   : >IQTABLES3,  PAGE = 0, TYPE = NOLOAD
    {
        -l IQmath_coff.lib<IQNasinTable.obj> (IQmathTablesRam)
    }
#endif
#endif

    GROUP
    {
        ramfuncs
        .TI.ramfunc
#ifndef BOOTLOADER
        IQmath
#endif
    }   LOAD = FLASH_PROG, PAGE = 0,
        RUN = RAM_FUNC, PAGE = 0,
        TABLE(BINIT), ALIGN(4)
}

#ifdef BOOTLOADER
-l 2803x_FlashAPI_BootROMSymbols.lib
#else
-l IQmath.lib
#endif
