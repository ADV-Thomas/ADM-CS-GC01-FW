# C2000 drivers and libraries

*Marko Gulin, 19 Feb 2021*

This note explains how driver and library files are organized. Most of the files are taken from **C2000Ware** v3.04, which replaced *controlSUITE* set of software, tools and documentation. Follow [this link](https://www.ti.com/tool/C2000WARE "Visit web site") for more information.

Advantics power converters are based on 32-bit microcontrollers from *C2000 Piccolo* family: [TMS320F28035](https://www.ti.com/product/TMS320F28035 "Visit web site") and [TMS320F280041](https://www.ti.com/product/TMS320F280041 "Visit web site"). The former is an older microcontroller without floating-point unit (FPU), i.e., the latter is much more powerful in every respect.

## Driverlib

The *driverlib* directory contains basic drivers which rarely include anything more complex than manipulation with registers on a bit level. However, it is a big step forward compared to *device_support* set of drivers from *controlSUITE*, and is recommended by TI for new designs and new devices.

Drivers for f2803x are custom-designed following the same design principles from f28004x driverlib. The f2803x driverlib is not complete, *i.e.*, it includes only what is necessary for Advantics power converters. Texas Instruments does not plan to provide driverlib for their older microcontrollers. Follow [this link](https://e2e.ti.com/support/microcontrollers/c2000/f/171/t/978800 "Visit web site") for more information.

## Library

The *library* directory contains pre-compiled library files, such as Flash API and IQMath libraries. Each microcontroller type has its own version of library files. The library files are included directly in the linker script file.