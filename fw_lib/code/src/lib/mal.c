/**************************************************************************************************
 * 
 * \file mal.c
 * 
 * \brief Memory abstraction layer implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "inc/lib/mal.h"

#include "inc/lib/crc.h"
#include "inc/lib/debug.h"
#include "inc/lib/mem.h"
#include "inc/net/eeprom.h"
#include "inc/net/flash.h"
#include "inc/hal/hapi.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**************************************************************************************************
 * 
 * Defines for the memory abstraction layer
 * 
 *************************************************************************************************/

#define MAL_WORDS       (MEM_PAGE / 2U)     /* Page size as number of 16-bit words              */

#define MAL_HEADER      3U                  /* Number of 16-bit words for page header           */
#define MAL_DATA        (MAL_WORDS - MAL_HEADER)    /* Number of 16-bit words for page data     */

#define MAL_RESET       0xFFFFU             /* Page word value at reset                         */

/**
 * TODO: Remove ACTIVE and SHADOW identifiers. Pages in the lower half are always active and pages
 * in the upper half are always shadow. When writing data to EEPROM, first write over shadow page,
 * and if successful, write over active page. When reading data, first read the active page, and
 * if not successful, read the shadow page.
 */

#define MAL_ACTIVE      0x1101U             /* Active page status code                          */
#define MAL_SHADOW      0x5501U             /* Shadow page status code                          */

#define MAL_FLASH_KEY   (0xC0DEACDCUL)      /* Key to know if FLASH can be used to store config */

/**************************************************************************************************
 * 
 * Memory page definition. NOTE: This assumes that actual page size equals default size defined
 * by \b MEM_PAGE. True page size abstraction would require dynamic memory allocation or more
 * complex memory abstraction logic. To be developed in future...
 * 
 *************************************************************************************************/
union mal_page {
    uint16_t buffer[MAL_WORDS];     /* Page buffer (header + data buffer)                       */
    struct {
        uint16_t status;            /* Page status                                              */
        uint16_t crc;               /* Data buffer CRC                                          */
        uint16_t rfu;               /* Reserved for future use                                  */
        uint16_t data[MAL_DATA];    /* Data buffer                                              */
    };
};

/**************************************************************************************************
 * 
 * Memory abstraction layer object definition
 * 
 *************************************************************************************************/
struct mal {
    const struct net *eeprom;       /* EEPROM driver handler                                    */
    const struct net *flash;
    const struct mem *mem;          /* Memory object handler                                    */
    bool open;                      /* True when MAL is open                                    */
    uint16_t npage;                 /* Total number of memory pages                             */
};

/**************************************************************************************************
 * 
 * \brief Swaps bytes in 16-bit word
 * 
 * \param data 16-bit word
 * 
 * \return 16-bit word with swapped bytes
 * 
 *************************************************************************************************/
static inline uint16_t
mal__swap16(uint16_t data)
{
    uint16_t word16 = 0U;

    word16 |= ((data << 8) & 0xFF00U);
    word16 |= ((data >> 8) & 0x00FFU);

    return word16;
}

/**************************************************************************************************
 * 
 * \brief Swaps words in 32-bit word and bytes in two 16-bit words
 * 
 * \param data 32-bit word
 * 
 * \return 32-bit word with swapped words and bytes
 * 
 *************************************************************************************************/
static inline uint32_t
mal__swap32(uint32_t data)
{
    uint32_t word32 = 0U;

    word32 |= (((uint32_t) mal__swap16(data) << 16) & 0xFFFF0000UL);
    word32 |= (((uint32_t) mal__swap16(data >> 16)) & 0x0000FFFFUL);

    return word32;
}

/**************************************************************************************************
 * 
 * \brief Unpacks single 16-bit word to two 8-bit bytes
 * 
 * \param data 16-bit word
 * \param buffer data buffer for two 8-bit bytes
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
mal__unpack16(uint16_t data, uint8_t *buffer)
{
    buffer[0] = (uint8_t) ((data >> 0) & 0x00FFU);
    buffer[1] = (uint8_t) ((data >> 8) & 0x00FFU);
}

/**************************************************************************************************
 * 
 * \brief Packs two 8-bit bytes into single 16-bit word
 * 
 * \param buffer data buffer with two 8-bit bytes
 * 
 * \return Packed 16-bit word
 * 
 *************************************************************************************************/
