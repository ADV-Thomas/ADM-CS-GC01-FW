# FW_LIB codebase

*Jorge Sola, 11 July 2022*

This note explains the idea behind the FW_LIB codebase. 

In Advantics we have several modules which share many things in common, such as microcontroller (either F280041 or F28035), main LED, common CAN interface, external EEPROM with I2C communcation, etc. Furthermore, these modules will also perform similar things (such as initialization routines, PID controllers, filters, etc.). Therefore, it makes sense to create a common library of code that is shared between the different firmwares for two reasons:
- To abstract away the application logic from the actual hardware implementation (i.e, in the main application, we may want to just 'initialize' the module, but it doesn't care how this initialization is done).
- To not repeat the code, so that it is more easily mantainable. 

The FW_LIB codebase was massively changed in 2021 when Marko created an essentially better codebase that better abstracted all the common elements of the advantics boards and CPUs. This note will therefore explain the structure of the new codebase.

## /build
This folder contains the common makefile and .cmake files that are shared by all firmwares for the compilation, as well as a small python program to generate a timestamp during compilation.

## /c2000
This folder contains some documentation about the C2000 family of microcontrollers, as well as some libraries that are needed, such as the IQ math library, the Flash libraries and the driverlib libraries. The driverlib library is a set of C and header files provided by Texas Instruments to directly access the peripheral registers. They are low level functions that directly modify the peripheral bits to configure and operate them. This library is provided only for the F28004X family and not for the F2803X family, so in order to have portability, a 'clone' of the driverlib library was manually created for the F2803X family.

## /code
This folder contains the actual code writen by us, which is subdivided into more subfolders.

### /code/src/drv folder
The /drv folder contains the functions that are used to configure the peripherals. These functions usually take a struct with some generic information of the peripheral to be configured, and then applies some logic and calls the 'driverlib' functions to configure the peripheral. 

For example, the application may want to generate a PWM in GPIO 0 with a 10 KHz frequency, so this information will be passed in a struct to the pwm.c module in the /drv folder, and then the PWM will be properly initialized and configured for this.

*Note:* certain peripherals (such as CAN or Flash) have a totally different implementation in the F2803X and F28004X families, so they have separate files, instead of just a conditional compilation switch.

###  /code/src/hal folder
The /hal folder contains a set of functions a variables that are used to create a Hardware Abstraction Layer (HAL), and a Hardware Application Interface (HAPI).

- The HAL includes all the initialization functions that are common for all firmwares (including the bootloader). This includes: initialization of flash, CPU clock, default state of GPIOs, CAN, I2C, EEPROM, and main LED.

- The HAPI contains wrapper functions to call dynamically allocated generic functions (such as 'interlock trip', or 'ISR run', etc.). The implementation of these functions is hardware dependant, and hence, their code needs to be abstracted away. These functions are just used in the main application firmware (and not the bootloader). Therefore, the HAPI is not initialized in the bootloader. 

###  /code/src/api folder
The /api folder contains application firmware that is common to all modules, but that is not hardware related. For example, the '/api/adc.c' module will have an 'initialization' routine and a 'run' routine that is common to all modules, where it will iterate through all the specified ADC variables and read their value, and perform some normalization and filtering on them.

###  /code/src/lib folder
The /lib folder contains modules that are commonly used as tools for many tasks, such as filters, PID controllers, CRC calculation, etc. 

###  /code/src/boot folder
Contains the 'bentry' symbol and function, as well as the 'codestart' assembly function, and the 'boot' assembly function.

###  /code/src/net folder
Contains an abstraction for the communication protocols with functions such as "X_open", "X_write", "X_read", etc., where X is the communication protocol (such as I2C, SPI, CAN, or even higher level protocols such as EEPROM, flash, etc.)). 

### /code/inc
The /inc folder contains all the header files for the .c files

### /code/cgen
The /cgen folder contains header files with macros, that are used essentially for defining in the main application the different variables that are needed. This is to enforce a common and simple semantic.

## /link
Contains the linker files, which describe how all the program memory is distributed inside the DSP memory.

## /toolchain
Contains the compiler binary and its installer, so that it can be automatically installed in the remote server with the appveyor file recipe.

# How to use FW_LIB
FW_LIB is a git submodule which can therefore be treated as an independent repo. Thus, a firmware should have inside it this submodule.

The main application firmware should have a 'hapi.c' file and a 'hapi_rev_X.c', which concentrate just all the hardware dependent variables and logic for a given module firmware and revision. In this high level hapi files, things like the low level pwm variables are defined. These variables are defined by using the /cgen macros, and these variables are created with a leading underscore (i.e, '_pwm'). Also, common hardware access functions such as ADC read and PWM update functions are defined in this file. The 'hapi' files therefore serves as a "glue" between the application high level variables, and the hardware low level variables. 

Let's put an example. In a module, we may want to sample the current of an inductor. For that application let's assume we want to call it "Current_phase_A". The application does not care about anything else than that. Then, in the main application firmware we will have a file called 'adc.c' that defines (by again, using the /cgen macros), a generic 'adc' struct with one 'adc_var' variable (it may be more). In the hapi.c file, we need to create also a variable '_adc_var' which defines its low level properties (hardware dependent, such as the analog chanel to be sampled). Then, in the '_hapi_adc_read' function, both high and low level variables will be 'glued' toguether.

Like that, any place in the application firmware code where the information of the phase current is needed, no hardware implementation details are shared, and code is much more abstracted. 




