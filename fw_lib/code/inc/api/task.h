/**************************************************************************************************
 * 
 * \file task.h
 * 
 * \brief Task scheduler abstraction interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_API_TASK_H
#define _INC_API_TASK_H

#include "cgen/api/task.h"
#include "inc/api/obj.h"

#include <stdint.h>
#include <stdbool.h>

/**************************************************************************************************
 * 
 * Forward declarations
 * 
 *************************************************************************************************/

struct task;
struct tlo;

/**************************************************************************************************
 * 
 * Private variables definition
 * 
 *************************************************************************************************/
OBJ_PRIV_STRUCT(task_priv,
    const struct tlo *tlo;          /* Top-level object handler                                 */
    bool init;                      /* Indicates if task object has been initialized            */
);

/**************************************************************************************************
 * 
 * Job object definition
 * 
 *************************************************************************************************/
struct task_job {
    bool enable;                    /* Enable or disable task                                   */
    const uint16_t freq;            /* Task frequency (Hz)                                      */
    uint16_t period;                /* Task period relative to scheduler frequency              */
    uint16_t counter;               /* Software timer counter                                   */

    /**********************************************************************************************
     * 
     * \brief Job callback function
     * 
     * \param tlo top-level object handler
     * 
     * \return None
     * 
     *********************************************************************************************/
    void (* const callback)(const struct tlo *tlo);
};

/**************************************************************************************************
 * 
 * \brief Initializes task scheduler object
 * 
 * \param task task scheduler object handler
 * 
 * \return 0 if initialization is successful; -1 otherwise
 * 
 *************************************************************************************************/
extern int
task_init(const struct task *task);

/**************************************************************************************************
 * 
 * \brief Runs task scheduler in an infinite loop
 * 
 * \param task task scheduler object handler
 * 
 * \return None
 * 
 *************************************************************************************************/
extern void
task_run(const struct task *task);

/**************************************************************************************************
 * 
 * \brief Creates new task scheduler object
 * 
 * \param tlo top-level object handler
 * 
 * \note This function is implemented in the application layer!
 * 
 * \return Task scheduler object handler
 * 
 *************************************************************************************************/
extern const struct task *
task_new(const struct tlo *tlo);

#endif /* _INC_API_TASK_H */
