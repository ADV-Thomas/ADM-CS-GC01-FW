/**************************************************************************************************
 * 
 * \file data.h
 * 
 * \brief Memory data abstraction interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_LIB_DATA_H
#define _INC_LIB_DATA_H

#include "inc/lib/conv.h"

#include <stdbool.h>

/**************************************************************************************************
 * 
 * Forward declarations
 * 
 *************************************************************************************************/

struct mal;
struct nfo;
struct adc;
struct wch;
struct fan_ctl;

enum error_code{
    SUCCESS = 0,
    ERROR = -1,
    SERIAL_NOT_READABLE = -2,
    LEGACY = 1,
};

/**************************************************************************************************
 * 
 * \brief Reads or write module data to external memory
 * 
 * \param mal MAL object handler
 * \param nfo module data object handler
 * \param rw_ true for read and false for write operation
 * 
 * \return 0 if operation is successful; 1 if data is loaded from legacy page; -1 otherwise
 * 
 *************************************************************************************************/
extern int
data_nfo(struct mal *mal, struct nfo *nfo, bool rw_);

/**************************************************************************************************
 * 
 * \brief Reads or writes ADC calibration data to external memory
 * 
 * \param mal MAL object handler
 * \param adc ADC object handler
 * \param nfo module data handler
 * \param src conversion data source
 * \param rw_ true for read and false for write operation
 * 
 * \details This function reads or writes ADC calibration data to external memory. It also updates
 * calibration data in the ADC object.
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
data_adc(struct mal *mal, const struct adc *adc, const struct nfo *mod, enum conv_src src,
    bool rw_);

/**************************************************************************************************
 * 
 * \brief Reads or writes WCH calibration data to external memory
 * 
 * \param mal MAL object handler
 * \param wch WCH object handler
 * \param nfo module data handler
 * \param src conversion data source
 * \param rw_ true for read and false for write operation
 * 
 * \details This function reads or writes WCH calibration data to external memory. It also updates
 * calibration data in the WCH object.
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
data_wch(struct mal *mal, const struct wch *wch, const struct nfo *mod, enum conv_src src,
    bool rw_);


/**************************************************************************************************
 * 
 * \brief Reads or write module data to external memory
 * 
 * \param mal MAL object handler
 * \param nfo module data object handler
 * \param rw_ true for read and false for write operation
 * 
 * \return 0 if operation is successful; 1 if data is loaded from legacy page; -1 otherwise
 * 
 *************************************************************************************************/
extern int
data_fan_ctl(struct mal *mal, struct fan_ctl *fan_ctl, bool rw_);
#endif /* _INC_LIB_DATA_H */
