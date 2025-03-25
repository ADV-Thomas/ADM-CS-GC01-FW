/**************************************************************************************************
 * 
 * \file task.h
 * 
 * \brief Task scheduler interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _APP_TASK_H
#define _APP_TASK_H

#include "inc/api/task.h"

/**************************************************************************************************
 * 
 * Task scheduler object definition
 * 
 *************************************************************************************************/
TASK_OBJ_STRUCT(
    TASK_OBJ_STRUCT_MEMBER(can);
    TASK_OBJ_STRUCT_MEMBER(blink);
    TASK_OBJ_STRUCT_MEMBER(meas);
    TASK_OBJ_STRUCT_MEMBER(phy);
    TASK_OBJ_STRUCT_MEMBER(ctl);
    TASK_OBJ_STRUCT_MEMBER(screen);

);



struct commande {
    bool up;
    bool down;
    bool left;
    bool right;
    bool knob_p;
    bool knob_m;
    bool knob_b;

};


#endif /* _APP_TASK_H */
