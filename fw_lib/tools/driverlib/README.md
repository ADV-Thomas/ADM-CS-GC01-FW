# Driverlib development tools

These tools are used to double-check custom-designed driverlib files (drivers) in

```
c2000/driverlib/f2803x/driverlib
c2000/driverlib/f2803x/driverlib/inc
code/inc/drv/f2803x
code/inc/drv/f28004x
```

The main motivation behind these tools is that it is very easy to make mistakes when writing shift and mask codes for register fields, not to mention more complicated codes that embed several variables.

## inc_hw.py

Program takes module name as input argument, for example:

```
py inc_hw.py sysctl
```

Program reads `inc/hw_memmap.h` header file to get the base register address, after which it reads the corresponding module header file in the `inc` directory. It checks all constants and outputs module registers in a form that can be directly compared against the datasheet, for example:

```
PLLSTS @ 0x7011 (1) EALLOW
15    NORMRDYE
14:9  Reserved
8:7   DIVSEL
6     MCLKOFF
5     OSCOFF
4     MCLKCLR
3     MCLKSTS
2     PLLOFF
1     Reserved
0     PLLLOCKS
```

## periph_clk.py

Program does not take input arguments, for example:

```
py periph_clk.py
```

Program reads `sysctl.h` header file and parses `enum SysCtl_PeripheralPCLOCKCR` items. It checks all constants and outputs registers peripheral clock enable registers `PCLKCR0..3` in a form that can be directly compared against the datasheet, for example:

```
PCLKCR0 @ 0x701C (1)
15    Reserved
14    ECANA
13:11 Reserved
10    SCIA
9     SPIB
8     SPIA
7:5   Reserved
4     I2CA
3     ADC
2     TBCLKSYNC
1     LINA
0     HRPWM
```

## pin_map.py

Program takes CPU type as input argument, for example:

```
py pin_map.py f2803x
py pin_map.py f28004x
```

Program parses `io.h` from `inc/drv` and `inc/drv/{cpu}`, `hw_memmap.h` and `hw_gpio.h` from `driverlib/inc`, and `pin_map.h` from `driverlib`, where `${cpu}` is either `f2803x` or `f28004x`. The CPU-specific `io.h` extends the `pin_map.h` from `driverlib` by including function code defined in `io_fun` enumeration in `io.h`. The program checks if `io.h` is consistent with `pin_map.h` and if all function codes have been properly defined.

Program outputs GPIO mux table in a form that can be directly compared against the datasheet, for example:

```
GPAMUX1 @ 0x6F86 (2)
(G)MUX: 0                 1                 2                 3                 
1:0     GPIO0             EPWM1_A           Reserved          Reserved          
3:2     GPIO1             EPWM1_B           Reserved          COMP1_OUT         
5:4     GPIO2             EPWM2_A           Reserved          Reserved          
7:6     GPIO3             EPWM2_B           SPIA_SOMI         COMP2_OUT         
9:8     GPIO4             EPWM3_A           Reserved          Reserved          
11:10   GPIO5             EPWM3_B           SPIA_SIMO         ECAP1             
13:12   GPIO6             EPWM4_A           SYNCIN            SYNCOUT           
15:14   GPIO7             EPWM4_B           SCIA_RX           Reserved          
17:16   GPIO8             EPWM5_A           Reserved          ADCSOCAO          
19:18   GPIO9             EPWM5_B           LINA_TX           HRCAP1            
21:20   GPIO10            EPWM6_A           Reserved          ADCSOCBO          
23:22   GPIO11            EPWM6_B           LINA_RX           HRCAP2            
25:24   GPIO12            TZ1               SCIA_TX           SPIB_SIMO         
27:26   GPIO13            TZ2               Reserved          SPIB_SOMI         
29:28   GPIO14            TZ3               LINA_TX           SPIB_CLK          
31:30   GPIO15            TZ1               LINA_RX           SPIB_STE          
```