/**************************************************************************************************
 * 
 * \file data.c
 * 
 * \brief Memory data abstraction implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "inc/lib/data.h"

#include "inc/api/adc.h"
#include "inc/api/wch.h"
#include "inc/lib/debug.h"
#include "inc/lib/mal.h"
#include "inc/lib/nfo.h"
#include "inc/lib/fan_ctl.h"

#include <stdint.h>
#include <stdbool.h>

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
    DATA_PAGE_CMPL_OFFSET  = 7,     /* Window comp offset for normalization (low) NOT USED      */
    DATA_PAGE_CMPL_GAIN    = 8,     /* Window comp gain corr. for normalization (low) NOT USED  */
    DATA_PAGE_CMPH_OFFSET  = 9,     /* Window comp offset for normalization (high) NOT USED     */
    DATA_PAGE_CMPH_GAIN    = 10,    /* Window comp gain corr. for normalization (high) NOT USED */
    DATA_PAGE_FAN_CONTROL  = 11     /* Fan control board (CS-FAN1) configuration                */
};

/**************************************************************************************************
 * 
 * Defines for the module
 * 
 *************************************************************************************************/

#define PAGE_WORDS      32U         /* EEPROM page size as number of 16-bit words               */

/**************************************************************************************************
 * 
 * \brief Writes module data to external memory
 * 
 * \param mal MAL object handler
 * \param nfo module data object handler
 * \param page memory page
 * 
 * \return 0 if write operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
static int
nfo_write(struct mal *mal, const struct nfo *nfo, enum data_page page)
{
    int ret;
    struct nfo_leg leg;

    ret = mal_write_page(mal, page, (const uint16_t *) nfo, sizeof(*nfo));

    if(page == DATA_PAGE_MODULE){
        // Write also legacy page, so that old firmwares can boot with the new changes
        leg.address = nfo->address;
        leg.id = nfo->id;
        leg.revision = nfo->revision;
        leg.serial = nfo->serial;
        leg.stack_size = nfo->stack_size;
        leg.variant = nfo->variant;

        mal_write_page(mal, DATA_PAGE_MODULE_L, (uint16_t *) &leg, sizeof(leg));
    }

    if (ret < 0) {
        return -1;
    }

    return 0;
}

/**************************************************************************************************
 * 
 * \brief Initializes module object with data from legacy page
 * 
 * \param mal MAL object handler
 * \param nfo module data object handler
 * 
 * \return 0 if module has been initialized; -1 otherwise
 * 
 *************************************************************************************************/
static int
nfo_legacy(struct mal *mal, struct nfo *nfo)
{
    /* There is no legacy page for bootloader data */
    if (nfo->id == NFO_BOOT) {
        return -1;
    }

    int ret;
    struct nfo_leg leg;

    ret = mal_read_page(mal, DATA_PAGE_MODULE_L, (uint16_t *) &leg, sizeof(leg));
    if (ret < 0) {
        return -1;
    }

    /* Initialize module object with data from legacy page */
    nfo->id = (enum nfo_id) leg.id;
    nfo->revision = leg.revision;
    nfo->variant = leg.variant;
    nfo->address = leg.address;
    nfo->stack_size = leg.stack_size;

    return 0;
}

/**************************************************************************************************
 * 
 * \brief Reads module data from external memory
 * 
 * \param mal MAL object handler
 * \param nfo module data object handler
 * \param page memory page
 * 
 * \return 0 if read operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
static int
nfo_read(struct mal *mal, struct nfo *nfo, enum data_page page)
{
    int ret;

    ret = mal_read_serial_number(mal, &nfo->serial);
    if (ret < 0) {
        return SERIAL_NOT_READABLE;
    }
    
    struct nfo tmp;

    ret = mal_read_page(mal, page, (uint16_t *) &tmp, sizeof(tmp));

    if (ret < 0) {
        if (nfo->id != NFO_BOOT) {
            /**
             * Load module data from legacy page. Legacy page must never be deleted because older
             * bootloader versions would not be able to run.
             */
            ret = nfo_legacy(mal, nfo);
            return ((ret < 0) ? ERROR : LEGACY);
        } else { /* nfo->id == NFO_BOOT */
#ifdef BOOTLOADER
            /**
             * Bootloader page can be initialized only from the bootloader. The page contains only
             * information about the bootloader revision and datecode.
             */
            nfo_write(mal, nfo, page);
            return SUCCESS;
#else
            return ERROR;
#endif /* BOOTLOADER */
        }
    }

    /* Bootloader can correct only bootloader data, and module can correct only module data */