static inline uint16_t
mal__pack16(const uint8_t *buffer)
{
    uint16_t word16 = 0U;

    word16 |= (((uint16_t) buffer[0] >> 0) & 0x00FFU);
    word16 |= (((uint16_t) buffer[1] << 8) & 0xFF00U);

    return word16;
}

/**************************************************************************************************
 * 
 * \brief Packs four 8-bit bytes into single 32-bit word
 * 
 * \param buffer data buffer with four 8-bit bytes
 * 
 * \return Packed 32-bit word
 * 
 *************************************************************************************************/
static inline uint32_t
mal__pack32(const uint8_t *buffer)
{
    uint32_t word32 = 0U;

    word32 |= ((uint32_t) mal__pack16(buffer+0) >>  0);
    word32 |= ((uint32_t) mal__pack16(buffer+2) << 16);

    return word32;
}

/**************************************************************************************************
 * 
 * \brief Unpacks a 32-bit word into four 8-bit bytes
 * 
 * \param buffer data buffer with four 8-bit bytes
 * 
 * 
 *************************************************************************************************/
static inline void
mal__unpack32(uint32_t data, uint8_t *buffer)
{
    buffer[0] = (uint8_t) ((data >> 0) & 0x00FFUL);
    buffer[1] = (uint8_t) ((data >> 8) & 0x00FFUL);
    buffer[2] = (uint8_t) ((data >> 16) & 0x00FFUL);
    buffer[3] = (uint8_t) ((data >> 24) & 0x00FFUL);
}

/**************************************************************************************************
 * 
 * \brief Initializes page to given value
 * 
 * \param page page handler
 * \param value target value
 * 
 * \return None
 * 
 *************************************************************************************************/
static void
mal__init_page(union mal_page *page, uint16_t value)
{
    unsigned i;
    for (i = 0U; i < MAL_WORDS; i++) {
        page->buffer[i] = value;
    }
}

/**************************************************************************************************
 * 
 * mal_read_page_raw()
 * 
 *************************************************************************************************/
int
mal_read_flash_raw(struct mal *mal, uint16_t *data)
{
    ASSERT(mal && data);

    if (!mal->open) {
        return -1;
    }

    int ret;

    struct flash_f f = {
        .address = mal->mem->serial, /* This is the starting position of the sector */
        .data = data,
        .length = mal->mem->rd.length/2
    };


    ret = flash_read(mal->flash,&f);

    return ret;
}

/**************************************************************************************************
 * 
 * mal_read_page_raw()
 * 
 *************************************************************************************************/
int
mal_write_flash_raw(struct mal *mal, uint16_t *data)
{
    ASSERT(mal && data);

    if (!mal->open) {
        return -1;
    }

    int ret;
    
    struct flash_f f = {
        .address = mal->mem->serial, /* This is the starting position of the sector */
        .data = data,
        .length = mal->mem->wr.length/2
    };

    ret = flash_write(mal->flash,&f);

    return ret;
}

