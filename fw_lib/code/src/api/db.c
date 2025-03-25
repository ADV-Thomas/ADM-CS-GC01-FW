/**************************************************************************************************
 * 
 * \file db.c
 * 
 * \brief CAN database abstraction implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "inc/api/db.h"

#include "inc/lib/debug.h"
#include "inc/net/can.h"

/**************************************************************************************************
 * 
 * Generic CAN database object definition
 * 
 *************************************************************************************************/
OBJ_STRUCT(db, db_priv,
    DB_OBJ_STRUCT_MEMBER(msg);
);

/**************************************************************************************************
 * 
 * \brief Acceptance filter for CAN consumer messages
 * 
 * \param flt CAN identifier handler
 * \param id received message identifier
 * 
 * \note Message should not be accepted if filter is not defined.
 * 
 * \return \b true if message should be accepted; \b false otherwise
 * 
 *************************************************************************************************/
static inline bool
filter(const struct db_id *flt, uint32_t id)
{
    return (flt ? (((id ^ flt->id) & flt->mask) == 0U) : false);
}

/**************************************************************************************************
 * 
 * db_handle_exceptions()
 * 
 *  This function calls the 'handle exceptions' pointer if it exists. This is to allow apps
 *  to add exceptions to message filtering. 
 * 
 *  Returning a 'false' will continue normal filtering
 * 
 *  Returning a 'true' will allow processing the message
 * 
 *************************************************************************************************/
bool db_handle_exceptions(const struct db_priv *db_priv, struct can_f *f)
{
    if(db_priv->exception_filter == NULL){
        return false;
    }
    else{
        return db_priv->exception_filter(db_priv,f);
    }
}

/**************************************************************************************************
 * 
 * db_subscribe()
 * 
 *************************************************************************************************/
int
db_subscribe(const struct db *db, enum nfo_id device, uint16_t address, uint32_t mask)
{
    ASSERT(obj_check(db, OBJ_DB));

    db_unsubscribe(db);

    struct db_priv *priv = db->priv;

    if ((priv->mask & mask) != 0U) {
        return -1;
    }

    db_id_set(&priv->_id, device, address, mask);

    return 0;
}

/**************************************************************************************************
 * 
 * db_unsubscribe()
 * 
 *************************************************************************************************/
int
db_unsubscribe(const struct db *db)
{
    ASSERT(obj_check(can, OBJ_DB));

    struct db_priv *priv = db->priv;

    db_id_reset(&priv->_id);

    return 0;
}

/**************************************************************************************************
 * 
 * db_run()
 * 
 *************************************************************************************************/
int
db_run(const struct net *can, const struct db * const *db, uint16_t size)
{
    int ret;
    struct can_f f;

    unsigned i, j;

    /* Consumer CAN messages */
    while ((ret = can_read(can, &f)) == 1) {
        for (i = 0U; i < size; i++) {
            ASSERT(obj_check(db[i], OBJ_DB));

            struct db_priv *priv = db[i]->priv;
            struct db_msg * const *msg = &db[i]->msg;

            ASSERT(priv->tlo);

            if (!priv->enable) {
                continue;
            }

            if(db_handle_exceptions(priv,&f)){
                /* Message is an exception and has been processed */
                continue;
            }

            /* Loop through all the messages in the database looking for a match */
            for (j = 0U; j < priv->size; j++) {
                ASSERT(msg[j] && msg[j]->msg && msg[j]->unpack && msg[j]->callback);

                if (msg[j]->type || !msg[j]->enable) {
                    continue;
                }

                struct db_id _id = {
                    .mask = priv->mask, /* 0x000FFFFF */
                    .id = msg[j]->id
                };

                // The following condition just checks if the received register address
                // matches this message register (also that the received length matches)
                if (!filter(&_id, f.id) || f.length != msg[j]->size) {
                    continue;
                }

                // The following condition checks that the received message is intended
                // for this type of device and with this device stack id
                if (!((msg[j]->global && filter(&priv->_id, f.id)) || filter(msg[j]->_id, f.id))) {
                    continue;
                }

                ret = msg[j]->unpack(msg[j]->msg, (uint8_t *) f.data, f.length);
                ASSERT(ret >= 0);
             
                msg[j]->callback(priv->tlo, msg[j]->msg);
            }
        }
    }

    /* Producer CAN messages */
    for (i = 0U; i < size; i++) {
        ASSERT(obj_check(db[i], OBJ_DB));

        struct db_priv *priv = db[i]->priv;
        struct db_msg * const *msg = &db[i]->msg;

        ASSERT(priv->tlo);

        if (!priv->enable) {
            continue;
        }

        if( msg[j]->periodic){
            if (++priv->counter >= priv->period) {
                priv->counter = 0U;
            }
        }

        for (j = 0U; j < priv->size; j++) {
            ASSERT(msg[j] && msg[j]->msg && msg[j]->pack);

            if (!msg[j]->type || !msg[j]->enable) {
                continue;
            }

            /* Proceed only if counter matches period or if a oneshot message was requested */
            if ( priv->counter % msg[j]->period != msg[j]->offset && !msg[j]->oneshot) {
                continue;
            }

            if( !msg[j]->periodic && !msg[j]->oneshot){
                continue;
            }
            msg[j]->oneshot = false;

            f.id = msg[j]->id & priv->mask;
            f.length = msg[j]->size;

            const struct db_id *_id = msg[j]->_id;
            if (!_id && msg[j]->global) {
                _id = &priv->_id;
            }

            if (_id) {
                f.id &= ~_id->mask;
                f.id |= _id->id & _id->mask;
            }

            struct db_mux *mux;
            if (mux = msg[j]->mux) {
                ASSERT(mux->list && mux->count > 0U);
                
                if (++mux->ind >= mux->count) {
                    mux->ind = 0U;
                }

                mux->callback(msg[j]->msg, mux->list[mux->ind]);
            }

            if (msg[j]->callback) {
                msg[j]->callback(priv->tlo, msg[j]->msg);
            }

            ret = msg[j]->pack((uint8_t *) f.data, msg[j]->msg, f.length);
            ASSERT(ret >= 0);

            ret = can_write(can, &f);
            if(ret == 0){
                /* The message could not be sent because all CAN mailboxes are full.
                Therefore, we enable oneshot so that we retry sending the message in
                the next iteration */
                msg[j]->oneshot = true;
                return (int)(f.id & 0x0000FFFF);
            }
            if (ret != 1) {
                return -1;
            }
        }
    }

    return 0;
}

void db_add_exception_filter(bool (*exception_filter)(const struct db_priv *db_priv,const struct can_f *f), const struct db *db)
{
    db->priv->exception_filter = exception_filter;
}
