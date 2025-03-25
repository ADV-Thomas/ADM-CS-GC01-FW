/**************************************************************************************************
 * 
 * \file task.h
 * 
 * \brief Task scheduler abstraction code generator
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _CGEN_API_TASK_H
#define _CGEN_API_TASK_H

#include "cgen/api/obj.h"

/**************************************************************************************************
 * 
 * \brief Generates new static private variables structure
 * 
 * \details This function generates private variables structure for task scheduler object:
 * - variable type is 'struct task_priv'
 * - variable name is 'priv'
 * - object structure type is 'struct task'
 * - object type is 'OBJ_TASK'
 * 
 *************************************************************************************************/
#define TASK_PRIV_NEW()                                                                         \
    OBJ_PRIV_NEW(task_priv, priv, task, OBJ_TASK,                                               \
        .tlo  = NULL,                                                                           \
        .init = false                                                                           \
    )

/**************************************************************************************************
 * 
 * \brief Generates new static task scheduler object
 * 
 * \param ... optional initializers
 * 
 * \details This function generates task scheduler object with its private variables structure:
 * - object structure type is 'struct task'
 * - object name is 'task'
 * 
 *************************************************************************************************/
#define TASK_OBJ_NEW(...)                                                                       \
    TASK_PRIV_NEW();                                                                            \
    OBJ_NEW(task, task, &priv, __VA_ARGS__)

/**************************************************************************************************
 * 
 * \brief Generates new job object
 * 
 * \param NAME job object name
 * \param FREQ job frequency in Hz
 * \param CALLBACK callback function handler
 * 
 * \details This function generates job object which is \b enabled by default.
 * 
 *************************************************************************************************/
#define TASK_JOB_NEW(NAME, FREQ, CALLBACK)                                                      \
    static struct task_job NAME = {                                                             \
        .enable   = true,                                                                       \
        .freq     = FREQ,                                                                       \
        .period   = 0U,                                                                         \
        .counter  = 0U,                                                                         \
        .callback = CALLBACK                                                                    \
    }

/**************************************************************************************************
 * 
 * \brief Disables job object
 * 
 * \param NAME job object name
 * 
 *************************************************************************************************/
#define TASK_JOB_DISABLE(NAME)                                                                  \
    NAME.enable = false

/**************************************************************************************************
 * 
 * \brief Generates task object structure definition
 * 
 * \param ... optional structure members
 * 
 *************************************************************************************************/
#define TASK_OBJ_STRUCT(...)                                                                    \
    OBJ_STRUCT(task, task_priv, __VA_ARGS__)

/**************************************************************************************************
 * 
 * \brief Adds job object to task object structure definition
 * 
 * \param NAME job object name
 * 
 *************************************************************************************************/
#define TASK_OBJ_STRUCT_MEMBER(NAME)                                                            \
    OBJ_STRUCT_MEMBER(task_job, NAME)

#endif /* _CGEN_API_TASK_H */
