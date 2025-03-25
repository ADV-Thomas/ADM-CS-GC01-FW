/**************************************************************************************************
 * 
 * \file can.h
 * 
 * \brief CAN communication interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_NET_CAN_H
#define _INC_NET_CAN_H

#include "inc/net/net.h"
#include "inc/drv/io.h"

#include <stdint.h>
#include <stdbool.h>

/**************************************************************************************************
 * 
 * User configuration data definition
 * 
 *************************************************************************************************/
struct can_usr {
    uint32_t bitrate;           /* Communication speed (bps)                                    */
    uint32_t id;                /* Acceptance filter identifier for receive messages            */
    uint32_t mask;              /* Acceptance filter identifier mask for receive messages       */
};

/**************************************************************************************************
 * 
 * Transaction frame definition
 * 
 *************************************************************************************************/
struct can_f {
    uint32_t id;                /* Frame identifier                                             */
    uint16_t length;            /* Data buffer length                                           */
    uint8_t data[8];            /* Data buffer                                                  */
};

/**************************************************************************************************
 * 
 * Mailbox configuration data definition
 * 
 * Mailbox setup bit mask decides which mailboxes to setup:
 * - 0x000FFFFF         Regular receive mailboxes. Acceptance filter ID and mask can be used.
 * - 0x3FF00000         Regular transmit mailboxes
 * - 0x40000000         Low-latency receive mailbox. Acceptance filter ID and mask can be used.
 * - 0x80000000         Low-latency transmit mailbox. CAN frame can be used to setup a message.
 * 
 * Acceptance filter identifier mask decides which identifier bits must match in order to accept
 * received message: 0 in mask means that bits are not compared, 1 means they must match.
 * 
 *************************************************************************************************/
struct can_cfg {
    uint32_t mbx;               /* Mailbox setup bit mask                                       */
    bool enable;                /* Enable or disable mailbox                                    */
    uint32_t id;                /* Acceptance filter identifier for receive messages            */
    uint32_t mask;              /* Acceptance filter identifier mask for receive messages       */
    struct can_f *f;            /* CAN frame to setup low-latency transmit message              */
};

/**************************************************************************************************
 * 
 * CAN bit timing configuration parameters
 * 
 *************************************************************************************************/
struct can_btp {
    uint16_t nq;                /* Number of quants within a single bit                         */
    uint16_t brp;               /* Baud rate prescaler to generate bit quants                   */
    uint16_t tProp;             /* Propagation time segment                                     */
    uint16_t tSeg1;             /* Time segment before the sample point                         */
    uint16_t tSeg2;             /* Time segment after the sample point                          */
    uint16_t sjw;               /* Synchronization jump width                                   */
    uint16_t sp;                /* Sample point position (%)                                    */
    uint16_t err;               /* Absolute bit rate error (%)                                  */
};

/**************************************************************************************************
 * 
 * CAN bit timing configuration parameters limit values in format [min,max]
 * 
 *************************************************************************************************/
struct can_btl {
    uint16_t nq[2];             /* Number of quants                                             */
    uint16_t brp[2];            /* Baud rate prescaler                                          */
    uint16_t tSeg1[2];          /* Time segment before the sample point                         */
    uint16_t tSeg2[2];          /* Time segment after the sample point                          */
    uint16_t sjw[2];            /* Synchronization jump width                                   */
};

/**************************************************************************************************
 * 
 * CAN object definition
 * 
 *************************************************************************************************/
struct can {
    enum io io[2];              /* CAN transmit and receive I/O pin configuration code          */
    enum io_fun exp[2];         /* Expected peripheral function code                            */
    int (*setup)(const struct net *net, const struct can_cfg *cfg);
    int (*get)(const struct net *net, struct can_f *f);
    int (*send)(const struct net *net, struct can_f *f);
    int (*status)(const struct net *net);
};

/**************************************************************************************************
 * 
 * Private function prototypes. Functions are implemented in driver and linked to \b net object.
 * 
 *************************************************************************************************/

static int can__open(const struct net *net, const struct can_usr *usr);
static int can__close(const struct net *net);
static int can__read(const struct net *net, struct can_f *f, unsigned length);
static int can__write(const struct net *net, struct can_f *f, unsigned length);

/**************************************************************************************************
 * 
 * Private function prototypes. Functions are implemented in driver and linked to \b can object.
 * 
 *************************************************************************************************/

static int can__setup(const struct net *net, const struct can_cfg *cfg);
static int can__get(const struct net *net, struct can_f *f);
static int can__send(const struct net *net, struct can_f *f);
static int can__status(const struct net *net);

/**************************************************************************************************
 * 
 * \brief Wrapper for \e net_open() function
 * 
 *************************************************************************************************/
extern int
can_open(const struct net *net, const struct can_usr *usr);

