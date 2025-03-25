/**************************************************************************************************
 * 
 * \file db.h
 * 
 * \brief CAN database abstraction code generator
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _CGEN_API_DB_H
#define _CGEN_API_DB_H

#include "cgen/api/obj.h"
#include "inc/lib/debug.h"

#include <string.h>

/**************************************************************************************************
 * 
 * \brief Generates new static private variables structure
 * 
 * \param TYPE corresponding object structure type
 * \param MASK message identifier mask
 * \param PERIOD counter period
 * 
 * \details This function generates private variables structure for CAN database object:
 * - variable type is 'struct db_priv'
 * - variable name is 'priv'
 * - object structure type is defined by \e TYPE parameter
 * - object type is 'OBJ_DB'
 * 
 *************************************************************************************************/
#define DB_PRIV_NEW(TYPE, MASK, PERIOD)                                                         \
    OBJ_PRIV_NEW(db_priv, priv, TYPE, OBJ_DB,                                                   \
        .tlo     = NULL,                                                                        \
        .enable  = true,                                                                        \
        .mask    = MASK,                                                                        \
        .period  = PERIOD,                                                                      \
        .counter = 0U,                                                                          \
        ._id = {                                                                                \
            .mask = 0U,                                                                         \
            .id   = 0U                                                                          \
        } ,                                                                                     \
        .exception_filter = NULL                                                                \
    )

/**************************************************************************************************
 * 
 * \brief Initializes private variables
 * 
 * \param None
 * 
 *************************************************************************************************/
#define DB_PRIV_INIT()                                                                          \
    priv.tlo = tlo

/**************************************************************************************************
 * 
 * \brief Generates new static CAN database object
 * 
 * \param TYPE object structure type
 * \param MASK message identifier mask
 * \param PERIOD counter period
 * \param ... optional initializers
 * 
 * \details This function generates CAN database object with its private variables structure:
 * - object structure type is defined by \e TYPE parameter
 * - object name is 'db'
 * 
 *************************************************************************************************/
#define DB_OBJ_NEW(TYPE, MASK, PERIOD, ...)                                                     \
    DB_PRIV_NEW(TYPE, MASK, PERIOD);                                                            \
    OBJ_NEW(TYPE, db, &priv, __VA_ARGS__)

/**************************************************************************************************
 * 
 * \brief Generates new CAN message object
 * 
 * \param NAME CAN message name
 * \param TYPE message type flag
 * \param ENABLE message enable flag
 * \param ID message identifier
 * \param SIZE message size
 * \param PERIOD period in 1 ms time base
 * \param OFFSET (random) time offset
 * \param MUX multiplexer object handler
 * \param CALLBACK callback function handler
 * 
 *************************************************************************************************/
