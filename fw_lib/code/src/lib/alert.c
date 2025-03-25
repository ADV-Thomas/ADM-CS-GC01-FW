/**************************************************************************************************
 * 
 * \file alert.c
 * 
 * \brief Alert module implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "inc/lib/alert.h"

#include "inc/lib/debug.h"
#include "inc/lib/utils.h"

#include <stdint.h>
#include <stddef.h>

/**************************************************************************************************
 * 
 * Decode recipe for alert identifier
 * 
 *************************************************************************************************/
union alert_u {
    enum alert id;                      /* Alert identifier                                     */
    struct {
        uint32_t ind:8;                 /* Alert index                                  *const* */
        uint32_t period:8;              /* Debounce counter period                              */
        uint32_t counter:8;             /* Debounce counter                                     */
        uint32_t latch:1;               /* True to enable alert latch                   *const* */
        uint32_t clear:1;               /* True to enable alert latch clear             *const* */
        uint32_t state:1;               /* Alert debounce state                                 */
        uint32_t output:1;              /* Alert output state                                   */
        uint32_t group:4;               /* Alert group (see 'enum alert_group')         *const* */
    };
};

/**************************************************************************************************
 * 
 * Private global variables
 * 
 * CAUTION: Elements in 'enum alert' directly index the \e alert_u array. Make sure that the array
 * includes all elements from 'enum alert'!
 * 
 *************************************************************************************************/
static union alert_u alert_u[] = {
    ALERT_INTERNAL,
    ALERT_EXTERNAL,
    ALERT_SYSTEM,
    ALERT_MEMORY,
    ALERT_NFO,
    ALERT_ADC,
    ALERT_WCH,
    ALERT_WCS,
    ALERT_PLL,
    ALERT_FSI_SYNC,
};

/**
 * The \e group field in alert identifier occupies 4 bits (see 'enum alert' definition), which
 * means there are 16 alert groups at most.
 */
static bool alert_g[16] = {
    false, false, false, false,
    false, false, false, false,
    false, false, false, false,
    false, false, false, false
};

/**************************************************************************************************
 * 
 * \brief Decodes alert index from the identifier
 * 
 * \param id alert identifier
 * 
 * \return Alert index
 * 
 *************************************************************************************************/
static inline unsigned
alert_ind(enum alert id)
{
    union alert_u _id = {
        .id = id
    };

    ASSERT(_id.ind < LENGTH(alert_u));
    ASSERT(alert_u[_id.ind].ind == _id.ind)

    return _id.ind;
}

/**************************************************************************************************
 * 
 * \brief Updates alert group status
 * 
 * \param group alert group identifier
 * 
 * \details This function updates alert group status. It also updates \b ALERT_ALL group status.
 * 
 * \return None
 * 
 *************************************************************************************************/
static void
alert_update_group(enum alert_group group)
{
    ASSERT(group < LENGTH(alert_g));

    bool status = false;

    unsigned i;
    for (i = 0U; i < LENGTH(alert_u); i++) {
        if (alert_u[i].group == group) {
            status = status || alert_u[i].output;
        }
    }

    if (alert_g[group] != status) {
        alert_g[group] = status;

        /* Update ALERT_ALL flag */
        alert_g[0] = false;
        for (i = 0U; i < LENGTH(alert_g); i++) {
            alert_g[0] = alert_g[0] || alert_g[i];
        }
    }
}

/**************************************************************************************************
 * 
 * alert_set()
 * 
 *************************************************************************************************/
void
alert_set(enum alert id, bool cond)
{
    unsigned ind = alert_ind(id);

    /* Update debounce counter */
    if (alert_u[ind].state == cond) { /** TODO: Possible pitfail since bool is not native type! */
        if (alert_u[ind].counter < alert_u[ind].period) {
            alert_u[ind].counter++;
        }
    } else { /* state != cond */
        alert_u[ind].counter = 0U;
    }

    alert_u[ind].state = cond;

    /* Must not update alert output while debounce counter is still running */
    if (alert_u[ind].counter < alert_u[ind].period) {
        return;
    }

    /* Latch alerts can be only set, while non-latch alerts can be both set and clear */
    if (cond) {
        if (!alert_u[ind].output) {
            alert_u[ind].output = true;
            alert_update_group((enum alert_group) alert_u[ind].group);
        }
    } else if (!alert_u[ind].latch) {
        if (alert_u[ind].output) {
            alert_u[ind].output = false;
            alert_update_group((enum alert_group) alert_u[ind].group);
        }
    }
}

/**************************************************************************************************
 * 
 * alert_get()
 * 
 *************************************************************************************************/
bool
alert_get(enum alert id)
{
    unsigned ind = alert_ind(id);

    return alert_u[ind].output;
}

/**************************************************************************************************
 * 
 * alert_get_group()
 * 
 *************************************************************************************************/
bool
alert_get_group(enum alert_group group)
{
    ASSERT(group < LENGTH(alert_g));

    return alert_g[group];
}

/**************************************************************************************************
 * 
 * alert_clear()
 * 
 *************************************************************************************************/
void
alert_clear(enum alert id)
{
    unsigned ind = alert_ind(id);

    /* The alert should be cleared if it is not latched or if latch clear is enabled */
    if (!alert_u[ind].latch || alert_u[ind].clear) {
        alert_u[ind].counter = 0U;
        alert_u[ind].state = false;
        if (alert_u[ind].output) {
            alert_u[ind].output = false;
            alert_update_group((enum alert_group) alert_u[ind].group);
        }
    }
}

/**************************************************************************************************
 * 
 * alert_reset()
 * 
 *************************************************************************************************/
void
alert_reset(void)
{
    unsigned i;
    for (i = 0U; i < LENGTH(alert_u); i++) {
        alert_clear(alert_u[i].id);
    }
}

/**************************************************************************************************
 * 
 * alert_period()
 * 
 *************************************************************************************************/
void
alert_period(enum alert id, unsigned period)
{
    ASSERT(period < 256U);

    unsigned ind = alert_ind(id);

    alert_u[ind].period = period;
}
