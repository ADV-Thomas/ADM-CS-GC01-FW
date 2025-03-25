/**************************************************************************************************
 * 
 * \file wch.c
 * 
 * \brief WCH absctraction implementation
 * 
 * \author Jorge Sola
 * 
 *************************************************************************************************/

#include "inc/api/wch.h"

#include "app/hapi.h"
#include "inc/hal/hapi.h"
#include "inc/lib/alert.h"
#include "inc/lib/data.h"
#include "inc/lib/debug.h"
#include "inc/lib/iir.h"

/**************************************************************************************************
 * 
 * Private functions prototype declarations
 * 
 *************************************************************************************************/

static int wch_test_type(const struct wch *wch, enum wch_type type);
static int wch_calibrate_type(const struct wch *wch, enum wch_type type);
static inline void wch_update(struct wch_cmp *cmp);
static inline void wch_reset(struct wch_cmp *cmp);
static inline void wch_limit(struct wch_cmp *cmp);

/**************************************************************************************************
 * 
 * wch_init()
 * 
 *************************************************************************************************/
int
wch_init(const struct wch *wch, const struct nfo *mod, struct mal *mal)
{
    ASSERT(obj_check(wch, OBJ_WCH));

    int ret;

    void **_wch = (void **) wch;
    struct wch_priv *priv = (struct wch_priv *) _wch[0];
    struct wch_var **var = (struct wch_var **) &_wch[1];

    priv->init = false;
    priv->state = WCH_STATE_START;
    
    ret = hapi_wch_data(wch);
    if (ret < 0) {
        return -1;
    }

    unsigned i;
    for (i = 0U; i < priv->size; i++) {
        ASSERT(var[i] && var[i]->conv);

        /* Initialize comparator type */
        var[i]->cmpl.type = WCH_TYPE_LOW;
        var[i]->cmph.type = WCH_TYPE_HIGH;

        /* Disable comparator test by default */
        var[i]->cmpl.test.enable = false;
        var[i]->cmph.test.enable = false;

        var[i]->cmpl.ref = (struct wch_ref){
            .nrm = 0,
            .phy = 0,
            .raw = 0,
        };
        var[i]->cmph.ref = (struct wch_ref){
            .nrm = 0,
            .phy = 0,
            .raw = 0,
        };
        var[i]->cmpl.test.nrm = 0;
        var[i]->cmpl.test.phy = 0;
        var[i]->cmpl.test.raw = 0;
        var[i]->cmph.test.nrm = 0;
        var[i]->cmph.test.phy = 0;
        var[i]->cmph.test.raw = 0;

    }

    ret = wch_conv(wch, CONV_HAPI);
    if (ret < 0) {
        return -1;
    }

    priv->init = true;

    
    ret = data_wch(mal, wch, mod, CONV_MEM, true);
    if (ret >= 0) {
        //ret |= wch_conv(wch, CONV_MEM);
    }

    wch_calibrate(wch);
    
    

    alert_set(ALERT_WCH, ret < 0);

    return 0;
}

/**************************************************************************************************
 * 
 * wch_conv()
 * 
 *************************************************************************************************/
int
wch_conv(const struct wch *wch, enum conv_src src)
{
    ASSERT(obj_check(wch, OBJ_WCH));

    int ret;
    
    void **_wch = (void **) wch;
    struct wch_priv *priv = (struct wch_priv *) _wch[0];
    struct wch_var **var = (struct wch_var **) &_wch[1];

    unsigned i;
    for (i = 0U; i < priv->size; i++) {
        ret = conv_update(var[i]->cmpl.conv, src);
        ret |= conv_update(var[i]->cmph.conv, src);
        if (ret < 0) {
            return -1;
        }
    }

    return 0;
}

/**************************************************************************************************
 * 
 * wch_run()
 * 
 *************************************************************************************************/