#ifdef BOOTLOADER
    bool flag = nfo->id == NFO_BOOT;
#else
    bool flag = nfo->id != NFO_BOOT;
#endif /* BOOTLOADER */

    /**
     * EEPROM has best information about device identifier and firmware revision and datecode only
     * when bootloader is accessing module data, and when module is accessing bootloader data.
     */
    if (!flag) {
        if (nfo->id == NFO_NONE) {
            nfo->id = tmp.id;
        }

        unsigned i;
        for (i = 0U; i < 7U; i++) {
            nfo->version[i] = tmp.version[i];
            nfo->timestamp[i] = tmp.timestamp[i];
        }
    }

    /**
     * EEPROM always has best information about hardware revision and variant, and stack position
     * and size. Copy this information from EEPROM to \b nfo object.
     */
    nfo->revision = tmp.revision;
    nfo->variant = tmp.variant;
    nfo->address = tmp.address;
    nfo->stack_size = tmp.stack_size;

    /* Correct EEPROM data if necessary */
    if (flag) {
        bool correct = false;

        correct |= (nfo->id != tmp.id);
        correct |= (nfo->serial != tmp.serial);

        unsigned i;
        for (i = 0U; i < 7U && !correct; i++) {
            correct |= (nfo->version[i] != tmp.version[i]);
            correct |= (nfo->timestamp[i] != tmp.timestamp[i]);
        }

        if (correct) {
            nfo_write(mal, nfo, page);
        }
    }
    
    return SUCCESS;
}

/**************************************************************************************************
 * 
 * data_nfo()
 * 
 *************************************************************************************************/
int
data_nfo(struct mal *mal, struct nfo *nfo, bool rw_)
{
    ASSERT(mal && nfo);

    int ret;

    enum data_page page = DATA_PAGE_BOOTLOADER;
    if (nfo->id != NFO_BOOT) {
        page = DATA_PAGE_MODULE;
    }

    if (rw_) {
        ret = nfo_read(mal, nfo, page);
    } else { /* !rw_ */
        ret = nfo_write(mal, nfo, page);
    }

    return ret;
}

