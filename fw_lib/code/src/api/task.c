/**************************************************************************************************
 * 
 * \file task.c
 * 
 * \brief Task scheduler abstraction implementation
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#include "inc/api/task.h"

#include "inc/hal/hal.h"
#include "inc/lib/debug.h"
#include "inc/lib/utils.h"

#include <stddef.h>

/**************************************************************************************************
 * 
 * task_init()
 * 
 *************************************************************************************************/
int
task_init(const struct task *task)
{
    ASSERT(obj_check(task, OBJ_TASK));

    void **_task = (void **) task;
    struct task_priv *priv = (struct task_priv *) _task[0];
    struct task_job **job = (struct task_job **) &_task[1];

    ASSERT(priv->tlo);

    priv->init = false;

    const uint32_t freq = hal_get_frequency();

    unsigned i;
    for (i = 0U; i < priv->size; i++) {
        ASSERT(job[i] && job[i]->freq && job[i]->callback);
        
#if 0 /* Allow task frequency error! */
        if (freq % job[i]->freq) {
            return -1;
        }
#endif

        job[i]->period = div_round_u32(freq, job[i]->freq);

        /* We initialize the counter with an offset depending
        on the task index, so that there is some time shift
        in the execution of the tasks and they are not all
        executed at the same time (at least the ones with
        the same frequency) */
        job[i]->counter = i*job[i]->period/priv->size;
    }

    priv->init = true;

    return 0;
}

/**************************************************************************************************
 * 
 * task_run()
 * 
 *************************************************************************************************/
void
task_run(const struct task *task)
{
    /* Null-guard */
    while (!task);

    ASSERT(obj_check(task, OBJ_TASK));

    void **_task = (void **) task;
    struct task_priv *priv = (struct task_priv *) _task[0];
    struct task_job **job = (struct task_job **) &_task[1];

    ASSERT(priv->tlo);

    if (!priv->init) {
        return;
    }

    while (true) {
        /* Wait until next task scheduler tick */
        while (!hal_tick());
        
        /* Dispatch tasks */
        unsigned i;
        for (i = 0U; i < priv->size; i++) {
            ASSERT(job[i] && job[i]->callback);

            if (!job[i]->enable) {
                job[i]->counter = 0U;
            } else if (++job[i]->counter >= job[i]->period) {
                job[i]->counter = 0U;
                job[i]->callback(priv->tlo);
            }
        }
    }
}