void
wch_run(const struct wch *self, enum wch_op op)
{
    /* Null guard */
    if (self == NULL)
        return;
    
    /* Initialize pointers */
    void **_wch = (void **) self;
    struct wch_priv *priv = (struct wch_priv *) _wch[0];
    struct wch_var **var = (struct wch_var **) &_wch[1];

    /* Loop over all variables */
    int i;
    for (i = 0u; (i < priv->size); i++) {
        // Pointer to i-th variable 
        //struct wch_var *var_i = *(_var_0 + i);
        struct wch_cmp *cmph = &(var[i]->cmph);
        struct wch_cmp *cmpl = &(var[i]->cmpl);

        switch (op) {
        case WCH_OP_UPDATE:
            wch_update(cmpl);
            wch_update(cmph);
            break;

        case WCH_OP_RESET:
            wch_reset(cmpl);
            wch_reset(cmph);
            break;

        case WCH_OP_CLEAR:
            break;

        case WCH_OP_TEST:
            wch_limit(cmpl);
            wch_limit(cmph);
            break;

        case WCH_OP_STATUS:
            break;

        default:
            return;
        }
    }

    switch (op) {
    case WCH_OP_UPDATE:
        /* Set comparator references in HAL */
        hapi_wch_set(self);
        break;

    case WCH_OP_RESET:
        /* Set comparator references in HAL */
        hapi_wch_set(self);
        /* Wait until comparator debouncing filter settles */
        hapi_delay(10U);
        /* Intentional fall through */
    case WCH_OP_CLEAR:
        /* Clear comparator latches */
        hapi_wch_clear(self);
        break;

    case WCH_OP_TEST:
        /* Set comparator references in HAL */
        hapi_wch_set(self);
        /* Wait until comparator debouncing filter settles */
        hapi_delay(10U);
        /* Intentional fall through */
    case WCH_OP_STATUS:
        /* Update fault flags */
        hapi_wch_get(self);
        break;

    default:
        return;
    }
}

int wch_test(const struct wch *self)
{
    /* Null guard */
    if (self == NULL)
        return WCH_RET_NULL;

    /* Return value */
    int ret = 0;

    /* Test low comparators */
    ret |= wch_test_type(self, WCH_TYPE_LOW);

    /* Test high comparators */
    ret |= wch_test_type(self, WCH_TYPE_HIGH);

    /* Reset all comparators to safe state and clear latches */
    wch_run(self, WCH_OP_RESET);

    /* Check if test failed */
    if (ret < 0)
        return WCH_RET_TEST_FAIL;

    /* All comparators have been tested */
    return WCH_RET_SUCCESS;
}

int wch_calibrate(const struct wch *self)
{
    /* Null guard */
    if (self == NULL)
        return WCH_RET_NULL;
    
    /* Initialize pointers */
    struct wch_priv **_priv = (struct wch_priv **) self;
    struct wch_priv *priv = *_priv;
    struct wch_var **_var_0 = (struct wch_var **) (_priv + 1);

    /* Loop iterator */
    unsigned i;

    /* Return value */
    int ret, ret1, ret2;

    /**
     *  STATE MACHINE
     */
    switch (priv->state) {
    /**
     *  ERROR STATE
     */
    case WCH_STATE_ERROR:
        /* Error can be cleared only by power cycling the device */
        return WCH_RET_CAL_ERROR;
    
    /**
     *  INITIALIZE CALIBRATION
     */
    case WCH_STATE_START:
        /* Reset calibration variables */
        for (i = 0u; (i < priv->size); i++) {
            //struct wch_var *var_i = *(_var_0 + i);
            //conv_reset(&var_i->cmpl.conv);
            //conv_reset(&var_i->cmph.conv);
        }
        /* Start calibration routine */
        hapi_wch_calibration_mode(self, true);
        priv->state = WCH_STATE_CALIBRATE;
        /* Intentional fall through */

    /**
     *  CALIBRATE LOW AND HIGH COMPARATORS
     */
    case WCH_STATE_CALIBRATE:
        /* Enable test flag for comparators with enabled calibration objects */
        for (i = 0u; (i < priv->size); i++) {
            struct wch_var *var_i = *(_var_0 + i);
            //var_i->cmpl.test.enable = (bool) (var_i->cmpl.conv.mode != CONV_MODE_DISABLE);
            //var_i->cmph.test.enable = (bool) (var_i->cmph.conv.mode != CONV_MODE_DISABLE);
            var_i->cmpl.test.enable = (bool) (true);
            var_i->cmph.test.enable = (bool) (true);
        }

        /* Detect comparator raw values */
        ret = wch_test(self);
        if (ret < 0)
            return ret;

        /* Calibrate low comparators */
        ret1 = wch_calibrate_type(self, WCH_TYPE_LOW);

        /* Calibrate high comparators */
        ret2 = wch_calibrate_type(self, WCH_TYPE_HIGH);

        /* Check return values */
        if ((ret1 < 0) || (ret2 < 0)) {
            priv->state = WCH_STATE_ERROR;
            return ((ret1 < 0) ? ret1 : ret2);
        }
        /**
         * All calibration points have been initialized, but not all comparators are calibrated.
         * Disable the calling task until user triggers next calibration point.
         */
        else if ((ret1 == WCH_RET_CAL_CONT) || (ret2 == WCH_RET_CAL_CONT))
            return WCH_RET_CAL_CONT;
        
        /* Initialize normalization data and reset WCH object */
        ret = wch_conv(self, CONV_CALIB);

        /* Check return value */
        if (ret < 0) {
            priv->state = WCH_STATE_ERROR;
            return ret;
        }


        /* Reset state machine */
        hapi_wch_calibration_mode(self, false);
        priv->state = WCH_STATE_START;

        /* Calibration successful */
        return WCH_RET_SUCCESS;

    /**
     *  UNEXPECTED STATE
     */
    default:
        priv->state = WCH_STATE_ERROR;
        return WCH_RET_CAL_STATE;
    }
}