/**************************************************************************************************
 * 
 * \brief Writes ADC calibration data to external memory
 * 
 * \param mal MAL object handler
 * \param adc ADC object handler
 * \param src conversion source
 * \param page memory page
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
static int
data_adc_write(struct mal *mal, const struct adc *adc, enum conv_src src, enum data_page page)
{
    int ret;

    uint16_t buffer[PAGE_WORDS];

    void **_adc = (void **) adc;
    struct adc_priv *priv = (struct adc_priv *) _adc[0];
    struct adc_var **var = (struct adc_var **) &_adc[1];

    unsigned i;
    for (i = 0; i < priv->size; i++) {
        const struct conv *conv = var[i]->conv;
        const struct conv_nop *nop;
        switch (src) {
        case CONV_HAPI:
            nop = &conv->hapi;
            break;
        case CONV_MEM:
            nop = &conv->mem;
            break;
        case CONV_USR:
            nop = &conv->usr;
            break;
#if 0
        case CONV_PT:
            nop = &conv->pt;
            break;
#endif
        default:
            ASSERT(false);
            return -1;
        }

        switch (page) {
        case DATA_PAGE_ADC_OFFSET:
            buffer[i] = nop->offset;
            break;
        case DATA_PAGE_ADC_GAIN:
            buffer[i] = nop->gain_c;
            break;
        default:
            ASSERT(false);
            return -1;
        }
    }

    ret = mal_write_page(mal, page, buffer, priv->size);
    if (ret < 0) {
        return -1;
    }
    
    return 0;
}

/**************************************************************************************************
 * 
 * \brief Reads ADC calibration data from legacy page
 * 
 * \param mal MAL object handler
 * \param adc ADC object handler
 * \param mod module data object handler
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
static int
data_adc_legacy(struct mal *mal, const struct adc *adc, const struct nfo *mod)
{
    int ret;

    uint16_t buffer[PAGE_WORDS];

    void **_adc = (void **) adc;
    struct adc_priv *priv = (struct adc_priv *) _adc[0];

    enum data_page legacy[] = { DATA_PAGE_ADC_OFFSET_L, DATA_PAGE_ADC_GAIN_L };
    const enum data_page pages[] = { DATA_PAGE_ADC_OFFSET, DATA_PAGE_ADC_GAIN };

    unsigned size;

    switch (mod->id) {
    /**
     * Variables on legacy page:
     *  In L1, In L2, In L3, Out L1, Out L2, Out L3
     */
    case NFO_LF45:
        size = 12U; // We have to use 12 because its 6 32-bit variables

        /** In legacy Filter firmwares, offset was written in page 2 and scale in 
         * page 3. However, this is different from other modules, and also, page 3
         * clashes with bootloader page. So as workaround, we ignore the scale page,
         * and we read the offset page, which is page 2 (DATA_PAGE_ADC_GAIN_L), even
         * though the names dont match */
        legacy[0] = DATA_PAGE_ADC_GAIN_L;
        legacy[1] = DATA_PAGE_ADC_GAIN_L;
        break;
    /**
     * Variables on legacy page:
     *  current_out, voltage_out, voltage_in, ground_fault (int32 variables)
     */
    case NFO_LL25:
        size = 8U;

        /** In legacy LLC firmwares, offset was written in page 2 and scale in 
         * page 3. However, this is different from other modules, and also, page 3
         * clashes with bootloader page. So as workaround, we ignore the scale page,
         * and we read the offset page, which is page 2 (DATA_PAGE_ADC_GAIN_L), even
         * though the names dont match */
        legacy[0] = DATA_PAGE_ADC_GAIN_L;
        legacy[1] = DATA_PAGE_ADC_GAIN_L;
        break;

    case NFO_UP25:
        size = 8U;

        /** In legacy UP25 firmwares, offset was written in page 2 and scale in 
         * page 3. However, this is different from other modules, and also, page 3
         * clashes with bootloader page. So as workaround, we ignore the scale page,
         * and we read the offset page, which is page 2 (DATA_PAGE_ADC_GAIN_L), even
         * though the names dont match */
        legacy[0] = DATA_PAGE_ADC_GAIN_L;
        legacy[1] = DATA_PAGE_ADC_GAIN_L;
        break;
    /**
     * Variables on legacy page:
     *  voltage_in, voltage_out, current_out
     */
    case NFO_BC25:
        size = 6U;

        /** In legacy BUCK firmwares, offset was written in page 2 and scale in 
         * page 3. However, this is different from other modules, and also, page 3
         * clashes with bootloader page. So as workaround, we ignore the scale page,
         * and we read the offset page, which is page 2 (DATA_PAGE_ADC_GAIN_L), even
         * though the names dont match */
        legacy[0] = DATA_PAGE_ADC_GAIN_L;
        legacy[1] = DATA_PAGE_ADC_GAIN_L;
        break;
    /**
     * Variables on legacy page:
     *  current_u, current_v, current_w, voltage_u, voltage_v, voltage_w, voltage_dc
     */
    case NFO_BP25:
        size = 7U;
        break;
    /**
     * There is no legacy page.
     */
    case NFO_BI25:
        return -1;
    default:
        ASSERT(false);
        return -1;
    }

    if (size > PAGE_WORDS) {
        ASSERT(false);
        return -1;
    }

    unsigned i,j;
    for (i = 0U; i < 2U; i++) {
        ret = mal_read_page(mal, legacy[i], buffer, size);
        if (ret < 0) {
            return -1;
        }
        
        switch (mod->id) {

        case NFO_LL25:
            if(pages[i] == DATA_PAGE_ADC_OFFSET){
                // In old LLC firmwares, offset values are
                // stored as 32-bit, so two words each. We
                // have to convert it to 1 word by ignoring
                // the upper word which is always 0
                for(j=0;j<4;j++){
                    buffer[j] = buffer[2*j];
                }
                for(j=4;j<12;j++){
                    buffer[j] = 0;
                }
            }
            if(pages[i] == DATA_PAGE_ADC_GAIN){
                for(j=0;j<12;j++){
                    buffer[j] = 0;
                }
            }
            break;  
        case NFO_UP25:
            if(pages[i] == DATA_PAGE_ADC_OFFSET){
                // In old UP25 firmwares, offset values are
                // stored as 32-bit, so two words each. We
                // have to convert it to 1 word by ignoring
                // the upper word which is always 0
                for(j=0;j<4;j++){
                    buffer[j] = buffer[2*j];
                }
                for(j=4;j<9;j++){
                    buffer[j] = 0;
                }
            }
            if(pages[i] == DATA_PAGE_ADC_GAIN){
                for(j=0;j<9;j++){
                    buffer[j] = 0;
                }
            }
            break;
        case NFO_BC25:
            if(pages[i] == DATA_PAGE_ADC_OFFSET){
                // In old UP25 firmwares, offset values are
                // stored as 32-bit, so two words each. We
                // have to convert it to 1 word by ignoring
                // the upper word which is always 0
                for(j=0;j<3;j++){
                    buffer[j] = buffer[2*j];
                }
                for(j=3;j<7;j++){
                    buffer[j] = 0;
                }
            }
            if(pages[i] == DATA_PAGE_ADC_GAIN){
                for(j=0;j<7;j++){
                    buffer[j] = 0;
                }
            }
            break;            
        case NFO_LF45:
            if(pages[i] == DATA_PAGE_ADC_OFFSET){
                // In old Filter firmwares, offset values are
                // stored as 32-bit, so two words each. We
                // have to convert it to 1 word by ignoring
                // the upper word which is always 0
                for(j=0;j<6;j++){
                    buffer[j] = buffer[2*j];
                }
                for(j=6;j<12;j++){
                    buffer[j] = 0;
                }
            }
            if(pages[i] == DATA_PAGE_ADC_GAIN){
                for(j=0;j<12;j++){
                    buffer[j] = 0;
                }
            }
            break;
        case NFO_BP25:
            switch (pages[i]) {
            case DATA_PAGE_ADC_OFFSET:
                buffer[0] += 2048U;
                buffer[1] += 2048U;
                buffer[2] += 2048U;
                buffer[7] = 0U;
                buffer[8] = 0U;
                break;
            case DATA_PAGE_ADC_GAIN:
                buffer[7] = 0U;
                buffer[8] = 0U;
                break;
            default:
                ASSERT(false);
                return -1;
            }
            break;
        case NFO_BI25:
            return -1;
        default:
            ASSERT(false);
            return -1;
        }

        ret = mal_write_page(mal, pages[i], buffer, priv->size);
        if (ret < 0) {
            return -1;
        }
    }

    return 0;
}

