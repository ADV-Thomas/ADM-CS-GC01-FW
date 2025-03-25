/**************************************************************************************************
 *
 * \file droop.h
 *
 * \brief Droop control module interface
 *
 * \author Jorge Sola
 *
 *************************************************************************************************/

#ifndef _INC_LIB_DROOP_H
#define _INC_LIB_DROOP_H


#include <stdint.h>
#include "IQmathLib.h"

#define C_MAX_GROUP_ID (7u) /* Group ID (between 0 and 7)				*/

struct droop_priv;

struct droop_usr {
    float droop_coeff; /* Droop 'virtual resistance' in [Ohm]      */
    float droop_shift; /* Shift coefficient in (ohms)				*/
    float max_droop;   /* Max allowed droop in V					*/
    float current_fs;  /* Current full scale                       */
    float voltage_fs;  /* Voltage full scale                       */
};

struct droop {
    struct droop_usr usr;
    struct droop_priv *priv;
    uint16_t group_id;
};

/**************************************************************************************************
 *
 * \brief Creates and initializes a droop object
 *
 * \param usr Pointer to a droop_usr struct with the desired parameters
 *
 * \details This function creates and initialises the droop object parameters. It only needs
 * current and voltage full scale values
 *
 * \return Pointer to droop struct
 *
 *************************************************************************************************/
struct droop *droop_new(struct droop_usr *usr);

/**************************************************************************************************
 *
 * \brief Updates node info
 *
 * \param droop Pointer to droop structure
 * \param node_id Stack number of the node that sent the message
 * \param node_current Current of the node (MUST BE in NORMALIZED VALUE [-1,1])
 *
 * \details Called when a new node in the bus sends a droop broadcast message. Updates the node
 * information and refreshes the timeout for that node
 *
 *************************************************************************************************/
void droop_update_node(struct droop *droop, uint16_t node_id, _iq node_current);

/**************************************************************************************************
 *
 * \brief Calculates average bus voltage of all nodes
 *
 * \param droop Pointer to droop structure
 *
 * \details Called every time this device sends a droop message
 *
 *************************************************************************************************/
void droop_avg_current(struct droop *droop);

/**************************************************************************************************
 *
 * \brief Returns a corrected voltage reference
 *
 * \param droop Pointer to droop structure
 * \param v_ref Initial voltage reference
 * \param node_current Measured bus current by the device (MUST BE in NORMALIZED VALUE [-1,1])
 *
 * \details Returns a correted voltage reference based on average bus current and
 * individual node current, implementing bus droop control.
 *
 * \return Corrected voltage reference
 *
 *************************************************************************************************/
_iq droop_run(struct droop *droop, _iq v_ref, _iq node_current);


#endif /* _INC_LIB_DROOP_H */