/**************************************************************************************************
 * 
 * \brief Raw page read or write
 * 
 * \param mal MAL object handler
 * \param page page handler
 * \param num page number
 * \param size data buffer size
 * \param rw_ \b true for read and \b false for write operation
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
static int
mal__rw(struct mal *mal, union mal_page *page, uint16_t num, uint16_t size, bool rw_)
{
    const struct mem *mem = mal->mem;

    int ret;

    /** TODO: Think about how to not hardcode page size. Maybe dynamic memory allocation? */
    uint8_t buffer[MEM_PAGE];
    
    struct eeprom_f f = {
        .address = (mem->wr.address + (num * mem->page)),
        .length  = (2U * size),
        .data    = buffer
    };

    if(mal->mem->type == TYPE_EEPROM){
        if (rw_) {
            ret = eeprom_read(mal->eeprom, &f);

            if (ret == 1) {
                unsigned i;
                for (i = 0U; i < size; i++) {
                    page->buffer[i] = mal__pack16(&buffer[i*2U]);
                }
            }
        } else { /* !rw_ */
            unsigned i;
            for (i = 0U; i < size; i++) {
                mal__unpack16(page->buffer[i], &buffer[i*2U]);
            }

            ret = eeprom_write(mal->eeprom, &f);
        }

    }
    #if defined(F28004X) || defined(F28P65X) 
    else if(mal->mem->type == TYPE_FLASH){

        struct flash_f f_flash = {
            .address = (mem->rd.address + (num * mem->page)),
            .length  = size,
            .data    = (uint16_t *)buffer
        };

        if(rw_){
            ret = flash_read(mal->flash, &f_flash);
            if(ret == 1){
                unsigned i;
                for (i = 0U; i < size; i++) {
                    page->buffer[i] = buffer[i];
                }
            }
        } else {

            /** TODO: Think about how to not hardcode page size. Maybe dynamic memory allocation? */
            uint16_t flash_image[MEM_FLASH_SIZE];
            
            /** NOTE:  In order to write the config into the flash we have to:
             * 1. Read its content and store it in a temporary variable.
             * 2. Erase the flash.
             * 3. Modify the content of the temporary variable with the new values that we want.
             * 4. Write the flash with the content of the temporary variable */

            ret = mal_read_flash_raw(mal,&flash_image[0]);
            ret |= mal_erase(mal);
            
            unsigned i;
            for (i = 0; i < f_flash.length; i++) {
                flash_image[mem->rd.address - mem->serial + i + num*mem->page] = page->buffer[i];
            }

            ret |= mal_write_flash_raw(mal,&flash_image[0]);
        }

    }
    #endif

    return (ret == 1) ? 0 : -1;
}

/**************************************************************************************************
 * 
 * \brief Erases single memory page
 * 
 * \param mal MAL object handler
 * \param num page number
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
static int
mal__erase_page(struct mal *mal, uint16_t num)
{
    int ret;
    union mal_page page;

    mal__init_page(&page, MAL_RESET);

    /* Write reset value to page */
    ret = mal__rw(mal, &page, num, MAL_WORDS, false);
    if (ret < 0) {
        return -1;
    }

    mal__init_page(&page, (uint16_t) ~MAL_RESET);

    /* Readback page */
    ret = mal__rw(mal, &page, num, MAL_WORDS, true);
    if (ret < 0) {
        return -1;
    }

    /* Verify page */
    unsigned i;
    for (i = 0U; i < MAL_WORDS; i++) {
        if (page.buffer[i] != MAL_RESET) {
            return -1;
        }
    }
    
    return 0;
}

/**************************************************************************************************
 * 
 * \brief Reads single page and checks data CRC
 * 
 * \param mal MAL object handler
 * \param page page handler
 * \param num page number
 * \param size data buffer size
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
static int
mal__read_page(struct mal *mal, union mal_page *page, uint16_t num, uint16_t size)
{
    int ret;

    mal__init_page(page, MAL_RESET);

    ret = mal__rw(mal, page, num, (size+MAL_HEADER), true);
    if (ret < 0) {
        return -1;
    }

    uint16_t crc = crc_calculate(page->data, size);
    if (crc != page->crc) {
        return -1;
    }

    return 0;
}

/**************************************************************************************************
 * 
 * \brief Writes and verifies single page
 * 
 * \param mal MAL object handler
 * \param page page handler
 * \param num page number
 * \param data data buffer
 * \param size data buffer size
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
static int
mal__write_page(struct mal *mal, union mal_page *page, uint16_t num, const uint16_t *data,
    uint16_t size)
{
    int ret;

    mal__init_page(page, MAL_RESET);

    uint16_t status = MAL_ACTIVE;
    if (num >= (mal->npage / 2U)) {
        status = MAL_SHADOW;
    }

    uint16_t crc = crc_calculate(data, size);
    uint16_t rfu = 0U;

    page->status = status;
    page->crc = crc;
    page->rfu = rfu;

    unsigned i;
    for (i = 0U; i < size; i++) {
        page->data[i] = data[i];
    }

    /* Write page */
    ret = mal__rw(mal, page, num, (size+MAL_HEADER), false);
    if (ret < 0) {
        return -1;
    }

    /* Readback page */
    ret = mal__read_page(mal, page, num, size);
    if (ret < 0) {
        return -1;
    }

    /* Check header */
    if ((status != page->status) || (crc != page->crc) || (rfu != page->rfu)) {
        return -1;
    }

    /* Check data */
    for (i = 0U; i < size; i++) {
        if (data[i] != page->data[i]) {
            return -1;
        }
    }

    return 0;
}