/**************************************************************************************************
 * 
 * \brief Wrapper for \e net_close() function
 * 
 *************************************************************************************************/
extern int
can_close(const struct net *net);

/**************************************************************************************************
 * 
 * \brief Wrapper for \e net_read() function
 * 
 *************************************************************************************************/
extern int
can_read(const struct net *net, struct can_f *f);

/**************************************************************************************************
 * 
 * \brief Wrapper for \e net_write() function
 * 
 *************************************************************************************************/
extern int
can_write(const struct net *net, struct can_f *f);

/**************************************************************************************************
 * 
 * \brief Wrapper for \e net_connect() function
 * 
 * \param tx transmit I/O pin configuration code
 * \param rx receive I/O pin configuration code
 * 
 *************************************************************************************************/
extern int
can_connect(const struct net *net, enum io tx, enum io rx);

/**************************************************************************************************
 * 
 * \brief Wrapper for \e net_disconnect() function
 * 
 *************************************************************************************************/
extern int
can_disconnect(const struct net *net);

/**************************************************************************************************
 * 
 * \brief Setups regular or low-latency receive message mailbox
 * 
 * \param net net object handler
 * \param cfg mailbox configuration data
 * 
 * \details Wrapper for \e can->setup() function. The function checks if:
 * - net object is of CAN type,
 * - driver has been opened,
 * - \e can->setup() has been set, and
 * - mailbox configuration data given by \e cfg parameter is not \b NULL.
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
can_setup(const struct net *net, const struct can_cfg *cfg);

/**************************************************************************************************
 * 
 * \brief Gets message from low-latency receive mailbox
 * 
 * \param net net object handler
 * \param f transaction frame
 * 
 * \details Wrapper for \e can_get() function. The function checks if:
 * - net object is of CAN type,
 * - driver has been opened, and
 * - \e can->get() has been set.
 * 
 * Note that transaction frame given by \e f parameter can be \b NULL. In that case, a message is
 * received from RAM but it is not returned to caller.
 * 
 * \return 1 if message is received; 0 if there is no message; -1 if operation failed
 * 
 *************************************************************************************************/
extern int
can_get(const struct net *net, struct can_f *f);

/**************************************************************************************************
 * 
 * \brief Sends low-latency message on the bus
 * 
 * \param net net object handler
 * \param f transaction frame
 * 
 * \details Wrapper for \e can->send() function. The function checks if:
 * - net object is of CAN type,
 * - driver has been opened, and
 * - \e can->send() has been set.
 * 
 * Note that transaction frame given by \e f parameter can be \b NULL. In that case, a message is
 * sent with extended identifier only, ie data length is \b 0.
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
can_send(const struct net *net, struct can_f *f);

/**************************************************************************************************
 * 
 * \brief Checks status of the low-latency transmit mailbox
 * 
 * \param net net object handler
 * 
 * \details Wrapper for \e can->status() function. The function checks if:
 * - net object is of CAN type,
 * - driver has been opened, and
 * - \e can->status() has been set.
 * 
 * This function returns status of the low-latency transmit mailbox:
 * - -1 when mailbox is busy and there is no previous send trigger (error state)
 * -  0 frame has not been sent after the previous trigger, still waiting for bus control
 * -  1 frame has been sent after the previous trigger
 * -  2 mailbox is ready (empty) and there is no previous send trigger
 * 
 * \return Status of the low-latency transmit mailbox
 * 
 *************************************************************************************************/
extern int
can_status(const struct net *net);

/**************************************************************************************************
 * 
 * \brief Calculates bit timing configuration parameters with respect to given limits
 * 
 * \param btl bit timing configuration parameters limit values
 * \param btp storage for bit timing configuration parameters
 * \param clk CAN peripheral clock frequency (Hz)
 * \param bitrate target communication speed (bps)
 * 
 * \details This function calculates bit timing configuration parameters with respect to given
 * limits, that produce lowest possible bitrate error. Bit time consists of quants (TQ):
 * - Synchronization segment. It always equals 1 TQ.
 * - Propagation segment which depends on parameters such as signal propagation delay on the bus
 *   line, input comparator delay, and output driver delay. It is usually in range 1..8 TQ.
 * - Phase segment to compensate positive edge phase error. It is usually in range 1..8 TQ.
 * - Phase segment to compensate negative edge phase error. It is usually in range 2..8 TQ.
 * 
 * Calculated bit timing configuration parameters are written to the \e btp parameter.
 * 
 * \note Propagation segment in this function is hardcoded to 2 TQ.
 * 
 * \return -1 when bit timing configuration parameters could not be calculated; 0 when parameters
 * produce 0% bitrate error; 1 when parameters produce non-zero bitrate error
 * 
 *************************************************************************************************/
extern int
can_bit_timing(const struct can_btl *btl, struct can_btp *btp, uint32_t clk, uint32_t bitrate);

#endif /* _INC_NET_CAN_H */
