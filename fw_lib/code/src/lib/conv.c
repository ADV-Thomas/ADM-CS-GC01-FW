/**************************************************************************************************
 * 
 * \file conv.c
 * 
 * \brief Measurements conversion implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "inc/lib/conv.h"

/**************************************************************************************************
 * 
 * conv_update()
 * 
 *************************************************************************************************/
int
conv_update(struct conv *conv, enum conv_src src)
{
    ASSERT(conv);

    struct conv_cop *cop = &conv->cop;
    struct conv_nop *nop;

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
    case CONV_CALIB:
        nop = &conv->calib;
        break;
    default:
        return -1;
    }

    ASSERT(cop->res > 0U);

    /**
     * Normalization gain correction is a signed 16-bit value in range -32768..32767. This means
     * that the effective gain correction is in range 0.5..1.5.
     */
    double gain_c = 1.0L + ((double) nop->gain_c / 0xFFFFU);

    _iq gain = _IQ(gain_c / (double) cop->res);

    conv->rnt = (struct conv_rnt) {
        .src = src,
        .offset = nop->offset,
        .gain = gain
    };
    
    return 0;
}