/**************************************************************************************************
 * 
 * \brief Reads ADC calibration data from external memory
 * 
 * \param mal MAL object handler
 * \param adc ADC object handler
 * \param mod module data object handler
 * \param page memory page
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
static int
data_adc_read(struct mal *mal, const struct adc *adc, const struct nfo *mod, enum data_page page)
{
    int ret;

    uint16_t buffer[PAGE_WORDS];

    void **_adc = (void **) adc;
    struct adc_priv *priv = (struct adc_priv *) _adc[0];
    struct adc_var **var = (struct adc_var **) &_adc[1];

    ret = mal_read_page(mal, page, buffer, priv->size);
    if (ret < 0) {
        /**
         * Initialize ADC data from legacy pages, both offset and gain, only when read from page
         * with offset data fails. Thus, page with gain data should never fail.
         */
        if (page != DATA_PAGE_ADC_OFFSET) {
            return -1;
        }

        ret = data_adc_legacy(mal, adc, mod);
        if (ret < 0) {
            return -1;
        }
        
        ret = mal_read_page(mal, page, buffer, priv->size);
        if (ret < 0) {
            return -1;
        }
    }

    unsigned i;
    for (i = 0U; i < priv->size; i++) {
        switch (page) {
        case DATA_PAGE_ADC_OFFSET:
            var[i]->conv->mem.offset = buffer[i];
            var[i]->conv->usr.offset = buffer[i];
#if 0
            var[i]->conv->pt .offset = buffer[i];
#endif
            break;
        case DATA_PAGE_ADC_GAIN:
            var[i]->conv->mem.gain_c = buffer[i];
            var[i]->conv->usr.gain_c = buffer[i];
#if 0
            var[i]->conv->pt .gain_c = buffer[i];
#endif
            break;
        default:
            ASSERT(false);
            return -1;
        }
    }

    return 0;
}

