/**************************************************************************************************
 * 
 * \file fsi.h
 * 
 * \brief FSI database abstraction interface
 * 
 * \author Jorge Sola
 * 
 *************************************************************************************************/

#ifndef _INC_API_FSI_H
#define _INC_API_FSI_H

#include "cgen/api/fsi.h"
#include "inc/api/obj.h"
#include "inc/lib/nfo.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**************************************************************************************************
 * 
 * Forward declarations
 * 
 *************************************************************************************************/

struct fsi;
struct net;
struct tlo;
struct fsi_f;



/**************************************************************************************************
 * 
 * Private variables definition
 * 
 *************************************************************************************************/
OBJ_PRIV_STRUCT(fsi_priv,
    const struct tlo *tlo;          /* Top-level object handler                                 */
    bool enable;                    /* Enable or disable all CAN messages                       */
    uint32_t counter;               /* Counter in 1 ms time base                                */
);

/**************************************************************************************************
 * 
 * CAN message definition
 * 
 *************************************************************************************************/
struct fsi_msg {
    bool type;                      /* true for producer, false for consumer CAN messages       */
    bool enable;                    /* Enable or disable CAN message                            */
    const uint32_t id;              /* Extended message identifier (29 bits)                    */
    const uint16_t size;            /* Message size (number of bytes)                           */

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

int
fsi_db_run(const struct net *fsi_net, const struct fsi *fsi_db);

#endif /* _INC_API_FSI_H */
