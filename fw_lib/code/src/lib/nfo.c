/**************************************************************************************************
 * 
 * \file nfo.c
 * 
 * \brief Module data implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "inc/lib/nfo.h"

#include "cgen/lib/nfo.h"
#include "inc/lib/debug.h"

#include <stddef.h>

/**************************************************************************************************
 * 
 * Load firmware revision and datecode from an auto-generated file
 * 
 *************************************************************************************************/

#include "version.h"

/**************************************************************************************************
 * 
 * \brief Unpacks a 16-bit variable to two 8-bit variables
 * 
 * \param data 16-bit variable
 * \param buffer storage for two 8-bit variables
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
unpack16(uint16_t data, char *buffer)
{
    buffer[0] = (data >> 0) & 0x00FFU;
    buffer[1] = (data >> 8) & 0x00FFU;
}

/**************************************************************************************************
 * 
 * \brief Packs two 8-bit variables in a single 16-bit variable
 * 
 * \param data storage for two 8-bit variables
 * 
 * \return Packed 16-bit variable
 * 
 *************************************************************************************************/
static inline uint16_t
pack16(const char *data)
{
    return (((uint16_t) data[1] << 8) & 0xFF00U) | (data[0] & 0x00FFU);
}

/**************************************************************************************************
 * 
 * \brief Packs firmware revision or datecode
 * 
 * \param source storage for source (unpacked) array
 * \param target storage for target (packed) array
 * 
 * \return None
 * 
 *************************************************************************************************/
static void
firmware_pack16(const char *source, uint16_t *target)
{
    unsigned i;
    for (i = 0U; i < 14U; i += 2U) {
        target[i/2U] = pack16(source+i);
    }
}

/**************************************************************************************************
 * 
 * \brief Converts firmware version to string
 * 
 * \param version firmware version
 * 
 * \return None
 * 
 *************************************************************************************************/
static void
version(uint32_t version, char *buffer)
{
    unsigned i;
    for (i = 0U; i < 14U; i++) {
        if (i < 8U) {
            unsigned digit = (version >> (4U*i)) & 0xFU;
            buffer[7U-i] = digit + ((digit < 10) ? '0' : ('a'-10));
        } else { /* i >= 8U */
            buffer[i] = 0;
        }
    }
}

/**************************************************************************************************
 * 
 * \brief Converts epoch time to timestamp string in format %Y%m%d%H%M%S
 * 
 * \param epoch number of seconds since the epoch
 * \param buffer storage for timestamp
 * 
 * \return None
 * 
 *************************************************************************************************/
static void
timestamp(uint32_t epoch, char *buffer)
{
    const uint16_t days_in_month[] = {
    /*  Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec */
        31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    };

    struct ts {
        uint16_t year;
        uint16_t month;
        uint16_t day;
        uint16_t hour;
        uint16_t minute;
        uint16_t second;
    };

    struct ts time = {
        .year   = 1970U,
        .month  = 1U,
        .day    = 1U,
        .hour   = 0U,
        .minute = 0U,
        .second = 0U
    };

    time.second = epoch % 60U;
    epoch /= 60U; /* epoch is in minutes */

    time.minute = epoch % 60U;
    epoch /= 60U; /* epoch is in hours */

    time.hour = epoch % 24U;
    epoch /= 24U; /* epoch is in days */

    epoch++;

    uint16_t days;
    bool is_leap;

    while (true) {
        is_leap = ((time.year % 4 == 0) && (time.year % 100 != 0)) || (time.year % 400 == 0);

        days = is_leap ? 366U : 365U;
        if (epoch > days) {
            epoch -= days;
            time.year++;
        } else {
            break;
        }
    }

    days = 0U;

    while (true) {
        days = days_in_month[time.month-1];
        if (is_leap && time.month == 2U) {
            days++;
        }

        if (epoch > days) {
            epoch -= days;
            time.month++;
        } else {
            break;
        }
    }

    time.day = epoch;

    char *s = &buffer[13];
    uint16_t *digit = &time.second;

    unsigned i;
    for (i = 0U; i < 6U; i++, digit--) {
        unsigned j;
        for (j = 0U; j < 2U || *digit != 0U; j++) {
            *s-- = (*digit % 10) + '0';
            *digit /= 10;
        }
    }
}

/**************************************************************************************************
 * 
 * nfo_new()
 * 
 *************************************************************************************************/
struct nfo *
nfo_new(enum nfo_id id)
{
    NFO_OBJ_NEW(mod, NFO_NONE);
    NFO_OBJ_NEW(boot, NFO_BOOT);

    struct nfo *ptr = NULL;

#ifndef BOOTLOADER
    if (id == NFO_BOOT) {
        return &boot;
    } else {
        ptr = &mod;
        ptr->id = id;
    }
#else
    if (id == NFO_NONE) {
        return &mod;
    } else {
        ptr = &boot;
    }
#endif

    char _version[14];
    version(VERSION, _version);
    firmware_pack16(_version, ptr->version);


    char _timestamp[14];
    timestamp(TIMESTAMP, _timestamp);
    firmware_pack16(_timestamp, ptr->timestamp);

    return ptr;
}

/**************************************************************************************************
 * 
 * nfo_firmware_unpack16()
 * 
 *************************************************************************************************/
void
nfo_firmware_unpack16(const uint16_t *source, char *target)
{
    if (!(source && target)) {
        ASSERT(false);
        return;
    }

    unsigned i;
    for (i = 0U; i < 14U; i += 2U) {
        unpack16(source[i/2U], target+i);
    }
}

/**************************************************************************************************
 * 
 * nfo_set_hardware()
 * 
 *************************************************************************************************/
int
nfo_set_hardware(struct nfo *nfo, enum nfo_id id, uint16_t revision, uint16_t variant)
{
    if ((nfo->id != id) && (nfo->id != NFO_NONE)) {
        return -1;
    } else if ((nfo->id != id) || (nfo->revision != revision) || (nfo->variant != variant)) {
        nfo->id = id;
        nfo->revision = revision;
        nfo->variant = variant;
        return 1;
    } else {
        return 0;
    }
}

/**************************************************************************************************
 * 
 * nfo_set_address()
 * 
 *************************************************************************************************/
int
nfo_set_address(struct nfo *nfo, uint16_t address, uint16_t stack_size)
{
    if (address < 32U && stack_size && stack_size <= 32U) {
        if ((nfo->address != address) || (nfo->stack_size != stack_size)) {
            nfo->address = address;
            nfo->stack_size = stack_size;
            return 1;
        } else {
            return 0;
        }
    } else {
        return -1;
    }
}
