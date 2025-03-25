/**************************************************************************************************
 * 
 * \file fsi.c
 * 
 * \brief FSI database abstraction implementation
 * 
 * \author Jorge Sola
 * 
 *************************************************************************************************/

#include "inc/api/fsi.h"

#include "inc/lib/debug.h"
#include "inc/net/fsi.h"
#include "inc/lib/crc.h"

#define CRC_BUFFER_POSITION 15
#define ID_BUFFER_POSITION  14
#define DATA_BUFFER_SIZE    15
/**************************************************************************************************
 * 
 * Generic CAN database object definition
 * 
 *************************************************************************************************/
OBJ_STRUCT(fsi, fsi_priv,
    FSI_OBJ_STRUCT_MEMBER(msg);
);

/**************************************************************************************************
 * 
 * db_run()
 * 
 *************************************************************************************************/
int
fsi_db_run(const struct net *fsi_net, const struct fsi *fsi_db)
{
    int ret;
    struct fsi_f f;

    int j = 0;
    int i = 0;
    uint16_t recieved_crc = 0;

    struct fsi_priv *priv = fsi_db->priv;
    struct fsi_msg * const *msg = &fsi_db->msg;

    ASSERT(obj_check(fsi_db, OBJ_FSI));
    ASSERT(priv->tlo);

    /**********************************************************************************************
    * 
    * PRODUCER MESSAGES
    * 
    * These are sent in a round robin fashion (i.e, one after the other in a loop fashion)
    * 
    **********************************************************************************************/

    /* Loop around to look for the next producer message in the database */
    priv->counter++;
    if(priv->counter >= priv->size){
        priv->counter = 0;
    }
    for(i=0; i < priv->size; i++){
        j = priv->counter+i;

        if(j >= priv->size){
            j = 0;
        }

        if(msg[j]->type && msg[j]->enable){
            // Producer message found
            priv->counter = j;

            f.id = msg[j]->id;
            f.length = msg[j]->size;

            if (msg[j]->callback) {
                msg[j]->callback(priv->tlo, (void *)&f);
            }
            f.data[ID_BUFFER_POSITION] = msg[j]->id;
            f.data[CRC_BUFFER_POSITION] = crc_calculate(f.data,DATA_BUFFER_SIZE);
            ret = fsi_write(fsi_net, &f);
            break;
        }
        else{
            // Look for next producer message
        }
    }

    /**********************************************************************************************
    *
    * CONSUMER MESSAGES
    *
    * These are received in a round robin fashion (i.e, one after the other in a loop fashion)
    *
    **********************************************************************************************/
    if((ret = fsi_read(fsi_net, &f)) == 1) {

        /* Loop through all the messages in the database looking for a match */
        for (j = 0U; j < priv->size; j++) {
            ASSERT(msg[j] && msg->callback);

            /* Verify that message is consumer type */
            if (msg[j]->type || !msg[j]->enable) {
                continue;
            }

            /* Check that the received frame id matches the database message id*/
            if (f.id != msg[j]->id) {
                continue;
            }

            recieved_crc = crc_calculate(f.data,DATA_BUFFER_SIZE);
            if((f.data[ID_BUFFER_POSITION] == msg[j]->id) && (f.data[CRC_BUFFER_POSITION] == recieved_crc)) {
            msg[j]->callback(priv->tlo, (void *)&f);
            } else{
                continue;
            }
        }
    }

    if (ret == -1) {
        return -1;
    }

    return 0;
}
