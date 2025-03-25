/**************************************************************************************************
 * 
 * \file id.c
 * 
 * \brief Microcontroller identifier implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "inc/drv/id.h"

#include "inc/lib/debug.h"

/**************************************************************************************************
 * 
 * Identifier register location
 * 
 *************************************************************************************************/
#if defined(F2803X)
#define ID_REG      0x000882UL
#define ID_PART     0x3D7E80UL
#elif defined(F28004X)
#define ID_REG      0x05D008UL
#define ID_UID      0x0703CCUL
#define ID_DC21     0x05D03AUL
#elif defined(F28P65X)
#define ID_REG      0x05D008UL
#define ID_UID_0    0x00072172U + 0xAU
#define ID_UID_1    0x00072172U + 0xCU

#endif

/**************************************************************************************************
 * 
 * Register decode recipe
 * 
 *************************************************************************************************/
union id_reg {
#if defined(F2803X)
    struct {
        uint32_t CLASSID:16;
        uint32_t REVID:16;
    };
    struct {
        /* CLASSID */
        uint32_t part:8;                /* Part number                                          */
        uint32_t :8;
        /* REVID */
        uint32_t rev:16;                /* Revision identifier                                  */
    };
#elif defined(F28004X) || defined(F28P65X)  
    struct {
        uint32_t PARTIDL;
        uint32_t PARTIDH;
        uint32_t REVID;
    };
    struct {
        /* PARTIDL */
        uint32_t :6;
        uint32_t qual:2;                /* Chip qualification                                   */
        uint32_t count:3;               /* Pin count                                            */
        uint32_t :2;
        uint32_t :2;                    /* InstaSPIN - not used                                 */
        uint32_t :1;
        uint32_t size:8;                /* Flash size                                           */
        uint32_t :8;
        /* PARTIDH */
        uint32_t :8;
        uint32_t family:8;              /* Device family                                        */
        uint32_t part:8;                /* Part number                                          */
        uint32_t class:8;               /* Class identifier                                     */
        /* REVID */
        uint32_t rev:16;                /* Revision identifier                                  */
        uint32_t :16;
    };
#endif

};

/**************************************************************************************************
 * 
 * Private identifier object
 * 
 *************************************************************************************************/
static struct id id = {
    .uid   = 0U,
    .part  = ID_PART_NONE,
    .rev   = ID_REV_0,
    .q100  = false,
    .count = 0U,
    .size  = ID_SIZE_NONE,
    .clb_available = false,
};

/**************************************************************************************************
 * 
 * id_init()
 * 
 *************************************************************************************************/
void
id_init(void)
{
#ifndef VIRTUAL
    union id_reg reg = *((volatile union id_reg *) ID_REG);

    id.part = (enum id_part) reg.part;
    
    id.rev = (enum id_rev) reg.rev;


#if defined(F2803X)
    id.size = ID_SIZE_128K;

    const uint16_t COUNT[] = {
        0U, 56U, 64U, 80U
    };

    uint16_t count = *((volatile uint16_t *) ID_PART) & 0x3U;
    ASSERT(count != 0U);

    id.count = COUNT[count];

#elif defined(F28004X) 
    id.size = (enum id_size) reg.size;

    const uint16_t COUNT[] = {
        56U, 64U, 64U, 0U, 0U, 100U, 0U, 0U
    };

    uint16_t count = reg.count;
    ASSERT((count != 3U) && (count != 4U) && (count != 6U) && (count != 7U));
    
    id.count = COUNT[count];
    id.q100 = (count == 1U);
    
    enum id_qual qual = (enum id_qual) reg.qual;

    /**
     * Unique identifier is available only on TMS Rev. B devices. It is different on each
     * individual device with the same PARTIDH.
     */
    if ((qual == ID_QUAL_TMS) && (id.rev == ID_REV_B)) {
        id.uid = *((volatile uint32_t *) ID_UID);
    }

    /* Check if CLB is available and set flag if so */
    uint16_t clb_present = *((volatile uint16_t *) ID_DC21);
    if((clb_present & 0x000F) == 0x000F){
        id.clb_available = true;
    }
#elif defined(F28P65X) 
    id.size = (enum id_size) reg.size;

    const uint16_t COUNT[] = {
        100U, 176U, 169U, 256U,0U, 0U, 0U, 0U
    };

    uint16_t count = reg.count;
    ASSERT((count != 4U) && (count != 5U) && (count != 6U) && (count != 7U));
    
    id.count = COUNT[count];
  
    
    //enum id_qual qual = (enum id_qual) reg.qual;

    id.uid = *((volatile uint32_t *) ID_UID_0);

    uint16_t clb_present = 0x000F; //clb always present 
    if((clb_present & 0x000F) == 0x000F){
        id.clb_available = true;
    }
#else 
#error "hal.c - dsp not defined"
#endif

#else
    id = (struct id) {
        .uid   = 0U,
#if defined(F2803X)
        .part  = ID_PART_F28035,
        .rev   = ID_REV_A,
#elif defined(F28004X)
        .part  = ID_PART_F280041,
        .rev   = ID_REV_B,
#elif defined(F28P65X)
        .part  = ID_PART_F28P650DK6,
        .rev   = ID_REV_A,
#else 
#error "hal.c - dsp not defined"
#endif
        .q100  = false,
        .count = 64U,
        .size  = ID_SIZE_128K
    };
#endif /* VIRTUAL */
}

/**************************************************************************************************
 * 
 * id_get()
 * 
 *************************************************************************************************/
const struct id *
id_get(void)
{
    return &id;
}
