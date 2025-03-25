/**************************************************************************************************
 * 
 * \file slave_ctl.c
 * 
 * \brief Slave control module implementation. This module is used whenever a board should control
 * another board (for example, when the LLC controls the PFC). This function simply abstracts the
 * state machine logic, but the application code must still implement the CAN messages and how to
 * call this module function.
 * 
 * NOTE: this function should be called with a task at 1 KHz frequency! Otherwise, the cooldown
 * time will not be what we expect.
 * 
 * \author Jorge Sola
 * 
 *************************************************************************************************/

#include "inc/lib/slave_ctl.h"

#define C_CONTROL_SETPOINT_WAIT (0.2f)  // 200 milliseconds
#define C_CONTROL_STOP_WAIT (0.2f)  // 200 milliseconds
#define C_TASK_FREQ (1000)          // 1 Khz


/**************************************************************************************************
 * 
 * slave_control()
 * 
 *************************************************************************************************/
void slave_control(struct slave_ctl *slave, enum slave_mode slave_mode, bool start, float current_ref, float voltage_ref)
{
    switch (slave->state) {
    case SLAVE_IDLE:
        if (start) {
            slave->state = SLAVE_SETPOINTS;
            slave->cooldown_cnt = C_CONTROL_SETPOINT_WAIT * C_TASK_FREQ;
        }
        break;
    case SLAVE_SETPOINTS:
        slave->current_setpoint = current_ref;
        slave->voltage_setpoint = voltage_ref;
        if (slave->cooldown_cnt-- == 0) {
            slave->state = SLAVE_START;
        }
        break;
    case SLAVE_START:
        // Set things ON and set setpoints
        if (slave_mode == MODE_AC) {
             slave->mode = MODE_AC;
        } else if (slave_mode == MODE_DC) {
            slave->mode = MODE_DC;
        }
        slave->module_on = true;
        slave->current_setpoint = current_ref;
        slave->voltage_setpoint = voltage_ref;
        slave->state = SLAVE_RUN;
        break;
    case SLAVE_RUN:
        slave->current_setpoint = current_ref;
        slave->voltage_setpoint = voltage_ref;
        if (!start) {
            slave->state = SLAVE_STOP;
        }
        break;
    case SLAVE_STOP:
        slave->cooldown_cnt = C_CONTROL_STOP_WAIT * C_TASK_FREQ;
        slave->module_on = false;
        slave->state = SLAVE_STOP_WAIT;
        
        break;
    case SLAVE_STOP_WAIT:
        if (slave->cooldown_cnt-- == 0) {
            slave->state = SLAVE_IDLE;
        }
        break;
    };
}
