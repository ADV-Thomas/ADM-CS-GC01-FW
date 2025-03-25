/**************************************************************************************************
 *
 * \file fsi.h
 *
 * \brief FSI driver interface
 *
 * \author Jorge Sola
 *
 *************************************************************************************************/

#ifndef _INC_DRV_FSI_H
#define _INC_DRV_FSI_H

/**************************************************************************************************
 *
 * Forward declarations
 *
 *************************************************************************************************/

struct net;

enum fsi_state{
    FSI_IDLE = 0,
    FSI_HANDSHAKE_WAIT_TAG_0,
    FSI_HANDSHAKE_WAIT_TAG_1,
    FSI_HANDSHAKE_EXTRA_WAIT,
    FSI_OPERATION,
};

/**************************************************************************************************
 *
 * \brief Creates FSI communication driver
 *
 * \param None
 *
 * \return Communication driver handler; \b NULL in case of failure
 *
 *************************************************************************************************/
extern const struct net *fsi_a_new(void);

/**************************************************************************************************
 *
 * \brief Callback called when the FSI state machine has successfully passed the handshake stage
 * and is in operation state
 *
 * \param net
 *
 *************************************************************************************************/
extern int fsi_operation_callback(const struct net *net);

#endif /* _INC_DRV_FSI_H */
