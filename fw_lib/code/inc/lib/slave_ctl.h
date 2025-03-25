/**************************************************************************************************
 * 
 * \file slave_ctl.h
 * 
 * \brief Slave control module implementation. This module is used whenever a board should control
 * another board (for example, when the LLC controls the PFC). This function simply abstracts the
 * state machine logic, but the application code must still implement the CAN messages and how to
 * call this module function.
 * 
 * \author Jorge Sola
 * 
 *************************************************************************************************/

#ifndef _INC_LIB_SLAVE_CTL_H
#define _INC_LIB_SLAVE_CTL_H


#include <stdbool.h>

enum slave_mode{
    MODE_DC = 0,
    MODE_AC,
};

enum slave_ctl_state {
    SLAVE_IDLE,
    SLAVE_SETPOINTS,
    SLAVE_START,
    SLAVE_RUN,
    SLAVE_STOP,
    SLAVE_STOP_WAIT,
};

struct slave_ctl{
    bool module_on;
    enum slave_mode mode;
    enum slave_ctl_state state;
    float current_setpoint;
    float voltage_setpoint;
    float last_sent_voltage;
    int cooldown_cnt;
};

void slave_control(struct slave_ctl *slave, enum slave_mode slave_mode, bool start, float current_ref, float voltage_ref);


#endif /* _INC_LIB_SLAVE_CTL_H */
