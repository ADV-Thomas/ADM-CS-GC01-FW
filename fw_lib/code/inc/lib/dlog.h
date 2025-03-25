/**************************************************************************************************
 * 
 * \file dlog.c
 * 
 * \brief DLOG implementation
 * 
 * \author Jorge Sola
 * 
 *************************************************************************************************/

#ifndef _INC_LIB_DLOG_H
#define _INC_LIB_DLOG_H

#include "IQmathLib.h"

#include <stdint.h>
#include <stdbool.h>

#define DLOG_MAX_CHANNEL_NUMBER (5)

struct dlog_db;
struct dlog;

struct dlog_usr{
    uint16_t num_channels;
    float *pInput[DLOG_MAX_CHANNEL_NUMBER];
};

/**************************************************************************************************
 * 
 * \brief Creates and initializes the dlog object
 * 
 * \param ch Pointer to channel group struct
 * \param can Pointer to CAN net struct
 * 
 * \details This function creates and initialises the DLOG object parameters.
 * 
 * \return Pointer to DLOG struct
 * 
 *************************************************************************************************/
struct dlog *dlog_init(struct dlog_usr *dlog_usr, struct dlog_db *db);

/**************************************************************************************************
 * 
 * \brief Runs DLOG
 * 
 * \param self Pointer to DLOG struct
 * \param ext_trigger External trigger
 * 
 * \details Calling this function runs the main DLOG state machine. This is tipically called 
 * from the main ISR.
 * 
 *************************************************************************************************/
void dlog_run(struct dlog *self, bool ext_trigger);

#endif /* _INC_LIB_DLOG_H */
