/**************************************************************************************************
 * 
 * \file fsi.h
 * 
 * \brief FSI database code generator
 * 
 * \author Jorge Sola
 * 
 *************************************************************************************************/

#ifndef _CGEN_API_FSI_H
#define _CGEN_API_FSI_H

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
 * \details This function generates private variables structure for FSI database object:
 * - variable type is 'struct fsi_priv'
 * - variable name is 'priv'
 * - object structure type is defined by \e TYPE parameter
 * - object type is 'OBJ_DB'
 * 
 *************************************************************************************************/
#define FSI_PRIV_NEW(TYPE)                                                         \
    OBJ_PRIV_NEW(fsi_priv, priv, TYPE, OBJ_FSI,                                                   \
        .tlo     = NULL,                                                                        \
        .enable  = true,                                                                        \
        .counter = 0U,                                                                          \
    )

/**************************************************************************************************
 * 
 * \brief Initializes private variables
 * 
 * \param None
 * 
 *************************************************************************************************/
#define FSI_PRIV_INIT()                                                                          \
    priv.tlo = tlo                                                                              

/**************************************************************************************************
 * 
 * \brief Generates new static FSI database object
 * 
 * \param TYPE object structure type
 * \param MASK message identifier mask
 * \param PERIOD counter period
 * \param ... optional initializers
 * 
 * \details This function generates FSI database object with its private variables structure:
 * - object structure type is defined by \e TYPE parameter
 * - object name is 'fsi'
 * 
 *************************************************************************************************/
#define FSI_OBJ_NEW(TYPE, ...)                                                     \
    FSI_PRIV_NEW(TYPE);                                                            \
    OBJ_NEW(TYPE, fsi, &priv, __VA_ARGS__)

/**************************************************************************************************
 * 
 * \brief Generates new FSI message object
 * 
 * \param NAME FSI message name
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
#define FSI_MSG_NEW(NAME, TYPE, ENABLE, ID, SIZE, CALLBACK)   \
    static struct fsi_msg NAME = {                                                               \
        .type     = TYPE,                                                                       \
        .enable   = ENABLE,                                                                     \
        .id       = ID,                                                                         \
        .size     = SIZE,                                                                       \
        .callback = (void (*)(const struct tlo *, void *)) CALLBACK                             \
    }

/**************************************************************************************************
 * 
 * \brief Generates new FSI consumer message object
 * 
 * \param NAME FSI message name
 * \param ENABLE message enable flag
 * \param ID message identifier
 * \param SIZE message size
 * 
 *************************************************************************************************/
#define FSI_MSG_CONS_NEW(NAME, ENABLE, ID, SIZE)                                                 \
    FSI_MSG_NEW(NAME, false, ENABLE, ID, SIZE, NAME##_callback)

/**************************************************************************************************
 * 
 * \brief Generates new FSI producer message object
 * 
 * \param NAME FSI message name
 * \param ENABLE message enable flag
 * \param ID message identifier
 * \param SIZE message size
 * \param PERIOD period in 1 ms time base
 * \param OFFSET (random) time offset
 * 
 *************************************************************************************************/
#define FSI_MSG_PROD_NEW(NAME, ENABLE, ID, SIZE)                                 \
    FSI_MSG_NEW(NAME, true, ENABLE, ID, SIZE, NULL)


/**************************************************************************************************
 * 
 * \brief Generates FSI database object structure definition
 * 
 * \param TYPE object structure type
 * \param ... optional structure members
 * 
 *************************************************************************************************/
#define FSI_OBJ_STRUCT(TYPE, ...)                                                                \
    OBJ_STRUCT(TYPE, fsi_priv, __VA_ARGS__)

/**************************************************************************************************
 * 
 * \brief Adds message to FSI database object structure definition
 * 
 * \param NAME FSI message name
 * 
 *************************************************************************************************/
#define FSI_OBJ_STRUCT_MEMBER(NAME)                                                              \
    OBJ_STRUCT_MEMBER(fsi_msg, NAME)

/**************************************************************************************************
 * 
 * \brief Generates FSI database object constructor prototype
 * 
 * \param TYPE object structure type
 * 
 *************************************************************************************************/
#define FSI_PROTOTYPE_NEW(TYPE)                                                                  \
    const OBJ_PROTOTYPE_NEW(TYPE, const struct tlo *tlo)

/**************************************************************************************************
 * 
 * \brief Generates FSI database object initializer prototype
 * 
 * \param TYPE object structure type
 * 
 *************************************************************************************************/
#define FSI_PROTOTYPE_INIT(TYPE)                                                                 \
    int TYPE##_init(const struct TYPE *fsi, const struct tlo *tlo)

/**************************************************************************************************
 * 
 * \brief Generates FSI message callback prototype
 * 
 * \param NAME FSI message name
 * 
 *************************************************************************************************/
#define FSI_PROTOTYPE_CALLBACK(NAME)                                                             \
    void NAME##_callback(const struct tlo *tlo, struct fsi_f *f)

/**************************************************************************************************
 * 
 * \brief Sets callback function handler
 * 
 * \param NAME FSI message name
 * 
 *************************************************************************************************/
#define FSI_SET_CALLBACK(NAME)                                                                   \
    fsi.NAME->callback = (void (*)(const struct tlo *, void *)) NAME##_callback


/**************************************************************************************************
 * 
 * \brief Enables FSI message
 * 
 * \param NAME FSI message name
 * 
 *************************************************************************************************/
#define FSI_MSG_ENABLE(NAME)                                                                     \
    fsi->NAME->enable = true

/**************************************************************************************************
 * 
 * \brief Disables FSI message
 * 
 * \param NAME FSI message name
 * 
 *************************************************************************************************/
#define FSI_MSG_DISABLE(NAME)                                                                    \
    fsi->NAME->enable = false

/**************************************************************************************************
 * 
 * \brief Calls FSI callback message
 * 
 * \param NAME FSI message name
 * 
 *************************************************************************************************/
#define FSI_MSG_CALLBACK(NAME)                                                                   \
    NAME##_callback(tlo, (struct NAME##_t *) fsi->NAME->msg)


/**************************************************************************************************
 * 
 * \brief Generates return statement in FSI database object constructor
 * 
 * \param None
 * 
 *************************************************************************************************/
#define FSI_NEW_RETURN()                                                                         \
    return &fsi

#endif /* _CGEN_API_FSI_H */