/**************************************************************************************************
 * 
 * mal_new()
 * 
 *************************************************************************************************/
struct mal *
mal_new(const struct net *eeprom)
{
    ASSERT(eeprom);

    if (!eeprom) {
        return NULL;
    }

    static struct mal mal = {
        .eeprom = NULL,
        .flash  = NULL,
        .mem    = NULL,
        .open   = false,
        .npage  = 0U
    };

    mal.eeprom = eeprom;

    return &mal;
}

/**************************************************************************************************
 *
 * mal_new()
 *
 *************************************************************************************************/
struct mal *
mal_new_flash(const struct net *flash)
{
    ASSERT(eeprom);

    if (!flash) {
        return NULL;
    }

    static struct mal mal = {
        .eeprom = NULL,
        .flash  = NULL,
        .mem    = NULL,
        .open   = false,
        .npage  = 0U
    };

    mal.flash = flash;

    return &mal;
}

/**************************************************************************************************
 * 
 * mal_open()
 * 
 *************************************************************************************************/
int
mal_open(struct mal *mal, const struct mal_usr *usr)
{
    ASSERT(mal && usr && usr->mem);

    if (mal->open) {
        return -1;
    }

    const struct mem *mem = usr->mem;

    mal->mem = mem;

    /** The MAL implementation supports only default page size. TODO: Improve this in future.. */
    ASSERT(mem->page == MEM_PAGE);

    mal->npage = mem->wr.length / mem->page;

    int ret;

    struct eeprom_usr eeprom_usr = {
        .mem = mem
    };

    if(mal->mem->type == TYPE_EEPROM){
        ret = eeprom_open(mal->eeprom, &eeprom_usr);
    }
    #if defined(F28004X) || defined(F28P65X) 
    else if(mal->mem->type == TYPE_FLASH){
        ret = flash_open(mal->flash);
    }
    #endif

    if (ret < 0) {
        return -1;
    }

    mal->open = true;

    return 0;
}

/**************************************************************************************************
 * 
 * mal_close()
 * 
 *************************************************************************************************/
int
mal_close(struct mal *mal)
{
    ASSERT(mal);

    if (!mal->open) {
        return -1;
    }

    mal->open = false;

    int ret;

    if(mal->mem->type == TYPE_EEPROM){
        ret = eeprom_close(mal->eeprom);
    }
    #if defined(F28004X) || defined(F28P65X) 
    else if(mal->mem->type == TYPE_FLASH){
        ret = flash_close(mal->flash);
    }
    #endif

    if (ret < 0) {
        return -1;
    }

    return 0;
}

/**************************************************************************************************
 * 
 * mal_erase()
 * 
 *************************************************************************************************/
int
mal_erase(struct mal *mal)
{
    ASSERT(mal);

    if (!mal->open) {
        return -1;
    }

    int ret = 0;
    unsigned num;

    if(mal->mem->type == TYPE_EEPROM){
        for (num = 0U; num < (mal->npage / 2U); num++) {
            ret |= mal_erase_page(mal, num);
        }
    }
    #if defined(F28004X) || defined(F28P65X) 
    else if(mal->mem->type == TYPE_FLASH){
        ret = flash_erase(mal->flash,mal->mem->sector);
    }
    #endif
    
    return (ret < 0) ? -1 : 0;
}