/**************************************************************************************************
 * 
 * data_adc()
 * 
 *************************************************************************************************/
int
data_adc(struct mal *mal, const struct adc *adc, const struct nfo *mod, enum conv_src src,
    bool rw_)
{
    ASSERT(mal && adc && mod);

    void **_adc = (void **) adc;
    struct adc_priv *priv = (struct adc_priv *) _adc[0];

    if ((priv->size) == 0 || (priv->size > PAGE_WORDS)) {
        return -1;
    }

    int ret;

    const enum data_page pages[] = { DATA_PAGE_ADC_OFFSET, DATA_PAGE_ADC_GAIN };

    unsigned i;
    for (i = 0U; i < 2U; i++) {
        if (rw_) {
            ret = data_adc_read(mal, adc, mod, pages[i]);
        } else { /* !rw_ */
            ret = data_adc_write(mal, adc, src, pages[i]);
        }
        
        if (ret < 0) {
            return -1;
        }
    }

    ret = adc_conv(adc, src);
    if (ret < 0) {
        ASSERT(false);
        return -1;
    }

    return 0;
}

int
data_fan_ctl_read(struct mal *mal, struct fan_ctl *fan_ctl)
{
    int ret;
    int i;
    uint16_t buffer[PAGE_WORDS];
    struct node *node;

    ret = mal_read_page(mal, DATA_PAGE_FAN_CONTROL, buffer, PAGE_WORDS);
    if(ret < 0){
        return -1;
    }

    /* Remove hardcoded value of 4 */
    for(i=0; i<4; i++){
        node = (struct node *)&fan_ctl->node[i];
        node->suscribed = buffer[i*7];
        node->id = buffer[i*7+1] | (((uint32_t)buffer[i*7+2])<<16);
        node->offset = buffer[i*7+3];
        node->min_temp = buffer[i*7+4];
        node->max_temp = buffer[i*7+5];
        node->max_speed = buffer[i*7+6];
    }

    return 0;
}

int
data_fan_ctl_write(struct mal *mal, struct fan_ctl *fan_ctl)
{
    int ret;
    int i;
    uint16_t buffer[PAGE_WORDS];
 
    /* Remove hardcoded value of 4 */
    for(i=0; i<4; i++){
        buffer[i*7]= fan_ctl->node[i].suscribed;
        buffer[i*7+1]= fan_ctl->node[i].id & 0x0000FFFFUL;
        buffer[i*7+2]= fan_ctl->node[i].id >> 16;
        buffer[i*7+3]= fan_ctl->node[i].offset;
        buffer[i*7+4]= fan_ctl->node[i].min_temp;
        buffer[i*7+5]= fan_ctl->node[i].max_temp;
        buffer[i*7+6]= fan_ctl->node[i].max_speed;
    }

    ret = mal_write_page(mal, DATA_PAGE_FAN_CONTROL, buffer, PAGE_WORDS);
    if (ret < 0) {
        return -1;
    }
    return 0;
}

int
data_fan_ctl(struct mal *mal, struct fan_ctl *fan_ctl, bool rw_)
{
    ASSERT(mal && fan_ctl);

    int ret;

    if (rw_) {
        ret = data_fan_ctl_read(mal, fan_ctl);
    } else { /* !rw_ */
        ret = data_fan_ctl_write(mal, fan_ctl);
    }
        
    if (ret < 0) {
        return -1;
    }

    return 0;   
}

