# Advantics firmware project template

## Getting started

This template is written for Advantics power converters. The application example is a simple blinky LED that runs on ADM-PC-BP25 module. Please note that application programs cannot run without bootloader.

## Prerequisites

Choose a good text editor. [**Visual Studio Code**](https://code.visualstudio.com/Download) is strongly recommended. If you decide to use VSCode, you should also install [**MSVC compiler**](https://visualstudio.microsoft.com/vs/features/cplusplus/) for *IntelliSense*. Alternatively, you could use some other compiler for *IntelliSense* such as GCC, but they are known to have some issues (e.g. macro expansion).

Install [**GNU Make**](http://mingw-w64.org/doku.php/download) and [**CMake**](https://cmake.org/download/) (version 3.13.1 or greater) and make sure they are added to OS path.

Install [**C2000 C/C++ compiler**](https://www.ti.com/tool/C2000-CGT#downloads) (version 18.12.1.LTS) into the following directory:

    C:\ti\ti-cgt-c2000_18.12.1.LTS on Windows
    /opt/ti/ti-cgt-c2000_18.12.1.LTS on Linux

Install [**Python 3**](https://www.python.org/downloads/) and make sure it is added to OS path.

Once Python (with pip) has been installed, install the internal ADVANTICS tool to process CAN database:

    pip3 uninstall acgu-gen
    pip3 install git+ssh://git@github.com/ADVANTICS/acgu-gen.git@2024.4.10#egg=acgu-gen

You should now be able to build the project. Open Terminal in project root and type:

    make clean
    make all

If it does not work with `make`, try with `mingw32-make` instead.

## VSCode configuration

Firmware is written for 2 different CPU types:

- TMS320F2803X used for LF45, LL25, BC25 and UP25 modules, and
- TMS320F28004X used for BP25 and BI25 modules.

The VSCode is configured for f28004x. If you are writing a project for f2803x, make sure to update the `.vscode/c_cpp_properties.json` file:

    ${workspaceFolder}/fw_lib/c2000/driverlib/f28004x
    F28004X

Also, do not forget to update the compiler path for *IntelliSense*.