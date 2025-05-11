/*******************************************************************************
 * @file threadpool.h
 * @author chcp (cmewhou@yandex.ru)
 * @brief Common threadpool implementation.
 * @version 1.4
 * @date 2025-05-10
 *
 * @copyright Copyright (c) 2024 - 2025
 ******************************************************************************/

#ifndef IPEE_THREADPOOL_H
#define IPEE_THREADPOOL_H

#include <pthread.h>

/*******************************************************************************
 * STRUCTS DECLARATIONS
 ******************************************************************************/

/**
 * @brief Task release type.
 */
typedef enum task_release_type_e {
    TASK_RELEASE_TYPE_DEFAULT, // Release task after completion.
    TASK_RELEASE_TYPE_MANUAL,  // Release task manually.
} task_release_type_t, *p_task_release_type;

typedef struct task_s {
    volatile int is_done;    // Task is done.
    volatile int is_running; // Task is running.
    int          id;         // Task identifier.
    void        *result;     // Task result.
} task_t, *p_task;

/*******************************************************************************
 * FUNCTION TYPEDEFS
 ******************************************************************************/

/**
 * @brief Task callback.
 *
 * @details
 * A task to complete. It is called when there is a free thread.
 *
 * @param args Task arguments.
 *
 * @return Task result.
 */
typedef void *(threadpool_task_callback)(void *args);

/**
 * @brief Task complete callback.
 *
 * @details
 * A task to complete. It is called when the task is completed.
 *
 * @param result Task result.
 * @param args   Captured arguments.
 */
typedef void(threadpool_complete_callback)(void *result, void *args);

/**
 * @brief Task release callback.
 *
 * @details
 * A task to release. It is called when the task is completed.
 *
 * @param task Task.
 */
typedef void(threadpool_release_callback)(p_task task);

/*******************************************************************************
 * FUNCTIONS DECLARATIONS
 ******************************************************************************/

/**
 * @brief Set thread pool size.
 *
 * @param size Thread pool size.
 */
extern void set_threadpool_size(int size);

/**
 * @brief Set internal task counter limit.
 *
 * @param limit Internal task counter limit.
 */
extern void set_internal_task_counter_limit(int limit);

/**
 * @brief Set task waiting timeout.
 *
 * @param timeout Task waiting timeout (in ms).
 */
extern void set_task_waiting_timeout(int timeout);

/**
 * @brief Initialize thread pool.
 */
extern void init_thread_pool(void);

/**
 * @brief Create new task.
 *
 * @param task_callback Task callback.
 * @param args          Task arguments
 *
 * @return Task.
 */
extern p_task make_task(threadpool_task_callback task_callback, void *args);

/**
 * @brief Cancel running task.
 * 
 * @param task Task.
 *
 * @return Task.
 */
extern int cancel_task(p_task task);

/**
 * @brief Task completion callback.
 * 
 * @details
 * A task to complete. It is called when there is a free thread.
 * You can add such callback as you want.
 *
 * @param task              Task.
 * @param complete_callback Task completion callback.
 * @param args              Captured arguments.
 *
 * @return Task.
 */
extern p_task on_complete(p_task task,
    threadpool_complete_callback complete_callback, void *args);

/**
 * @brief Set task delay.
 *
 * @param task  Task.
 * @param delay Delay (in ms).
 *
 * @return Task.
 */
extern p_task with_delay(p_task task, int delay);

/**
 * @brief Set task interval.
 *
 * @param task     Task.
 * @param interval Interval (in ms).
 * @param count    Times to repeat (0 for infinity).
 *
 * @return Task.
 */
extern p_task with_interval(p_task task, int interval, int count);

/**
 * @brief Set task priority.
 *
 * @param task     Task.
 * @param priority Priority (0 - max priority,
 *                 other positive value - less priority).
 *
 * @return Task.
 */
extern p_task with_priority(p_task task, int priority);

/**
 * @brief Manual resource releasing.
 *
 * @details
 * Never execute on_complete and on_final.
 *
 * @param task Task.
 *
 * @return Task.
 */
extern p_task as_manual(p_task task);

/**
 * @brief Run task.
 *
 * @param task Task.
 *
 * @return Task.
 */
extern p_task run_task(p_task task);

/**
 * @brief Run task with custom arguments.
 *
 * @param task Task.
 * @param args Custom arguments.
 *
 * @return Task.
 */
extern p_task run_task_with_args(p_task task, void *args);

/**
 * @brief Start new task.
 *
 * @details
 * It is an equivalent for run_task(make_task(...)).
 *
 * @param task_callback Task callback.
 * @param args Task arguments.
 *
 * @return Task.
 */
extern p_task start_task(threadpool_task_callback task_callback, void *args);

/**
 * @brief Await task completion.
 *
 * @param task Task.
 *
 * @return Task result.
 */
extern void *await_task(p_task task);

/**
 * @brief Destroy task pool.
 */
extern void destroy_thread_pool(void);

#endif // IPEE_THREADPOOL_H