/**************************************************************************************************
 * 
 * \brief Writes ADC calibration data to external memory
 * 
 * \param mal MAL object handler
 * \param adc ADC object handler
 * \param src conversion source
 * \param page memory page
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
static int
data_wch_write(struct mal *mal, const struct wch *wch, enum conv_src src, enum data_page page)
{
    int ret;

    uint16_t buffer[PAGE_WORDS];

    void **_wch = (void **) wch;
    struct wch_priv *priv = (struct wch_priv *) _wch[0];
    struct wch_var **var = (struct wch_var **) &_wch[1];

    unsigned i;
    for (i = 0; i < priv->size; i++) {
        
        const struct conv_nop *nop;
        /* Set pointer to comparator object */
        const struct wch_cmp *cmp_i;

        switch (page) {
        case DATA_PAGE_CMPL_OFFSET:
            /* Intentional fall through */
        case DATA_PAGE_CMPL_GAIN:
            cmp_i = &var[i]->cmpl;
            break;
        case DATA_PAGE_CMPH_OFFSET:
            /* Intentional fall through */
        case DATA_PAGE_CMPH_GAIN:
            cmp_i = &var[i]->cmph;
            break;
        default:
            return -1;
        }

        switch (src) {
        case CONV_HAPI:
            nop = &cmp_i->conv->hapi;
            break;
        case CONV_MEM:
            nop = &cmp_i->conv->mem;
            break;
        case CONV_USR:
            nop = &cmp_i->conv->usr;
            break;
#if 0
        case CONV_PT:
            nop = &conv->pt;
            break;
#endif
        default:
            ASSERT(false);
            return -1;
        }

        switch (page) {
        case DATA_PAGE_CMPL_OFFSET:
            /* Intentional fall through */
        case DATA_PAGE_CMPH_OFFSET:
            buffer[i] = nop->offset;
            break;
        case DATA_PAGE_CMPL_GAIN:
            /* Intentional fall through */
        case DATA_PAGE_CMPH_GAIN:
            buffer[i] = nop->gain_c;
            break;
        default:
            return -1;
        }
    }

    ret = mal_write_page(mal, page, buffer, priv->size);
    if (ret < 0) {
        return -1;
    }
    
    return 0;
}


#if 0
/**************************************************************************************************
 * 
 * \brief Reads ADC calibration data from legacy page
 * 
 * \param mal MAL object handler
 * \param adc ADC object handler
 * \param mod module data object handler
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
static int
data_wch_legacy(struct mal *mal, const struct wch *wch, const struct nfo *mod)
{
    int ret;

    uint16_t buffer[PAGE_WORDS];

    void **_wch = (void **) wch;
    struct wch_priv *priv = (struct wch_priv *) _wch[0];

    const enum data_page legacy[] = { DATA_PAGE_ADC_OFFSET_L, DATA_PAGE_ADC_GAIN_L };
    const enum data_page pages[] = { DATA_PAGE_ADC_OFFSET, DATA_PAGE_ADC_GAIN };

    unsigned size;

    switch (mod->id) {
    /**
     * Variables on legacy page:
     *  current_u, current_v, current_w, voltage_u, voltage_v, voltage_w, voltage_dc
     */
    case NFO_BP25:
        size = 7U;
        break;
    /**
     * There is no legacy page.
     */
    case NFO_BI25:
        return -1;
    default:
        ASSERT(false);
        return -1;
    }

    if (size > PAGE_WORDS) {
        ASSERT(false);
        return -1;
    }

    unsigned i;
    for (i = 0U; i < 2U; i++) {
        ret = mal_read_page(mal, legacy[i], buffer, size);
        if (ret < 0) {
            return -1;
        }
        
        switch (mod->id) {
        case NFO_LF45:
            switch (pages[i]) {
            case DATA_PAGE_ADC_OFFSET:
                /* Intentional fall through */
            case DATA_PAGE_ADC_GAIN:
                buffer[6] = 0U;
                buffer[7] = 0U;
                break;
            default:
                ASSERT(false);
                break;
            }
            break;
        case NFO_BP25:
            switch (pages[i]) {
            case DATA_PAGE_ADC_OFFSET:
                buffer[0] += 2048U;
                buffer[1] += 2048U;
                buffer[2] += 2048U;
                buffer[7] = 0U;
                buffer[8] = 0U;
                break;
            case DATA_PAGE_ADC_GAIN:
                buffer[7] = 0U;
                buffer[8] = 0U;
                break;
            default:
                ASSERT(false);
                return -1;
            }
            break;
        case NFO_BI25:
            return -1;
        default:
            ASSERT(false);
            return -1;
        }

        ret = mal_write_page(mal, pages[i], buffer, priv->size);
        if (ret < 0) {
            return -1;
        }
    }

    return 0;
}
#endif 