/**************************************************************************************************
 * 
 * mal_erase_page()
 * 
 *************************************************************************************************/
int
mal_erase_page(struct mal *mal, uint16_t num)
{
    ASSERT(mal);

    if (!mal->open) {
        return -1;
    }

    if (num >= (mal->npage / 2U)) {
        return -1;
    }

    int ret = 0;

    /* Erase active page */
    ret |= mal__erase_page(mal, num);

    /* Erase shadow page */
    ret |= mal__erase_page(mal, mal->npage-1U-num);

    return (ret < 0) ? -1 : 0;
}

/**************************************************************************************************
 * 
 * mal_read_serial_number()
 * 
 *************************************************************************************************/
int
mal_read_serial_number(struct mal *mal, uint32_t *serial)
{
    ASSERT(mal && serial);

    if (!mal->open) {
        return -1;
    }

    int ret;

    uint8_t buffer[4];

    struct eeprom_f f = {
        .address = mal->mem->serial,
        .length  = 4U,
        .data    = buffer
    };

    if(mal->mem->type == TYPE_EEPROM){
        ret = eeprom_read(mal->eeprom, &f);
        if (ret != 1) {
            return -1;
        }

        *serial = mal__swap32(mal__pack32(buffer));
    }
    #if defined(F28004X) || defined(F28P65X) 
    else if(mal->mem->type == TYPE_FLASH){
        struct flash_f f_flash = {
            .address = mal->mem->serial,
            .data = (uint16_t *)buffer,
            .length = 4U
        };

        ret = flash_read(mal->flash,&f_flash);
        if (ret != 1) {
            return -1;
        }
        uint32_t key = (((uint32_t)buffer[3])<< 16) | buffer[2];
        *serial = (((uint32_t)buffer[1])<< 16) | buffer[0];
        if(*serial == 0xFFFFFFFFUL || key != MAL_FLASH_KEY){
            return -1;
        }
    }
    #endif
    
    return 0;
}

/**************************************************************************************************
 * 
 * mal_read_serial_number()
 * 
 *************************************************************************************************/
int
mal_write_serial_number(struct mal *mal, uint32_t serial)
{
    ASSERT(mal && serial);

    if (!mal->open) {
        return -1;
    }

    int ret;
    uint8_t buffer[4];

    struct eeprom_f f = {
        .address = mal->mem->serial,
        .length  = 4U,
        .data    = buffer
    };

    if(mal->mem->type == TYPE_EEPROM){
        mal__unpack32(mal__swap32(serial),buffer);

        ret = eeprom_write(mal->eeprom, &f);
    }

    #if defined(F28004X) || defined(F28P65X) 
    else if(mal->mem->type == TYPE_FLASH){

        buffer[0] = serial & 0x0000FFFFUL;
        buffer[1] = serial >> 16;
        buffer[2] = MAL_FLASH_KEY & 0x0000FFFFUL;
        buffer[3] = MAL_FLASH_KEY >> 16;

        /** TODO: Think about how to not hardcode page size. Maybe dynamic memory allocation? */
        uint16_t flash_image[MEM_FLASH_SIZE];
            
        /** NOTE:  In order to write the config into the flash we have to:
        * 1. Read its content and store it in a temporary variable.
        * 2. Erase the flash.
        * 3. Modify the content of the temporary variable with the new values that we want.
        * 4. Write the flash with the content of the temporary variable */
        ret = mal_read_flash_raw(mal,&flash_image[0]);
        ret |= mal_erase(mal);
            
        /* Serial number takes 2 FLASH words, plus the MAL_FLASH_KEY that takes another 2 words */
        unsigned i;
        for (i = 0; i < 4; i++) {
            flash_image[i] = buffer[i];
        }

        ret |= mal_write_flash_raw(mal,&flash_image[0]);
    }
    #endif

    if (ret != 1) {
        return -1;
    }

    return 0;
}

/**************************************************************************************************
 * 
 * mal_read_page()
 * 
 *************************************************************************************************/