#define DB_MSG_NEW(NAME, TYPE, ENABLE, ID, SIZE, PERIOD, OFFSET, MUX, PACK, UNPACK, CALLBACK)   \
    static struct NAME##_t NAME##_msg;                                                          \
    memset((void *) &NAME##_msg, 0, sizeof(NAME##_msg));                                        \
    static struct db_msg NAME = {                                                               \
        .type     = TYPE,                                                                       \
        .enable   = ENABLE,                                                                     \
        .periodic = false,                                                                      \
        .global   = true,                                                                       \
        .oneshot  = false,                                                                      \
        .id       = ID,                                                                         \
        .size     = SIZE,                                                                       \
        .period   = PERIOD,                                                                     \
        .offset   = OFFSET,                                                                     \
        .mux      = MUX,                                                                        \
        ._id      = NULL,                                                                       \
        .msg      = (void *) &NAME##_msg,                                                       \
        .pack     = (int (*)(uint8_t *, void *, uint16_t)) PACK,                                \
        .unpack   = (int (*)(void *, uint8_t *, uint16_t)) UNPACK,                              \
        .callback = (void (*)(const struct tlo *, void *)) CALLBACK                             \
    }

/**************************************************************************************************
 * 
 * \brief Generates and assigns new multiplexer object
 * 
 * \param NAME CAN message name
 * \param ... mux values
 * 
 *************************************************************************************************/
#define DB_MUX_NEW(NAME, ...)                                                                   \
    static const uint16_t NAME##_list[] = { __VA_ARGS__ };                                      \
    static struct db_mux NAME##_mux = {                                                         \
        .ind      = 0U,                                                                         \
        .count    = sizeof(NAME##_list) / sizeof(NAME##_list[0]),                               \
        .list     = NAME##_list,                                                                \
        .callback = (void (*)(void *, uint16_t value)) NAME##_mux_callback                      \
    }

/**************************************************************************************************
 * 
 * \brief Generates new CAN consumer message object
 * 
 * \param NAME CAN message name
 * \param ENABLE message enable flag
 * \param ID message identifier
 * \param SIZE message size
 * 
 *************************************************************************************************/
#define DB_MSG_CONS_NEW(NAME, ENABLE, ID, SIZE)                                                 \
    DB_MSG_NEW(NAME, false, ENABLE, ID, SIZE, 0U, 0U, NULL, NULL, NAME##_unpack, NAME##_callback)

/**************************************************************************************************
 * 
 * \brief Generates new CAN producer message object
 * 
 * \param NAME CAN message name
 * \param ENABLE message enable flag
 * \param ID message identifier
 * \param SIZE message size
 * \param PERIOD period in 1 ms time base
 * \param OFFSET (random) time offset
 * 
 *************************************************************************************************/
#define DB_MSG_PROD_NEW(NAME, ENABLE, ID, SIZE, PERIOD, OFFSET)                                 \
    DB_MSG_NEW(NAME, true, ENABLE, ID, SIZE, PERIOD, OFFSET, NULL, NAME##_pack, NULL, NULL)

/**************************************************************************************************
 * 
 * \brief Generates new CAN producer (multiplexed) message object
 * 
 * \param NAME CAN message name
 * \param ENABLE message enable flag
 * \param ID message identifier
 * \param SIZE message size
 * \param PERIOD period in 1 ms time base
 * \param OFFSET (random) time offset
 * \param ... mux values
 * 
 *************************************************************************************************/
#define DB_MSG_PMUX_NEW(NAME, ENABLE, ID, SIZE, PERIOD, OFFSET, ...)                            \
    DB_MUX_NEW(NAME, __VA_ARGS__);                                                              \
    DB_MSG_NEW(NAME, true, ENABLE, ID, SIZE, PERIOD, OFFSET, &NAME##_mux, NAME##_pack, NULL, NULL)

/**************************************************************************************************
 * 
 * \brief Generates CAN database object structure definition
 * 
 * \param TYPE object structure type
 * \param ... optional structure members
 * 
 *************************************************************************************************/
#define DB_OBJ_STRUCT(TYPE, ...)                                                                \
    OBJ_STRUCT(TYPE, db_priv, __VA_ARGS__)

/**************************************************************************************************
 * 
 * \brief Adds message to CAN database object structure definition
 * 
 * \param NAME CAN message name
 * 
 *************************************************************************************************/
#define DB_OBJ_STRUCT_MEMBER(NAME)                                                              \
    OBJ_STRUCT_MEMBER(db_msg, NAME)

/**************************************************************************************************
 * 
 * \brief Generates CAN database object constructor prototype
 * 
 * \param TYPE object structure type
 * 
 *************************************************************************************************/
#define DB_PROTOTYPE_NEW(TYPE)                                                                  \
    const OBJ_PROTOTYPE_NEW(TYPE, const struct tlo *tlo)

/**************************************************************************************************
 * 
 * \brief Generates CAN database object initializer prototype
 * 
 * \param TYPE object structure type
 * 
 *************************************************************************************************/
#define DB_PROTOTYPE_INIT(TYPE)                                                                 \
    int TYPE##_init(const struct TYPE *db, const struct tlo *tlo)

/**************************************************************************************************
 * 
 * \brief Generates CAN message callback prototype
 * 
 * \param NAME CAN message name
 * 
 *************************************************************************************************/
#define DB_PROTOTYPE_CALLBACK(NAME)                                                             \
    void NAME##_callback(const struct tlo *tlo, struct NAME##_t *msg)

/**************************************************************************************************
 * 
 * \brief Sets callback function handler
 * 
 * \param NAME CAN message name
 * 
 *************************************************************************************************/
#define DB_SET_CALLBACK(NAME)                                                                   \
    db->NAME->callback = (void (*)(const struct tlo *, void *)) NAME##_callback

/**************************************************************************************************
 * 
 * \brief Sets callback function handler and disables periodic call
 * 
 * \name NAME CAN message name
 * 
 *************************************************************************************************/
#define DB_SET_APERIODIC_CALLBACK(NAME)                                                         \
    db->NAME->periodic = false;                                                                 \
    DB_SET_CALLBACK(NAME)

/**************************************************************************************************
 * 
 * \brief Sets callback function handler and enables periodic call
 * 
 * \name NAME CAN message name
 * 
 *************************************************************************************************/
#define DB_SET_PERIODIC_CALLBACK(NAME)                                                          \
    db->NAME->periodic = true;                                                                  \
    DB_SET_CALLBACK(NAME)

/**************************************************************************************************
 * 
 * \brief Sends a 'onseshot' CAN message. This message will be sent only once and asynchronously.
 * The flag is cleared automatically when sent. 
 * 
 * \param NAME CAN message name
 * 
 *************************************************************************************************/
#define DB_MSG_SEND_ONESHOT(NAME)                                                                     \
    db->NAME->oneshot = true

/**************************************************************************************************
 * 
 * \brief Enables CAN message
 * 
 * \param NAME CAN message name
 * 
 *************************************************************************************************/
#define DB_MSG_ENABLE(NAME)                                                                     \
    db->NAME->enable = true

/**************************************************************************************************
 * 
 * \brief Disables CAN message
 * 
 * \param NAME CAN message name
 * 
 *************************************************************************************************/
#define DB_MSG_DISABLE(NAME)                                                                    \
    db->NAME->enable = false

/**************************************************************************************************
 * 
 * \brief Calls CAN callback message
 * 
 * \param NAME CAN message name
 * 
 *************************************************************************************************/
#define DB_MSG_CALLBACK(NAME)                                                                   \
    NAME##_callback(tlo, (struct NAME##_t *) db->NAME->msg)

/**************************************************************************************************
 * 
 * \brief Generates multiplex control variable setter
 * 
 * \param NAME CAN message name
 * \param MUX multiplex control variable name
 * 
 *************************************************************************************************/
#define DB_MUX_CALLBACK(NAME, MUX)                                                              \
    static void NAME##_mux_callback(struct NAME##_t *msg, uint16_t value)                       \
    {                                                                                           \
        msg->MUX = value;                                                                       \
    }

/**************************************************************************************************
 * 
 * \brief Generates return statement in CAN database object constructor
 * 
 * \param None
 * 
 *************************************************************************************************/
#define DB_NEW_RETURN()                                                                         \
    return &db

#endif /* _CGEN_API_DB_H */
