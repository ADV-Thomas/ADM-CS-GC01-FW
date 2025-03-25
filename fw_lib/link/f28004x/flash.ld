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
        CODESTART       : origin = 0x080000, length = 0x000002      /* Part of FLASH0   */

        /* Program data */
        FLASH_PROG      : origin = 0x080002, length = 0x002FFE      /* FLASH0..2        */

        /* Flash reserved memory for module info */
        FLASH_MOD_CONFIG: origin = 0x084000, length = 0x001000      /* FLASH4        */

        RAM_FUNC        : origin = 0x008000, length = 0x004000      /* RAMLS0..7        */
#else
        /* Application program entry point */
        BENTRY          : origin = 0x08FFF8, length = 0x000008      /* Part of FLASH15  */

        /* Program data */
        FLASH_PROG      : origin = 0x085000, length = 0x00AFF8      /* FLASH6..15       */
        RAM_FUNC        : origin = 0x008000, length = 0x002000      /* RAMLS0..3        */
#endif

        RESET           : origin = 0x3FFFC0, length = 0x000002

    PAGE 1: /* DATA SPACE */

        /* NOTE: according to user manual, some space in RAM M0 must be reserved for 
        Boot ROM stack and variables, however, this location can be overwritten by
        application if jump to flash is used, according to this E2E forum thread:
        https://e2e.ti.com/support/microcontrollers/c2000-microcontrollers-group/c2000/f/c2000-microcontrollers-forum/1115563/tms320f280041-can-bootrom-reserved-m0-ram-be-reused-by-application-if-booting-to-flash/4135880#4135880
        
        If something starts misbehaving consider changing this back to reserve 
        memory for the Boot ROM, although it should not be needed */

        FLASH_API_RSVD  : origin = 0x000760, length = 0x000020
        
#ifdef BOOTLOADER
        /* Volatile data */
        RAM_STACK       : origin = 0x000000, length = 0x000760      /* RAMM0            */
        RAM_DATA        : origin = 0x00C000, length = 0x008000      /* RAMM0..1         */
#else
        /* Volatile data */
        RAM_STACK       : origin = 0x000000, length = 0x000760      /* RAMM0..1         */
        RAM_DATA        : origin = 0x00A000, length = 0x002000      /* RAMLS4..7        */
        RAM_DLOG        : origin = 0x00C000, length = 0x008000      /* RAMGS0..3        */
#endif
}

SECTIONS
{
#ifdef BOOTLOADER
    codestart   : >CODESTART,   PAGE = 0, ALIGN(4)
#else
    bentry      : >BENTRY,      PAGE = 0, ALIGN(4)
#endif

    .text       : >FLASH_PROG,  PAGE = 0, ALIGN(4)
    .cinit      : >FLASH_PROG,  PAGE = 0, ALIGN(4)
    .binit      : >FLASH_PROG,  PAGE = 0, ALIGN(4)
    .switch     : >FLASH_PROG,  PAGE = 0, ALIGN(4)

#ifdef BOOTLOADER
    GROUP
    {
        .econst
    }   LOAD = FLASH_PROG, PAGE = 0,
        RUN = RAM_DATA, PAGE = 1,
        TABLE(BINIT), ALIGN(4)
#else
    .econst     : >FLASH_PROG,  PAGE = 0, ALIGN(4)
    .dlog_data  : >RAM_DLOG,    PAGE = 1
#endif

    .reset      : >RESET,       PAGE = 0, TYPE = DSECT

    .stack      : >RAM_STACK,   PAGE = 1
    .ebss       : >RAM_DATA,    PAGE = 1
    
    GROUP
    {
        .TI.ramfunc
    }   LOAD = FLASH_PROG, PAGE = 0,
        RUN = RAM_FUNC, PAGE = 0,
        TABLE(BINIT), ALIGN(4)
}


-l F021_ROM_API_F28004x_FPU32.lib
-l rts2800_fpu32.lib