int
mal_read_page(struct mal *mal, uint16_t num, uint16_t *data, uint16_t size)
{
    ASSERT(mal && data && (size > 0U));

    if (!mal->open) {
        return -1;
    }

    if ((num >= (mal->npage / 2U)) || (size > MAL_DATA)) {
        return -1;
    }

    int ret;
    union mal_page page;
    
    /* First read lower-half page */
    ret = mal__read_page(mal, &page, num, size);

    bool corrupted = (ret < 0) || ((page.status != MAL_ACTIVE) && (page.status != MAL_SHADOW));

    if (!corrupted) {
        unsigned i;
        for (i = 0U; i < size; i++) {
            data[i] = page.data[i];
        }
    }

    if (corrupted || (page.status == MAL_SHADOW)) {
        num = mal->npage - 1U - num;

        /* Read upper-half page */
        ret = mal__read_page(mal, &page, num, size);

        /* Both pages are corrupted */
        if (corrupted && (ret < 0)) {
            return -1;
        }

        /* Take upper-half page only if it is ACTIVE or lower-half page is corrupted */
        if (corrupted || ((ret >= 0) && (page.status == MAL_ACTIVE))) {
            unsigned i;
            for (i = 0U; i < size; i++) {
                data[i] = page.data[i];
            }
        }
    }
    
    return 0;
}

/**************************************************************************************************
 * 
 * mal_read_page_raw()
 * 
 *************************************************************************************************/
int
mal_read_page_raw(struct mal *mal, uint16_t num, uint16_t *data, uint16_t size)
{
    ASSERT(mal && data && (size > 0U));

    if (!mal->open) {
        return -1;
    }

    if ((num >= mal->npage) || (size > MAL_WORDS)) {
        return -1;
    }

    int ret;
    union mal_page page;

    ret = mal__rw(mal, &page, num, size, true);
    if (ret < 0) {
        return -1;
    }

    unsigned i;
    for (i = 0U; i < size; i++) {
        data[i] = page.buffer[i];
    }

    return 0;
}

/**************************************************************************************************
 * 
 * mal_write_page()
 * 
 *************************************************************************************************/
int
mal_write_page(struct mal *mal, uint16_t num, const uint16_t *data, uint16_t size)
{
    ASSERT(mal && data && (size > 0U));

    if (!mal->open) {
        return -1;
    }

    if ((num >= (mal->npage / 2U)) || (size > MAL_DATA)) {
        return -1;
    }

    int ret;
    union mal_page page;

    /* Read page from lower-half */
    ret = mal__read_page(mal, &page, num, size);

    if(mal->mem->type == TYPE_EEPROM){

    /**
     * If lower-half page is \b ACTIVE and valid, first write over upper- and then over lower-half
     * page. Otherwise, first write over lower- and then over upper-half page. In this way chances
     * that at least one page is valid are maximized.
     */
        if ((ret >= 0) && (page.status == MAL_ACTIVE)) {
            num = mal->npage - 1U - num;
        }

        /* Write and verify page */
        ret = mal__write_page(mal, &page, num, data, size);
        if (ret < 0) {
            return -1;
        }

        num = mal->npage - 1U - num;

        /* Write and verify page */
        ret = mal__write_page(mal, &page, num, data, size);
        if (ret < 0) {
            return -1;
        }

    }
    #if defined(F28004X) || defined(F28P65X) 
    else if(mal->mem->type == TYPE_FLASH){
        /* Write and verify page */
        /* For FLASH, it does not make sense to have ACTIVE and SHADOW
        pages, since the entire flash has to be erased every time there
        is a write cycle */
        #ifndef BOOTLOADER
        bool isr_status = hapi_get_isr_status_flag();
        hapi_isr_disable();
        #endif
        ret = mal__write_page(mal, &page, num, data, size);
        #ifndef BOOTLOADER
        if(isr_status) {
            hapi_isr_enable();
        }
        #endif
        if (ret < 0) {
            return -1;
        }
        
    }
    #endif

    return 0;
}
