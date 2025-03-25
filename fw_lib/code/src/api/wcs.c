/**************************************************************************************************
 * 
 * \file wcs.c
 * 
 * \brief WCS abstraction implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "inc/api/wcs.h"

#include "inc/hal/hapi.h"
#include "inc/lib/conv.h"

/**************************************************************************************************
 * 
 * Comparator type
 * 
 *************************************************************************************************/
enum wcs_type {
    WCS_TYPE_LOW = 0,
    WCS_TYPE_HIGH
};

/**************************************************************************************************
 * 
 * \brief Updates normalized comparator reference
 * 
 * \param conv conversion object handler
 * \param cmp comparator object handler
 * \param type comparator type
 * 
 * \return None
 * 
 *************************************************************************************************/
static void
wcs_update(const struct conv *conv, struct wcs_cmp *cmp, enum wcs_type type)
{
    struct wcs_ref *sys = &cmp->sys;
    struct wcs_ref *usr = &cmp->usr;

    sys->norm = conv_p2n(conv, sys->phy);
    usr->norm = conv_p2n(conv, usr->phy);

    /* Initialize active reference to more restrictive reference set */
    if (type == WCS_TYPE_LOW && (usr->phy > sys->phy)) {
        cmp->ref = usr;
    } else if (type == WCS_TYPE_HIGH && (usr->phy < sys->phy)) {
        cmp->ref = usr;
    } else {
        cmp->ref = sys;
    }
}

/**************************************************************************************************
 * 
 * \brief Resets comparator reference to system-defined values
 * 
 * \param cmp comparator object handler
 * 
 * \return None
 * 
 *************************************************************************************************/
static void
wcs_reset(struct wcs_cmp *cmp)
{
    cmp->ref = &cmp->sys;

    /* Overwrite user-defined references */
    cmp->usr = cmp->sys;
}

/**************************************************************************************************
 * 
 * \brief Clears fault flags for both low- and high-comparator
 * 
 * \param var WCS variable handler
 * 
 * \return None
 * 
 *************************************************************************************************/
static void
wcs_clear(struct wcs_var *var)
{
    var->cmpl.fault = false;
    var->cmph.fault = false;
    var->fault = false;
}

/**************************************************************************************************
 * 
 * \brief Updates comparator output
 * 
 * \param var WCS variable handler
 * 
 * \return \b true when there is a fault; \b false otherwise
 * 
 *************************************************************************************************/
static bool
wcs_status(struct wcs_var *var)
{
    bool fault;
    struct wcs_cmp *cmp;

    cmp = &var->cmpl;
    fault = *var->meas < cmp->ref->norm;
    cmp->fault |= (cmp->enable && (!cmp->invert ? fault : !fault));

    cmp = &var->cmph;
    fault = *var->meas > cmp->ref->norm;
    cmp->fault |= (cmp->enable && (!cmp->invert ? fault : !fault));

    var->fault |= var->cmpl.fault || var->cmph.fault;

    return (var->cmpl.fault || var->cmph.fault);
}

/**************************************************************************************************
 * 
 * wcs_init()
 * 
 *************************************************************************************************/
int
wcs_init(const struct wcs *wcs)
{
    ASSERT(obj_check(wcs, OBJ_WCS));

    int ret;

    void **_wcs = (void **) wcs;
    struct wcs_priv *priv = (struct wcs_priv *) _wcs[0];

    priv->init = false;

    ret = hapi_wcs_data(wcs);
    if (ret < 0) {
        return -1;
    }
    
    wcs_run(wcs, WCS_OP_RESET);
    wcs_run(wcs, WCS_OP_UPDATE);

    return 0;
}

/**************************************************************************************************
 * 
 * wcs_run()
 * 
 *************************************************************************************************/
bool
wcs_run(const struct wcs *wcs, enum wcs_op op)
{
    ASSERT(obj_check(wcs, OBJ_WCS));

    bool fault = false;

    void **_wcs = (void **) wcs;
    struct wcs_priv *priv = (struct wcs_priv *) _wcs[0];
    struct wcs_var **var = (struct wcs_var **) &_wcs[1];

    ASSERT(priv->init);

    unsigned i;
    for (i = 0U; i < priv->size; i++) {
        switch (op) {
        case WCS_OP_UPDATE:
            wcs_update(var[i]->conv, &var[i]->cmpl, WCS_TYPE_LOW);
            wcs_update(var[i]->conv, &var[i]->cmph, WCS_TYPE_HIGH);
            break;
        case WCS_OP_RESET:
            wcs_reset(&var[i]->cmpl);
            wcs_reset(&var[i]->cmph);
            /* Intentional fall through */
        case WCS_OP_CLEAR:
            wcs_clear(var[i]);
            break;
        case WCS_OP_STATUS:
            fault |= wcs_status(var[i]);
            break;
        default:
            return true;
        }
    }

    return fault;
}
