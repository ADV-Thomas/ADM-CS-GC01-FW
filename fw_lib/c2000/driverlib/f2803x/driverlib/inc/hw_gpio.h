/**************************************************************************************************
 * 
 * \file hw_gpio.h
 * 
 * \brief Definitions for GPIO registers
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef HW_GPIO_H
#define HW_GPIO_H

/**************************************************************************************************
 * 
 * The following are defines for GPIO control registers (Sec. 1.4.2).
 * BASE REGISTER    GPIOCTRL_BASE
 * 
 *************************************************************************************************/

#define GPIO_O_GPACTRL          0x0U    /* GPIO A Control Register (GPIO0..31)              *E* */
#define GPIO_O_GPAQSEL1         0x2U    /* GPIO A Qualifier Select 1 Register (GPIO0..15)   *E* */
#define GPIO_O_GPAQSEL2         0x4U    /* GPIO A Qualifier Select 2 Register (GPIO16..31)  *E* */
#define GPIO_O_GPAMUX1          0x6U    /* GPIO A MUX 1 Register (GPIO0..15)                *E* */
#define GPIO_O_GPAMUX2          0x8U    /* GPIO A MUX 2 Register (GPIO16..31)               *E* */
#define GPIO_O_GPADIR           0xAU    /* GPIO A Direction Register (GPIO0..31)            *E* */
#define GPIO_O_GPAPUD           0xCU    /* GPIO A Pull Up Disable Register (GPIO0..31)      *E* */
#define GPIO_O_GPBCTRL          0x10U   /* GPIO B Control Register (GPIO32..44)             *E* */
#define GPIO_O_GPBQSEL1         0x12U   /* GPIO B Qualifier Select 1 Register (GPIO32..44)  *E* */
#define GPIO_O_GPBMUX1          0x16U   /* GPIO B MUX 1 Register (GPIO32..44)               *E* */
#define GPIO_O_GPBDIR           0x1AU   /* GPIO B Direction Register (GPIO32..44)           *E* */
#define GPIO_O_GPBPUD           0x1CU   /* GPIO B Pull Up Disable Register (GPIO32..44)     *E* */
#define GPIO_O_AIOMUX1          0x36U   /* Analog I/O MUX1 Register (AIO0..15)              *E* */
#define GPIO_O_AIODIR           0x3AU   /* Analog I/O Direction Register (AIO0..15)         *E* */

#define GPIO_S_GPACTRL          2
#define GPIO_S_GPAQSEL1         2
#define GPIO_S_GPAQSEL2         2
#define GPIO_S_GPAMUX1          2
#define GPIO_S_GPAMUX2          2
#define GPIO_S_GPADIR           2
#define GPIO_S_GPAPUD           2
#define GPIO_S_GPBCTRL          2
#define GPIO_S_GPBQSEL1         2
#define GPIO_S_GPBMUX1          2
#define GPIO_S_GPBDIR           2
#define GPIO_S_GPBPUD           2
#define GPIO_S_AIOMUX1          2
#define GPIO_S_AIODIR           2

/**************************************************************************************************
 * 
 * The following are defines for GPIO data registers (Sec. 1.4.3).
 * BASE REGISTER    GPIODATA_BASE
 * 
 *************************************************************************************************/

#define GPIO_O_GPADAT           0x0U    /* GPIO A Data Register (GPIO0-GPIO31)                  */
#define GPIO_O_GPASET           0x2U    /* GPIO A Set Register (GPIO0-GPIO31)                   */
#define GPIO_O_GPACLEAR         0x4U    /* GPIO A Clear Register (GPIO0-GPIO31)                 */
#define GPIO_O_GPATOGGLE        0x6U    /* GPIO A Toggle Register (GPIO0-GPIO31)                */
#define GPIO_O_GPBDAT           0x8U    /* GPIO B Data Register (GPIO32-GPIO44)                 */
#define GPIO_O_GPBSET           0xAU    /* GPIO B Set Register (GPIO32-GPIO44)                  */
#define GPIO_O_GPBCLEAR         0xCU    /* GPIO B Clear Register (GPIO32-GPIO44)                */
#define GPIO_O_GPBTOGGLE        0xEU    /* GPIO B Toggle Register (GPIO32-GPIO44)               */
#define GPIO_O_AIODAT           0x18U   /* Analog I/O Data Register (AIO0-AIO15)                */
#define GPIO_O_AIOSET           0x1AU   /* Analog I/O Data Set Register (AIO0-AIO15)            */
#define GPIO_O_AIOCLEAR         0x1CU   /* Analog I/O Clear Register (AIO0-AIO15)               */
#define GPIO_O_AIOTOGGLE        0x1EU   /* Analog I/O Toggle Register (AIO0-AIO15)              */

#define GPIO_S_GPADAT           2
#define GPIO_S_GPASET           2
#define GPIO_S_GPACLEAR         2
#define GPIO_S_GPATOGGLE        2
#define GPIO_S_GPBDAT           2
#define GPIO_S_GPBSET           2
#define GPIO_S_GPBCLEAR         2
#define GPIO_S_GPBTOGGLE        2
#define GPIO_S_AIODAT           2
#define GPIO_S_AIOSET           2
#define GPIO_S_AIOCLEAR         2
#define GPIO_S_AIOTOGGLE        2

#endif /* HW_GPIO_H */
