/**************************************************************************************************
 * 
 * \file id.h
 * 
 * \brief Microcontroller identifier interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_DRV_ID_H
#define _INC_DRV_ID_H

#include <stdint.h>
#include <stdbool.h>

/**************************************************************************************************
 * 
 * Part number
 * 
 *************************************************************************************************/
enum id_part {
    ID_PART_NONE    = 0x0000U,
#if defined(F2803X)
    ID_PART_F28030  = 0x00ABU,
    ID_PART_F28031  = 0x00AFU,
    ID_PART_F28032  = 0x00B3U,
    ID_PART_F28033  = 0x00B7U,
    ID_PART_F28034  = 0x00BBU,
    ID_PART_F28035  = 0x00BFU,
#elif defined(F28004X)
    ID_PART_F280040 = 0x00F6U,
    ID_PART_F280041 = 0x00F7U,
    ID_PART_F280045 = 0x00FBU,
    ID_PART_F280048 = 0x00FEU,
    ID_PART_F280049 = 0x00FFU,
#elif defined(F28P65X)
    ID_PART_F28P650DK9 = 0x00FFU,
    ID_PART_F28P650DK7 = 0x00FEU,
    ID_PART_F28P650DK8 = 0x00FDU,
    ID_PART_F28P659DK8 = 0x00FDU,
    ID_PART_F28P650SK7 = 0x00FCU,
    ID_PART_F28P650DK6 = 0x00FBU,
    ID_PART_F28P650SK6 = 0x00FAU,
    ID_PART_F28P659DH8 = 0x00F9U,
    ID_PART_F28P650SH6 = 0x00F8U,
    ID_PART_F28P659SH6 = 0x00F8U,
    ID_PART_F28P650DH6 = 0x00F7U,
    ID_PART_F28P650SH7 = 0x00F6U, 
#else
#error "id.h - dsp not defined"
#endif
};

/**************************************************************************************************
 * 
 * Revision identifier
 * 
 *************************************************************************************************/
enum id_rev {
    ID_REV_0 = 0U,
    ID_REV_A = 1U,
#ifdef F28004X
    ID_REV_B = 2U
#endif /* F28004X */
};

/**************************************************************************************************
 * 
 * Chip qualification
 * 
 *************************************************************************************************/
enum id_qual {
    ID_QUAL_TMX = 0U,           /* Engineering sample                                           */
    ID_QUAL_TMP = 1U,           /* Pilot production                                             */
    ID_QUAL_TMS = 2U            /* Fully qualified                                              */
};

/**************************************************************************************************
 * 
 * Flash size
 * 
 *************************************************************************************************/
enum id_size {
    ID_SIZE_NONE = 0U,
    ID_SIZE_128K = 5U,
#if defined(F28004X) || defined(F28P65X)
    ID_SIZE_256K = 6U
#endif /* F28004X ||  F28P65X */
};

/**************************************************************************************************
 * 
 * Identifier object definition
 * 
 *************************************************************************************************/
struct id {
    uint32_t uid;               /* Unique device identifier                                     */
    enum id_part part;          /* Part number                                                  */
    enum id_rev rev;            /* Revision identifier                                          */
    bool q100;                  /* True when chip is Q100 rated                                 */
    uint16_t count;             /* Pin count                                                    */
    enum id_size size;          /* Flash size                                                   */
    bool clb_available;         /* CLB is present in the device                                 */
};

/**************************************************************************************************
 * 
 * \brief Initializes device identifiers
 * 
 * \param None
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
id_init(void);

/**************************************************************************************************
 * 
 * \brief Returns identifier object handler
 * 
 * \param None
 * 
 * \return Identifier object handler
 * 
 *************************************************************************************************/
extern const struct id *
id_get(void);

#endif /* _INC_DRV_ID_H */
