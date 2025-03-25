# Linker command file

*Marko Gulin, 17 Feb 2021*

This note explains how linker command file is designed, which tells the linker how to place the software into the hardware. See [spru513](docs/spru513v.pdf "TMS320C28x Assembly Language Tools") for more information.

There are two types of user programs that run on a single microcontroller: (i) bootloader and (ii) module firmware. A single linker script file is used for both programs, in which user-defined macro symbols control how memory is organized for each program.

Programs and constants are saved to a non-volatile memory (Flash), although some parts could be copied in runtime to a volatile memory (RAM) for faster execution. For example, functions with *.TI.ramfunc*, *ramfunc*, or *ramfuncs* attribute are placed in Flash but run from RAM.

## Boot ROM bootloader

When microcontroller is powered, the factory-programmed bootloader in read-only boot ROM runs first. Boot-mode signals (general purpose I/Os) are used to tell the bootloader which boot mode to use. When configured for Boot-to-Flash, the bootloader gets program start address from **0x3F7FF6** (f2803x) or **0x080000** (f28004x). This is denoted as *CODESTART* section in the linker file. See [spru722](docs/spru722c.pdf "TMS320x280x, 2801x, 2804x Boot ROM") for more information.

## User bootloader

User bootloader is a small program that is used to program Flash on the microcontroller over CAN. This is convenient as no JTAG probe is required to flash the microcontroller, which means that customers can update firmware without additional equipment.

### Run program from RAM

Since bootloader should be able to update itself, the entire bootloader program must run from RAM. This also includes constants in the *.econst* section, which are initially mapped to Flash. A *--ramfunc=on* compiler directive is used to specify that all program functions are to be placed in the *.TI.ramfunc* section, unless some other section is explicitly set for a particular function.

All sections that should be copied from Flash to RAM should have both *LOAD* and *RUN* memory sections specified. The *TABLE(BINIT)* directive adds entry in the boot initialization table which contains information such as load start address and length, and run start address. Before reaching *main()*, program first runs *_c_int00()* which copies data from Flash to RAM as specified by initialization tables in *.cinit* and *.binit* sections. The *_c_int00()* is automatically inserted by the TI C2000 compiler. See [spru514](docs/spru514v.pdf "TMS320C28x Optimizing C/C++ Compiler") for more information.

### Disable the watchdog

Since watchdog is enabled by default, it could happen that watchdog timer expires before all data is copied from Flash to RAM in *_c_int00()*, which will prevent microcontroller from starting. The watchdog should be disabled before reaching *_c_int00()* which is done in the *codestartbranch.asm* routine from the code library.

### Possible pitfails

A small part of the program is not copied from Flash to RAM, which includes *.text*, *.binit*, and *.cinit* sections. The *.cinit* and *.binit* sections contain initialization tables that are used only in *_c_int00()*, and they are not needed once program reaches *main()*. However, if compiler decides to use some routine from the RTS library, there is a big chance it will place it to the *.text* section by default. This will lead to a critical failure when bootloader updates itself, since before uploading new bootloader program it has to delete the existing one, which includes the routine from the *.text* section it needs. Always check the generated *.map* file to see how program is linked.

### Flash API

The Flash API is a pre-compiled library that provides interface to Flash programming. It is used only in user bootloader program.

### Memory distribution
When compiling the bootloader, the program and data are distributed like this:

- F2803X devices:
1. Program instructions (.text, etc) are stored in FLASH A and B sectors.
2. FLASH G and H are reserved for shadow bootloader.
3. Program stack and data (.stack, .ebss, etc) is stored in RAM M0 and RAM M1
4. Program instructions, although stored in flash, are copied and run in RAM L0 to RAM L3.

- F28004X devices:
1. Program instructions (.text, etc) are stored in FLASH 0 to 2 sectors (3 sectors in total)
2. FLASH 3 to 6 are reserved for shadow bootloader.
3. Program stack and data (.stack, .ebss, etc) is stored in RAM M0 and RAM M1
4. Program instructions, although stored in flash, are copied and run in RAM LS0 to RAM LS7.


## Main application firmware
The module firmware is the main application firmware that will run in the module. The user bootloader will automatically jump to this main application firmware if it is detected.

To detect if there is a main application firmware stored in the flash, the user bootloader will look for the "BENTRY" symbol in a specific place, which is chosen by convention.

### The 'BENTRY' symbol
The BENTRY symbol is actually a small struct (piece of information) which contains a 'key' and a program pointer to the 'c_int00' function, which is the main initialization function from the RTS library. Therefore, the user bootloader must look for this BENTRY symbol, verify the 'key', and then just jump to the program pointer specified. This will run the main application firmware.

### Memory distribution
When compiling the main application firmware, the program and data are distributed like this:

- F2803X devices:
1. The BENTRY instruction will be located at 0x3F3FF8, which is the end of FLASH C.
2. Program instructions (.text, etc) are stored in FLASH C to F sectors (4 sectors in total).
3. Program stack (.stack) is stored in RAM M0 and RAM M1
4. Program data (.ebss, etc) is stored in RAM L0 to RAM L2
5. Some program instructions (such as those executed in ISR) are stored in flash but executed in RAM, and take RAM L3.

- F28004X devices:
1. The BENTRY instruction will be located at 0x08FFF8, which is the end of FLASH 15.
2. Program instructions (.text, etc) are stored in FLASH 6 to 15 sectors (10 sectors in total).
3. Program stack (.stack) is stored in RAM M0 and RAM M1
4. Program data (.ebss, etc) is stored in RAM L4 to RAM L7
5. Some program instructions (such as those executed in ISR) are stored in flash but executed in RAM, and take RAM L0 to RAM L3.
