/**************************************************************************************************
 * 
 * \file conv.h
 * 
 * \brief Measurements conversion interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_LIB_CONV_H
#define _INC_LIB_CONV_H

#include "cgen/lib/conv.h"

#include "inc/lib/debug.h"

#include <stdint.h>
#include <stdbool.h>

#include "IQmathLib.h"

/**************************************************************************************************
 * 
 * Values that can be passed to \e conv_update() as the \e src parameter.
 * 
 *************************************************************************************************/
enum conv_src {
    CONV_NONE = 0,
    CONV_HAPI,                  /* Conversion data from HAPI                                    */
    CONV_MEM,                   /* Conversion data from memory (EEPROM)                         */
    CONV_USR,                   /* Conversion data from user over CAN                           */
    CONV_CALIB,                 /* Conversion data from calibration                             */
};

/**************************************************************************************************
 * 
 * Conversion parameters. Initialized in HAPI layer.
 * 
 *************************************************************************************************/
struct conv_cop {
    uint32_t res;               /* Measurement resolution (number of bins)                      */
    float fs;                   /* Full-scale range for linear transformation                   */
};

/**************************************************************************************************
 * 
 * Normalization parameters
 * 
 *************************************************************************************************/
struct conv_nop {
    int16_t offset;             /* Normalization offset                                         */
    int16_t gain_c;             /* Normalization gain correction                                */
};

/**************************************************************************************************
 * 
 * Raw-normalized transformation parameters
 * 
 *************************************************************************************************/
struct conv_rnt {
    enum conv_src src;          /* Normalization data source (HAPI, MEM, USR, or PT)            */
    int16_t offset;             /* Normalization offset                                         */
    _iq gain;                   /* Normalization gain                                           */
};

/**************************************************************************************************
 * 
 * Custom normalized-physical transformation
 * 
 *************************************************************************************************/
struct conv_npt {
    const void *obj;            /* Object for custom transformation                             */

    /**********************************************************************************************
     * 
     * \brief Transforms normalized to physical value
     * 
     * \param obj transformation object
     * \param norm normalized value
     * 
     * \return Physical value
     * 
     *********************************************************************************************/
    float (*n2p)(const void *obj, _iq norm);

    /**********************************************************************************************
     * 
     * \brief Transforms physical to normalized value
     * 
     * \param obj transformation object
     * \param phy physical value
     * 
     * \return Normalized value
     * 
     *********************************************************************************************/
    _iq (*p2n)(const void *obj, float phy);
};

/**************************************************************************************************
 * 
 * Conversion object definition
 * 
 *************************************************************************************************/
struct conv {
    struct conv_cop cop;        /* Conversion parameters initialized in HAPI layer              */
    struct conv_nop hapi;       /* Normalization parameters from HAPI                           */
    struct conv_nop mem;        /* Normalization parameters from memory (EEPROM)                */
    struct conv_nop usr;        /* Normalization parameters from user over CAN                  */
    struct conv_nop calib;      /* Normalization parameters from calibration                    */
    struct conv_rnt rnt;        /* Raw-normalized transformation parameters                     */
    struct conv_npt npt;        /* Custom normalized-physical transformation                    */
    bool invert;
};

/**************************************************************************************************
 * 
 * \brief Updates raw-normalized transformation parameters
 * 
 * \param conv conversion object handler
 * \param src transformation parameters source
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
conv_update(struct conv *conv, enum conv_src src);

/**************************************************************************************************
 * 
 * \brief Converts raw to normalized value
 * 
 * \param conv conversion object handler
 * \param raw raw value
 * 
 * \return Normalized value
 * 
 *************************************************************************************************/
static inline _iq
conv_r2n(const struct conv *conv, uint32_t raw)
{
    ASSERT(conv);

    const struct conv_rnt *rnt = &conv->rnt;

    ASSERT(rnt->src != CONV_NONE);

    int32_t raw_0 = (int32_t) raw - rnt->offset;

    if(conv->invert){
        return -_IQ24mpyI32(rnt->gain, raw_0);
    }else{
        return _IQ24mpyI32(rnt->gain, raw_0);
    }
}

/**************************************************************************************************
 * 
 * \brief Converts normalized to raw value
 * 
 * \param conv conversion object handler
 * \param norm normalized value
 * 
 * \return Raw value
 * 
 *************************************************************************************************/
static inline uint32_t
conv_n2r(const struct conv *conv, _iq norm)
{
    ASSERT(conv);

    const struct conv_rnt *rnt = &conv->rnt;

    ASSERT(rnt->src != CONV_NONE);
    ASSERT(rnt->gain != 0U);

    return ((norm / rnt->gain) + rnt->offset);
}

/**************************************************************************************************
 * 
 * \brief Converts normalized to physical value
 * 
 * \param conv conversion object handler
 * \param norm normalized value
 * 
 * \return Physical value
 * 
 *************************************************************************************************/
static inline float
conv_n2p(const struct conv *conv, _iq norm)
{
    ASSERT(conv);

    const struct conv_cop *cop = &conv->cop;
    const struct conv_npt *npt = &conv->npt;

    if (npt->obj && npt->n2p) {
        return npt->n2p(npt->obj, norm);
    } else {
        ASSERT(cop->fs > 0.0f);
        return (_IQtoF(norm) * cop->fs);
    }
}

/**************************************************************************************************
 * 
 * \brief Converts physical to normalized value
 * 
 * \param conv conversion object handler
 * \param phy physical value
 * 
 * \return Normalized value
 * 
 *************************************************************************************************/
static inline _iq
conv_p2n(const struct conv *conv, float phy)
{
    ASSERT(conv);

    const struct conv_cop *cop = &conv->cop;
    const struct conv_npt *npt = &conv->npt;

    if (npt->obj && npt->p2n) {
        return npt->p2n(npt->obj, phy);
    } else {
        ASSERT(cop->fs > 0.0f);
        return _IQ(phy / cop->fs);
    }
}

#endif /* _INC_LIB_CONV_H */
