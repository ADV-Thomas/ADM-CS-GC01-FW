/**************************************************************************************************
 * 
 * \file net.h
 * 
 * \brief Interface for different communication protocols
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_NET_NET_H
#define _INC_NET_NET_H

#include "cgen/net/net.h"
#include "inc/drv/io.h"

#include <stdbool.h>

/**************************************************************************************************
 * 
 * Net object is a generic interface for different communication protocols. Since all drivers are
 * of 'struct net' type, these enums provide means to identify communication protocol. Values are
 * used in the \e net_check() as \e type parameter and returned from the \e net_get_type().
 * 
 *************************************************************************************************/
enum net_type {
    NET_NONE = 0,               /* Undefined communication protocol                             */
    NET_CAN,                    /* Communication over CAN protocol                              */
    NET_I2C,                    /* Communication over I2C protocol                              */
    NET_FLASH,                  /* Communication with on-chip Flash                             */
    NET_EEPROM,                 /* Communication with external EEPROM                           */
    NET_FILE,                   /* Communication with binary file (in virtual mode only)        */
    NET_SPI,
    NET_FSI
};

/**************************************************************************************************
 * 
 * Net object (interface) definition. Macro \b NET_OBJ_NEW() can be used to create new net object
 * for any communication protocol (driver) implementation.
 * 
 *************************************************************************************************/
struct net {
    /**********************************************************************************************
     * 
     * \brief Driver private variables
     * 
     * \details The private structure is defined separately for every communication protocol
     * implementation. However, it must include variables in the following order:
     *  - enum net_type type    Communication protocol type
     *  - bool open             True when driver is opened
     *  - bool connect          True when driver is connected to I/O pins
     * 
     * Macro \b NET_PRIV_STRUCT() should be used for the private structure definition, and macro
     * \b NET_PRIV_NEW() should be used to create new private variables structure.
     * 
     * \note Drivers that do not connect to I/O pins must have the \e connect flag set to \e true.
     * Otherwise, the \e net_open() function will fail.
     * 
     *********************************************************************************************/
    void *priv;

    /**********************************************************************************************
     * 
     * \brief Driver object handler
     * 
     * \details \e net object is a generic interface that contains only variables and functions
     * common to all communication protocols. This variable contains driver-specific functions.
     * 
     *********************************************************************************************/
    const void *drv;

    int (*open)(const struct net *net, const void *usr);
    int (*close)(const struct net *net);
    int (*read)(const struct net *net, void *f, unsigned length);
    int (*write)(const struct net *net, void *f, unsigned length);
};

/**************************************************************************************************
 * 
 * \brief Performs type check of a generic net object
 * 
 * \param net net object handler
 * \param type expected communication protocol
 * 
 * \details This function checks if a generic net object (interface) matches the expected
 * communication protocol.
 * 
 * \return \b true when type check is successful
 * 
 *************************************************************************************************/
extern bool
net_check(const struct net *net, enum net_type type);

/**************************************************************************************************
 * 
 * \brief Returns communication protocol
 * 
 * \param net net object handler
 * 
 * \return Communication protocol
 * 
 *************************************************************************************************/
extern enum net_type
net_get_type(const struct net *net);

/**************************************************************************************************
 * 
 * \brief Opens driver for communication
 * 
 * \param net net object handler
 * \param usr driver-specific user configuration data
 * \param type expected communication protocol
 * 
 * \details This function is a wrapper for \e net->open(). It performs the following checks:
 * - net object matches given communication protocol,
 * - driver has been connected to I/O pins, and
 * - \e net->open() has been set.
 * 
 * \note Drivers that do not connect to I/O pins must have the \e connect flag set to \e true.
 * 
 * \note User configuration data are optional for some drivers and are not checked here.
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
net_open(const struct net *net, const void *usr, enum net_type type);

/**************************************************************************************************
 * 
 * \brief Closes driver for communication
 * 
 * \param net net object handler
 * \param type expected communication protocol
 * 
 * \details This function is a wrapper for \e net->close(). It performs the following checks:
 * - net object matches given communication protocol, and
 * - \e net->close() has been set.
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
net_close(const struct net *net, enum net_type type);

/**************************************************************************************************
 * 
 * \brief Reads data as specified by transaction frames
 * 
 * \param net net object handler
 * \param f transaction frame with read specification
 * \param length number of transaction frames
 * \param type expected communication protocol
 * 
 * \details This function is a wrapper for \e net->read(). It performs the following checks:
 * - net object matches given communication protocol,
 * - driver has been opened,
 * - \e net->read() has been set,
 * - transaction frame is not \b NULL, and
 * - number of transactions frames is greater than \b 0.
 * 
 * \return number of read frames if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
net_read(const struct net *net, void *f, unsigned length, enum net_type type);

/**************************************************************************************************
 * 
 * \brief Writes data as specified by transaction frames
 * 
 * \param net net object handler
 * \param f transaction frame with write specification
 * \param length number of transaction frames
 * \param type expected communication protocol
 * 
 * \details This function is a wrapper for \e net->write(). It performs the following checks:
 * - net object matches given communication protocol,
 * - driver has been opened,
 * - \e net->write() has been set,
 * - transaction frame is not \b NULL, and
 * - number of transactions frames is greater than \b 0.
 * 
 * \return number of written frames if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
net_write(const struct net *net, void *f, unsigned length, enum net_type type);

/**************************************************************************************************
 * 
 * \brief Connects driver to I/O pins
 * 
 * \param net net object handler
 * \param io I/O pin configuration code
 * \param exp expected I/O peripheral function
 * \param length number of elements in \e io
 * \param type expected communication protocol
 * 
 * \details This function connects the driver to I/O pins. It performs the following checks:
 * - net object matches given communication protocol,
 * - driver is not already connected,
 * - \e io is not \b NULL and number of elements is greater than \b 0, and
 * - given I/O pin configuration code matches expected peripheral function, but only if the \e exp
 *   parameter is not \b NULL.
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
net_connect(const struct net *net, enum io *io, const enum io_fun *exp, unsigned length,
    enum net_type type);

/**************************************************************************************************
 * 
 * \brief Disconnects driver from I/O pins
 * 
 * \param net net object handler
 * \param io I/O pin configuration code
 * \param length number of elements in \e io
 * \param type expected communication protocol
 * 
 * \details This function disconnects the driver from I/O pins. It performs the following checks:
 * - net object matches given communication protocol,
 * - driver is not already disconnected, and
 * - \e io is not \b NULL and number of elements is greater than \b 0.
 * 
 * \return 0 if operation is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
net_disconnect(const struct net *net, enum io *io, unsigned length, enum net_type type);

#endif /* _INC_NET_NET_H */
