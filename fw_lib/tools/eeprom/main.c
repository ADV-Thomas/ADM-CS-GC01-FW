/**************************************************************************************************
 * 
 * \file main.c
 * 
 * \brief Program to create EEPROM file with default data
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "inc/drv/eeprom.h"
#include "inc/drv/file.h"
#include "inc/drv/io.h"
#include "inc/lib/mal.h"
#include "inc/lib/mem.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/**************************************************************************************************
 * 
 * Redefine module data structure
 * 
 *************************************************************************************************/
struct nfo {
    uint16_t id;                    /* Module identifier                                        */
    uint16_t rsvd;                  /* Added by C2000 compiler to align on 32-bit boundary      */
    uint32_t serial;                /* Serial number                                            */
    uint16_t version[7];            /* Firmware version                                         */
    uint16_t timestamp[7];          /* Firmware timestamp                                       */
    uint16_t variant;               /* Module variant                                           */
    uint16_t revision;              /* Module revision                                          */
    uint16_t address;               /* Module address                                           */
    uint16_t stack_size;            /* Stack size (not in use)                                  */
};

/**************************************************************************************************
 * 
 * Data page definition
 * 
 *************************************************************************************************/
enum data_page {
    DATA_PAGE_MODULE_L     = 0,     /* Module information (legacy page)                         */
    DATA_PAGE_ADC_OFFSET_L = 1,     /* ADC offset for normalization (legacy page)               */
    DATA_PAGE_ADC_GAIN_L   = 2,     /* ADC gain correction for normalization (legacy page)      */
    DATA_PAGE_BOOTLOADER   = 3,     /* Bootloader information                                   */
    DATA_PAGE_MODULE       = 4,     /* Module information                                       */
    DATA_PAGE_ADC_OFFSET   = 5,     /* ADC normalization offset                                 */
    DATA_PAGE_ADC_GAIN     = 6,     /* ADC normalization gain correction                        */
    DATA_PAGE_CMPL_OFFSET  = 7,     /* Window comparator offset for normalization (low)         */
    DATA_PAGE_CMPL_GAIN    = 8,     /* Window comparator gain corr. for normalization (low)     */
    DATA_PAGE_CMPH_OFFSET  = 9,     /* Window comparator offset for normalization (high)        */
    DATA_PAGE_CMPH_GAIN    = 10     /* Window comparator gain corr. for normalization (high)    */
};

/**************************************************************************************************
 * 
 * io_connect()
 * 
 *************************************************************************************************/
int
io_connect(enum io io, enum io_fun exp)
{
    return -1;
}

/**************************************************************************************************
 * 
 * io_disconnect()
 * 
 *************************************************************************************************/
int
io_disconnect(enum io io)
{
    return -1;
}

/**************************************************************************************************
 * 
 * \brief TODO:
 * 
 * \param mem memory object handler
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
static int
create(const struct mem *mem)
{
    FILE *fp = fopen(mem->fpath, "wb");
    if (!fp) {
        return -1;
    }

    fseek(fp, 0U, SEEK_SET);

    uint8_t buffer[MEM_PAGE];

    unsigned i;
    for (i = 0U; i < MEM_PAGE; i++) {
        buffer[i] = 0xFFU;
    }

    for (i = 0U; i < (mem->rd.length / mem->page); i++) {
        fwrite(buffer, MEM_PAGE, 1U, fp);
    }

    /* Serial number shows as 00DE-C0DE */
    const uint8_t serial[4] = { 0x00, 0xDE, 0xC0, 0xDE };

    fseek(fp, mem->serial, SEEK_SET);
    fwrite(serial, sizeof(serial), 1U, fp);

    fclose(fp);

    return 0;
}

/**************************************************************************************************
 * 
 * main()
 * 
 *************************************************************************************************/
void
main(int argc, char *argv[])
{
    if (argc < 5) {
        return;
    }

    const unsigned id  = atoi(argv[1]);
    const unsigned rev = atoi(argv[2]);
    const unsigned var = atoi(argv[3]);
    const unsigned adr = atoi(argv[4]);

    if ((id == 0U) || (id == 5U) || (id == 6U) || (id > 8U) || (adr > 31U)) {
        return;
    }

    const struct net *file = file_new();

    const struct net *eeprom = eeprom_new(file);
    const struct mem *mem = mem_new();

    create(mem);

    struct mal *mal = mal_new();

    struct mal_usr mal_usr = {
        .eeprom = eeprom,
        .mem = mem
    };

    mal_open(mal, &mal_usr);

    uint32_t serial;
    mal_read_serial_number(mal, &serial);

    struct nfo mod = {
        .id         = id,
        .serial     = serial,
        .version    = { 0, 0, 0, 0, 0, 0, 0 },
        .timestamp  = { 0, 0, 0, 0, 0, 0, 0 },
        .variant    = var,
        .revision   = rev,
        .address    = adr,
        .stack_size = 1
    };

    struct nfo boot = {
        .id         = 0U, /* NFO_BOOT */
        .serial     = serial,
        .version    = { 0, 0, 0, 0, 0, 0, 0 },
        .timestamp  = { 0, 0, 0, 0, 0, 0, 0 },
        .variant    = 0,
        .revision   = 0,
        .address    = 0,
        .stack_size = 1
    };

    const unsigned nfo_size = (sizeof(struct nfo) / sizeof(uint16_t));

    const unsigned size[1+8] = {
        0U, /* BOOT */
        8U, /* LF45 */
        0U, /* UP25 */
        0U, /* LL25 */
        0U, /* BC25 */
        0U, /* NONE */
        0U, /* NONE */
        0U, /* BP25 */
        0U  /* BI25 */
    };

    const unsigned adc_size = size[id];

    const uint16_t offset[1+8][10] = {
        {    0U,    0U,    0U,    0U,    0U,    0U,    0U,    0U,    0U,    0U }, /* BOOT */
        { 2043U, 2043U, 2043U, 2043U, 2043U, 2043U,    0U,    0U,    0U,    0U }, /* LF45 */
        {    0U,    0U,    0U,    0U,    0U,    0U,    0U,    0U,    0U,    0U }, /* UP25 */
        {    0U,    0U,    0U,    0U,    0U,    0U,    0U,    0U,    0U,    0U }, /* LL25 */
        {    0U,    0U,    0U,    0U,    0U,    0U,    0U,    0U,    0U,    0U }, /* BC25 */
        {    0U,    0U,    0U,    0U,    0U,    0U,    0U,    0U,    0U,    0U }, /* NONE */
        {    0U,    0U,    0U,    0U,    0U,    0U,    0U,    0U,    0U,    0U }, /* NONE */
        {    0U,    0U,    0U,    0U,    0U,    0U,    0U,    0U,    0U,    0U }, /* BP25 */
        {    0U,    0U,    0U,    0U,    0U,    0U,    0U,    0U,    0U,    0U }, /* BI25 */
    };

    const uint16_t gain[10] = { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };

    mal_write_page(mal, DATA_PAGE_BOOTLOADER, (uint16_t *) &boot,          nfo_size);
    mal_write_page(mal, DATA_PAGE_MODULE,     (uint16_t *) &mod,           nfo_size);
    mal_write_page(mal, DATA_PAGE_ADC_OFFSET, (uint16_t *) &offset[id][0], adc_size);
    mal_write_page(mal, DATA_PAGE_ADC_GAIN,   (uint16_t *) &gain[0],       adc_size);
}