/**************************************************************************************************
 * 
 * \brief Reads ADC calibration data from external memory
 * 
 * \param mal MAL object handler
 * \param adc ADC object handler
 * \param mod module data object handler
 * \param page memory page
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
static int
data_wch_read(struct mal *mal, const struct wch *wch, const struct nfo *mod, enum data_page page)
{
    int ret;

    uint16_t buffer[PAGE_WORDS];

    void **_wch = (void **) wch;
    struct wch_priv *priv = (struct wch_priv *) _wch[0];
    struct wch_var **var = (struct wch_var **) &_wch[1];

    ret = mal_read_page(mal, page, buffer, priv->size);
    if (ret < 0) {
#if 0
        /**
         * Initialize WCH data from legacy pages, both offset and gain, only when read from page
         * with offset data fails. Thus, page with gain data should never fail.*/
         
        if (page != DATA_PAGE_ADC_OFFSET) {
            return -1;
        }

        ret = data_wch_legacy(mal, wch, mod);
        if (ret < 0) {
            return -1;
        }
        
        ret = mal_read_page(mal, page, buffer, priv->size);
        if (ret < 0) {
            return -1;
        }
        return -1;
#endif
    }


    unsigned i;
    for (i = 0U; i < priv->size; i++) {
        /* Set pointer to comparator object */
        struct wch_cmp *cmp_i;
        switch (page) {
        case DATA_PAGE_CMPL_OFFSET:
            /* Intentional fall through */
        case DATA_PAGE_CMPL_GAIN:
            cmp_i = &var[i]->cmpl;
            break;
        case DATA_PAGE_CMPH_OFFSET:
            /* Intentional fall through */
        case DATA_PAGE_CMPH_GAIN:
            cmp_i = &var[i]->cmph;
            break;
        default:
            return -1;
        }

        switch (page) {
        case DATA_PAGE_CMPL_OFFSET:
            /* Intentional fall through */
        case DATA_PAGE_CMPH_OFFSET:
            cmp_i->conv->mem.offset = buffer[i];
            cmp_i->conv->usr.offset = buffer[i];
            //cmp_i->conv.pt .offset = buffer[i];
            break;
        case DATA_PAGE_CMPL_GAIN:
            /* Intentional fall through */
        case DATA_PAGE_CMPH_GAIN:
            cmp_i->conv->mem.gain_c = buffer[i];
            cmp_i->conv->usr.gain_c = buffer[i];
            //cmp_i->conv.pt .gain = buffer[i];
            break;
        default:
            return -1;
        }
    }

    return 0;
}

/**************************************************************************************************
 * 
 * data_wch()
 * 
 *************************************************************************************************/
int
data_wch(struct mal *mal, const struct wch *wch, const struct nfo *mod, enum conv_src src,
    bool rw_)
{
    ASSERT(mal && wch && mod);

    void **_wch = (void **) wch;
    struct wch_priv *priv = (struct wch_priv *) _wch[0];

    if ((priv->size) == 0 || (priv->size > PAGE_WORDS)) {
        return -1;
    }

    int ret;

    /* EEPROM pages with calibration data */
    const enum data_page pages[] = {
        DATA_PAGE_CMPL_OFFSET,
        DATA_PAGE_CMPL_GAIN,
        DATA_PAGE_CMPH_OFFSET,
        DATA_PAGE_CMPH_GAIN
    };

    int i;
    for (i = 0U; i < 4U; i++) {
        if (rw_) {
            ret = data_wch_read(mal, wch, mod, pages[i]);
        } else { /* !rw_ */
            ret = data_wch_write(mal, wch, src, pages[i]);
        }
        
        if (ret < 0) {
            return -1;
        }
    }

    //ret = wch_conv(wch, CONV_MEM);
    if (ret < 0) {
        ASSERT(false);
        return -1;
    }

    return 0;
}
