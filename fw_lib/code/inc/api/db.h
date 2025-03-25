/**************************************************************************************************
 * 
 * \file db.h
 * 
 * \brief CAN database abstraction interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_API_DB_H
#define _INC_API_DB_H

#include "cgen/api/db.h"
#include "inc/api/obj.h"
#include "inc/lib/nfo.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**************************************************************************************************
 * 
 * Defines for the CAN message identifier
 * 
 *************************************************************************************************/

#define DB_ID_MSG_S         0U
#define DB_ID_MSG_M         0x0000FFFFUL

#define DB_ID_DEV_S         16U
#define DB_ID_DEV_M         0x00FF0000UL

#define DB_ID_ADR_S         24U
#define DB_ID_ADR_M         0x1F000000UL

#define DB_ID_DEV_ADR_M     (DB_ID_DEV_M | DB_ID_ADR_M)

/**************************************************************************************************
 * 
 * Forward declarations
 * 
 *************************************************************************************************/

struct db;
struct net;
struct tlo;
struct can_f;

/**************************************************************************************************
 * 
 * CAN message identifier definition
 * 
 *************************************************************************************************/
struct db_id {
    uint32_t mask;                  /* Identifier mask (0 means identifier bit is not used)     */
    uint32_t id;                    /* Extended message identifier (29 bits)                    */
};

/**************************************************************************************************
 * 
 * Private variables definition
 * 
 *************************************************************************************************/
OBJ_PRIV_STRUCT(db_priv,
    const struct tlo *tlo;          /* Top-level object handler                                 */
    bool enable;                    /* Enable or disable all CAN messages                       */
    const uint32_t mask;            /* Mask for message identifier acceptance filter            */
    const uint32_t period;          /* Counter period                                           */
    uint32_t counter;               /* Counter in 1 ms time base                                */
    struct db_id _id;               /* Global CAN identifier                                    */
    bool (*exception_filter)(const struct db_priv *db_priv,const struct can_f *f);
);

/**************************************************************************************************
 * 
 * CAN message multiplexer definition
 * 
 *************************************************************************************************/
struct db_mux {
    uint16_t ind;                   /* Multiplex list iterator                                  */
    const uint16_t count;           /* Multiplex list size                                      */
    const uint16_t * const list;    /* List with valid multiplex values                         */

    /**********************************************************************************************
     * 
     * \brief Updates multiplexer control variable
     * 
     * \param msg message object handler
     * \param value control variable value
     * 
     * \return None
     * 
     *********************************************************************************************/
    void (* const callback)(void *msg, uint16_t value);
};

/**************************************************************************************************
 * 
 * CAN message definition
 * 
 *************************************************************************************************/
struct db_msg {
    bool type;                      /* true for producer, false for consumer CAN messages       */
    bool enable;                    /* Enable or disable CAN message                            */
    bool periodic;                  /* Enable or disable periodic call of producer callback     */
    bool global;                    /* Enable or disable global CAN identifier                  */
    bool oneshot;                   /* Enable to send a async. message (cleared automatically)  */
    const uint32_t id;              /* Extended message identifier (29 bits)                    */
    const uint16_t size;            /* Message size (number of bytes)                           */
    const uint16_t period;          /* Message period in 1 ms time base                         */
    const uint16_t offset;          /* Message (random) time offset                             */
    struct db_mux * const mux;      /* Multiplex object handler                                 */
    const struct db_id *_id;        /* Local CAN identifier handler                             */
    void * const msg;               /* Message object handler                                   */

    /**********************************************************************************************
     * 
     * \brief Packs message data
     * 
     * \param data raw data array
     * \param msg message object handler
     * \param size number of bytes in raw data array
     * 
     * \details This function is initialized to a function from \b auto-generated CAN database.
     * 
     * \return number of packed bytes if operation is successful; negative value otherwise
     * 
     *********************************************************************************************/
    int (* const pack)(uint8_t *data, void *msg, uint16_t size);

    /**********************************************************************************************
     * 
     * \brief Unpacks message data
     * 
     * \param msg message object handler
     * \param data raw data array
     * \param size number of bytes in raw data array
     * 
     * \details This function is called if a message passes acceptance filter. It is initialized
     * to a function from \b auto-generated CAN database.
     * 
     * \return 0 if operation is successful; negative value otherwise
     * 
     *********************************************************************************************/
    int (* const unpack)(void *msg, uint8_t *data, uint16_t size);

    /**********************************************************************************************
     * 
     * \brief Message callback
     * 
     * \param tlo top-level object handler
     * \param msg message object handler
     * 
     * \return None
     * 
     *********************************************************************************************/
    void (*callback)(const struct tlo *tlo, void *msg);
};

/**************************************************************************************************
 * 
 * \brief Sets CAN message identifier
 * 
 * \param id CAN identifier handler
 * \param device device identifier
 * \param address device address
 * \param mask identifier mask
 * 
 * \details This function sets CAN message identifier used as acceptance filter in CAN consumer
 * messages or as identifier insertion in CAN producer messages. The \e mask parameter defines
 * which identifier bits are relevant, and is formed as logical OR between \b DB_ID_DEV_M and
 * \b DB_ID_ADR_M.
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
db_id_set(struct db_id *id, enum nfo_id device, uint16_t address, uint32_t mask)
{
    id->id = 0U;

    id->id |= ((uint32_t) device << DB_ID_DEV_S) & DB_ID_DEV_M;
    id->id |= ((uint32_t) address << DB_ID_ADR_S) & DB_ID_ADR_M;

    id->mask = mask & (DB_ID_DEV_M | DB_ID_ADR_M);
}

/**************************************************************************************************
 * 
 * \brief Resets CAN message identifier
 * 
 * \param id CAN identifier handler
 * 
 * \return None
 * 
 *************************************************************************************************/
static inline void
db_id_reset(struct db_id *id)
{
    id->id = 0U;
    id->mask = 0U;
}

/**************************************************************************************************
 * 
 * \brief Subscribes CAN database object to a device
 * 
 * \param db CAN database object handler
 * \param device device identifier
 * \param address device address
 * \param mask identifier mask
 * 
 * \details This function subscribes CAN database object to a device specified via identifier and
 * address. The \e mask parameter defines which identifier bits are relevant and is formed as
 * logical OR between \b DB_ID_DEV_M and \b DB_ID_ADR_M. If message identifier already contains
 * information about device identifier, the \b DB_ID_DEV_M must not be used in \e mask parameter.
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
db_subscribe(const struct db *db, enum nfo_id device, uint16_t address, uint32_t mask);

/**************************************************************************************************
 * 
 * \brief Unsubscribes CAN database object from a device
 * 
 * \param db CAN database object handler
 * 
 * \return 0
 * 
 *************************************************************************************************/
extern int
db_unsubscribe(const struct db *db);

/**************************************************************************************************
 * 
 * \brief Processes CAN database objects
 * 
 * \param can CAN driver handler
 * \param db array of CAN database object handlers
 * \param size array size
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
db_run(const struct net *can, const struct db * const *db, uint16_t size);


/**************************************************************************************************
 * 
 * \brief Adds an exception filter
 * 
 * \param exception_filter Pointer to exception filter function
 * \param db Pointer to dabase
 * 
 *************************************************************************************************/
void db_add_exception_filter(bool (*exception_filter)(const struct db_priv *db_priv,const struct can_f *f), const struct db *db);

#endif /* _INC_API_DB_H */
