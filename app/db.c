/**************************************************************************************************
 * 
 * \file db.c
 * 
 * \brief Database app layer to define filter exceptions
 * 
 * \author Jorge Sola
 * 
 *************************************************************************************************/

#include "app/db.h"

#include "app/tlo.h"
#include "app/dev_ctl.h"

#include "inc/api/db.h"
#include "adm_cs_fp_db.h"


/** This function handles exceptions to messages that are not in the database
 * (i.e. temperature messages from other modules). This messages are monitored
 * dynamically. Hence, when one arrives, we need to check in the 'supervised
 * nodes list' and look for a match in the ID
 * */
bool handle_db_exceptions(const struct db_priv *db_priv, const struct can_f *f)
{
    /*if(db_priv->tlo == NULL){
        return false;
    }*/

    bool ret;
    ret = dev_ctl_update_devices(db_priv->tlo, f);


    return ret;
    
}



