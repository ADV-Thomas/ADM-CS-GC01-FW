/**************************************************************************************************
 * 
 * \file alert.h
 * 
 * \brief Alert module interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_LIB_ALERT_H
#define _INC_LIB_ALERT_H

#include <stdbool.h>

/**************************************************************************************************
 * 
 * Alert identifier. The data is encoded as follows:
 * - 0x000000FF     Alert index
 * - 0x0000FF00     Debounce counter period
 * - 0x00FF0000     Debounce counter
 * - 0x01000000     True to enable alert latch
 * - 0x02000000     True to enable alert latch clear
 * - 0x04000000     Alert debounce state
 * - 0x08000000     Alert output state
 * - 0xF0000000     Alert group (see 'enum alert_group')
 * 
 * NOTE: If you add new identifier in this enum, you must also add it to the \e alert_u array in
 * the alert module implementation (alert.c).
 * 
 *************************************************************************************************/
enum alert {
    ALERT_INTERNAL = 0x10000000U,       /* Internal interlock line (hardware trip)              */
    ALERT_EXTERNAL = 0x10000001U,       /* External interlock line (hardware trip)              */
    ALERT_SYSTEM   = 0x11000002U,       /* System (critical) error                              */
    ALERT_MEMORY   = 0x11000003U,       /* External memory (EEPROM)                             */
    ALERT_NFO      = 0x11000004U,       /* Module data                                          */
    ALERT_ADC      = 0x21000005U,       /* ADC calibration data                                 */
    ALERT_WCH      = 0x21000006U,       /* WCH calibration data                                 */
    ALERT_WCS      = 0x13000007U,       /* Window comparators in software                       */
    ALERT_PLL      = 0x13000008U,        /* PLL unlocked condition                               */
    ALERT_FSI_SYNC = 0x13000009U        /* FSI not synchronizing                                */  
};

/**************************************************************************************************
 * 
 * Alert group identifier
 * 
 *************************************************************************************************/
enum alert_group {
    ALERT_ALL     = 0x0U,               /* All alert groups combined                            */
    ALERT_ERROR   = 0x1U,
    ALERT_WARNING = 0x2U
};

/**************************************************************************************************
 * 
 * \brief Updates alert state
 * 
 * \param id alert identifier
 * \param cond condition
 * 
 * \details This function updates alert state with respect to condition defined by the \e cond
 * parameter. Alert is set to active state if \e cond is \b true. Otherwise, non-latch alert is
 * cleared to inactive state. The input condition is filtered if debounce period is set in the
 * \e alert_period() function.
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void 
alert_set(enum alert id, bool cond);

/**************************************************************************************************
 * 
 * \brief Returns alert status
 * 
 * \param id alert identifier
 * 
 * \return \b true when alert is active
 * 
 *************************************************************************************************/
extern bool
alert_get(enum alert id);

/**************************************************************************************************
 * 
 * \brief Returns alert group status
 * 
 * \param group alert group identifier
 * 
 * \return \b true when group alert is active
 * 
 *************************************************************************************************/
extern bool
alert_get_group(enum alert_group group);

/**************************************************************************************************
 * 
 * \brief Clears alert to inactive state
 * 
 * \param id alert identifier
 * 
 * \details This function clears alert given by \e id parameter to inactive state. Note that some
 * alerts cannot be cleared. The function also updates group alerts.
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
alert_clear(enum alert id);

/**************************************************************************************************
 * 
 * \brief Clears all alerts to inactive state
 * 
 * \param None
 * 
 * \details This function clears all alerts to inactive state. Note that some alerts cannot be
 * cleared. The function also updates group alerts.
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
alert_reset(void);

/**************************************************************************************************
 * 
 * \brief Programs alert condition debounce period
 * 
 * \param id alert identifier
 * \param period condition debounce period (0..255)
 * 
 * \details This function programs alert condition debounce period. The condition is required to
 * be stable over the time defined by the debounce period before updating the alert output.
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
alert_period(enum alert id, unsigned period);

#endif /* _INC_LIB_ALERT_H */
