/**************************************************************************************************
 * 
 * \file nfo.h
 * 
 * \brief Module data interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_LIB_NFO_H
#define _INC_LIB_NFO_H

#include <stdint.h>
#include <stdbool.h>

/**************************************************************************************************
 * 
 * Defines for the module
 * 
 *************************************************************************************************/

#define NFO_SN_DEFAULT  (0x00C0FFEEUL)  /* Default serial number                                */

#define NFO_HW_DEFAULT  (0x00FFU)       /* Default hardware revision and variant                */

/**************************************************************************************************
 * 
 * Module identifier definition
 * 
 *************************************************************************************************/
enum nfo_id {
    NFO_NONE = 0x00,                /* Default identifier                                       */
    NFO_BOOT = 0xAA,                /* Bootloader                                               */
    NFO_LF45 = 0x01,                /* Line filter (45 A)                                       */
    NFO_UP25 = 0x02,                /* Unidirectional PFC/boost converter (25 kW)               */
    NFO_LL25 = 0x03,                /* Unidirectional LLC converter (25 kW)                     */
    NFO_BC25 = 0x04,                /* Unidirectional buck converter (25 kW)                    */
    NFO_BP25 = 0x07,                /* Bidirectional PFC/AFE converter (25 kW)                  */
    NFO_BI25 = 0x08,                /* Bidirectional isolated converter (25 kW)                 */
    NFO_SFRA = 0x09,                /* Software frequency response analyzer                     */
    NFO_DLOG = 0x0A,                /* DLOG identifier (for advanced diagnostics)               */
    NFO_LOGGING = 0x0B,             /* LOGGING identifier (for text output log through CAN)     */
    NFO_VG11_FM01 = 0x0C,           /* VG11 bidirectional charger (11 kW) - primary side DSP    */
    NFO_VG11_FM02 = 0x0D,           /* VG11 bidirectional charger (11 kW) - secondary side DSP  */
    NFO_CB01 = 0x0E,                /* Combiner board (contactor control, fan control, etc. )   */
    NFO_TBT = 0x10,                 /* Thermal bar tester based on FAN1 with i2C mxl and led    */
    NFO_FAN1 = 0x11,                /* Fan controller module                                    */
    NFO_TBTE = 0x12,                /* RnD ADC board                                           */
    NFO_FP  = 0x42,                   /*front panel*/
};

/**************************************************************************************************
 * 
 * Legacy module data object definition
 * 
 *************************************************************************************************/
struct nfo_leg {
    uint16_t id;                    /* Device identifier                                        */
    uint16_t variant;               /* Hardware variant                                         */
    uint16_t revision;              /* Hardware revision number                                 */
    uint16_t address;               /* Device address                                           */
    uint16_t stack_size;            /* Total number of devices in the stack                     */
//  uint16_t;                       /* Added by compiler (align to 32-bit boundary)             */
    uint32_t serial;                /* Device unique serial number                              */
};

/**************************************************************************************************
 * 
 * Module data object definition
 * 
 *************************************************************************************************/
struct nfo {
    enum nfo_id id;                 /* Device identifier                                        */
//  uint16_t;                       /* Added by compiler (align to 32-bit boundary)             */
    uint32_t serial;                /* Device unique serial number                              */
    uint16_t version[7];            /* Firmware version as commit SHA1 value                    */
    uint16_t timestamp[7];          /* Firmware timestamp in format %Y%m%d%H%M%S                */
    uint16_t variant;               /* Hardware variant                                         */
    uint16_t revision;              /* Hardware revision number                                 */
    uint16_t address;               /* Device address                                           */
    uint16_t stack_size;            /* Total number of devices in the stack (obsolete)          */
};

/**************************************************************************************************
 * 
 * \brief Creates new module data object
 * 
 * \param id module identifier
 * 
 * \return Module data handler
 * 
 *************************************************************************************************/
extern struct nfo *
nfo_new(enum nfo_id id);

/**************************************************************************************************
 * 
 * \brief Unpacks firmware revision or datecode
 * 
 * \param source storage for source (packed) array
 * \param target storage for target (unpacked) array
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
nfo_firmware_unpack16(const uint16_t *source, char *target);

/**************************************************************************************************
 * 
 * \brief Sets new hardware revision and variant
 * 
 * \param nfo module data handler
 * \param id device identifier
 * \param revision hardware revision
 * \param variant hardware variant
 * 
 * \return 0 if parameters do not need to be updated; 1 if parameters have been updated
 * 
 *************************************************************************************************/
extern int
nfo_set_hardware(struct nfo *nfo, enum nfo_id id, uint16_t revision, uint16_t variant);

/**************************************************************************************************
 * 
 * \brief Sets new device address and stack size
 * 
 * \param nfo module data handler
 * \param address device address
 * \param stack_size stack size
 * 
 * \return -1 if input parameters are invalid; 0 if parameters do not need to be updated; 1 if
 * parameters have been updated
 * 
 *************************************************************************************************/
extern int
nfo_set_address(struct nfo *nfo, uint16_t address, uint16_t stack_size);

#endif /* _INC_LIB_NFO_H */