static int wch_test_type(const struct wch *self, enum wch_type type)
{
    /* Null guard */
    if (self == NULL)
        return WCH_RET_NULL;
    
    /* Initialize pointers */
    struct wch_priv **_priv = (struct wch_priv **) self;
    struct wch_priv *priv = *_priv;
    struct wch_var **_var_0 = (struct wch_var **) (_priv + 1);

    /* Reset all comparators to safe state and clear latches */
    wch_run(self, WCH_OP_RESET);

    /* Flag to indicate if all comparators have been tested */
    bool done = false;

    /* Loop until all comparators are tested */
    while (!done) {
        /* Test comparator reference and update status flags */
        wch_run(self, WCH_OP_TEST);

        /**
         * Assume that test is done
         *  . This flag is cleared by the for loop below if there is a comparator not tested.
         */
        done = true;

        /* Loop over all comparators */
        unsigned i;
        for (i = 0u; (i < priv->size); i++) {
            /* Pointer to i-th variable */
            struct wch_var *var_i = *(_var_0 + i);

            /* Pointer to i-th comparator */
            struct wch_cmp *cmp_i = NULL;
            switch (type) {
            case WCH_TYPE_LOW:
                cmp_i = &var_i->cmpl;
                break;
            case WCH_TYPE_HIGH:
                cmp_i = &var_i->cmph;
                break;
            default:
                return WCH_RET_TYPE;
            }
            
            /* Skip comparators which should not be tested */
            if (!cmp_i->test.enable)
                continue;
            
            /* Check if comparator has tripped */
            if (cmp_i->fault) {
                /* Latch reference which tripped the comparator */
                cmp_i->test.raw = cmp_i->ref.raw;

                /* Do not test this comparator anymore */
                cmp_i->test.enable = false;

#if 0 /* Think about do we really need this. What data is used for conversion!? */
                /* Update normalized and physical data for diagnostics */
                cmp_i->test.nrm = conv_raw_to_nrm(&cmp_i->conv, cmp_i->test.raw);
                cmp_i->test.phy = conv_nrm_to_phy(&cmp_i->conv, cmp_i->test.nrm);
#endif
                /* Continue with next comparator */
                continue;
            }

            /* Check and update comparator reference */
            switch (cmp_i->type) {
            case WCH_TYPE_LOW:
                /* Check if reference reached boundary value */
                if (cmp_i->ref.raw >= (cmp_i->conv->cop.res - 1u))
                    return WCH_RET_TEST_FAIL;
                /* Reference goes from 0 to conv.bin-1 */
                cmp_i->ref.raw += 1u;
                break;
            case WCH_TYPE_HIGH:
                /* Check if reference reached boundary value */
                if (cmp_i->ref.raw == 0u)
                    return WCH_RET_TEST_FAIL;
                /* Reference goes from conv.bin-1 to 0 */
                cmp_i->ref.raw -= 1u;
                break;
            default:
                /* Unknown comparator type */
                return WCH_RET_TYPE;
            }

            /* There is at least one comparator not tested */
            done = false;
        }
    }

    /* Operation successful */
    return WCH_RET_SUCCESS;
}

static int wch_calibrate_type(const struct wch *self, enum wch_type type)
{
    /* Null guard */
    if (self == NULL)
        return WCH_RET_NULL;

    /* TODO add these return codes to conv.h */
    #define CONV_RET_SUCCESS (0)
    #define CONV_RET_POINT2 (-2)
    
    /* Initialize pointers */
    struct wch_priv **_priv = (struct wch_priv **) self;
    struct wch_priv *priv = *_priv;
    struct wch_var **_var_0 = (struct wch_var **) (_priv + 1);

    /**
     * Flag to indicate if all comparators have been calibrated
     *  . This flag is cleared by the for loop below if there is a comparator not calibrated.
     */
    bool done = true;

    /* Loop over all comparators */
    int i;
    for (i = 0u; (i < priv->size); i++) {
        /* Pointer to i-th variable */
        struct wch_var *var_i = *(_var_0 + i);

        /* Pointer to i-th comparator */
        struct wch_cmp *cmp_i = NULL;
        switch (type) {
        case WCH_TYPE_LOW:
            cmp_i = &var_i->cmpl;
            break;
        case WCH_TYPE_HIGH:
            cmp_i = &var_i->cmph;
            break;
        default:
            return WCH_RET_TYPE;
        }

        cmp_i->conv->calib.offset = cmp_i->test.raw;
        cmp_i->conv->calib.gain_c = 0;
        #if 0
        /**
         * Skip comparators with disabled conversion object
         *  . Conversion object is disabled by default after successful calibration.
         */
        if (cmp_i->conv.mode == CONV_MODE_DISABLE)
            continue;

        struct conv_pt *pt = NULL;

        
        /* Get calibration point #1 */
        if (!cmp_i->conv.pt1.lock)
            pt = &cmp_i->conv.pt1;
        /* Get calibration point #2 */
        else if (!cmp_i->conv.pt2.lock) {
            pt = &cmp_i->conv.pt2;
            /* Calibration point #2 can be used only in two-point calibration mode */
            if (cmp_i->conv.mode != CONV_MODE_TWO_POINT)
                return WCH_RET_CAL_POINT;
        }
        /* Both calibration points have been locked */
        else
            return WCH_RET_CAL_LOCK;

        /* Update and lock calibration point */
        pt->raw = cmp_i->test.raw;
        pt->lock = true;
        

        /* Try to calibrate comparator */
        ret = conv_calibrate(&cmp_i->conv);
        if (ret == CONV_RET_SUCCESS)
            continue;
        else if (ret == CONV_RET_POINT2)
            done = false;
        else
            return WCH_RET_CAL_CONV;
        #endif
    }

    /* Check if there are comparators not calibrated */
    if (!done)
        return WCH_RET_CAL_CONT;
    
    /* All comparators have been calibrated */
    return WCH_RET_SUCCESS;
}


static inline void wch_update(struct wch_cmp *cmp)
{
    if (!cmp->enable) {
        wch_reset(cmp);
        return;
    }
    cmp->ref.nrm = conv_p2n(cmp->conv, cmp->ref.phy);
    cmp->ref.raw = conv_n2r(cmp->conv, cmp->ref.nrm);
    wch_limit(cmp);
}

static inline void wch_reset(struct wch_cmp *cmp)
{
    cmp->ref.raw = (cmp->type == WCH_TYPE_LOW) ? 0u : (cmp->conv->cop.res - 1u);
#if 0
    cmp->ref.nrm = conv_raw_to_nrm(&cmp->conv, cmp->ref.raw);
    cmp->ref.phy = conv_nrm_to_phy(&cmp->conv, cmp->ref.nrm);
#endif
}

static inline void wch_limit(struct wch_cmp *cmp)
{
    if (cmp->ref.raw > (cmp->conv->cop.res - 1u))
        cmp->ref.raw = (cmp->conv->cop.res - 1u);
}

int
wch_set_reference(struct wch_var *wch_var, float upper, float lower)
{
    if(upper < lower){
        return -1;
    }

    wch_var->cmph.ref.phy = upper;
    wch_var->cmpl.ref.phy = lower;

    return 0;
}
