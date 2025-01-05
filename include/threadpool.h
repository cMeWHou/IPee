/**
 * @file threadpool.h
 * @author chcp (cmewhou@yandex.ru)
 * @brief Common threadpool implementation.
 * @version 1.2
 * @date 2024-11-16
 *
 * @copyright Copyright (c) 2024
 */

#ifndef IPEE_THREADPOOL_H
#define IPEE_THREADPOOL_H

#include <pthread.h>

/*********************************************************************************************
 * STRUCTS DECLARATIONS
 ********************************************************************************************/

/**
 * @brief Task release type.
 */
typedef enum task_release_type_e {
    TASK_RELEASE_TYPE_DEFAULT = 0,   // Release task after completion.
    TASK_RELEASE_TYPE_IMMEDIATE = 1, // Release task immediately after completion.
    TASK_RELEASE_TYPE_MANUAL = 2,    // Release task manually.
} task_release_type_t, *p_task_release_type;

typedef struct task_s task_t, *p_task;
typedef struct thread_s thread_t, *p_thread;

/**
 * @brief Task metadata.
 */
typedef struct task_metadata_s {
    int task_id;                            // Task Id.
    p_thread thread;                        // Thread.
    void *(*callback)(void *args);          // Task callback.
    void *args;                             // Task callback arguments.
    void (*release_callback)(p_task *task); // Task release callback.
    task_release_type_t release_type;       // Task release type.
} task_metadata_t, *p_task_metadata;

/**
 * @brief Task information.
 */
typedef struct task_s {
    volatile int is_done;     // Task is done.
    void *result;             // Task result.
    p_task_metadata metadata; // Task metainformation.
} task_t, *p_task;

/**
 * @brief Thread data.
 */
typedef struct thread_s {
    pthread_t thread;        // Thread Id.
    int is_buzy;             // Thread is buzy.
    volatile int is_running; // Thread is running.
    volatile p_task task;    // Current task.
} thread_t, *p_thread;

/***********************************************************************************************
 * FUNCTION TYPEDEFS
 **********************************************************************************************/

/**
 * @brief Task callback.
 * @details A task to complete. It is called when there is a free thread.
 *
 * @param args Task arguments.
 * @return Task result.
 */
typedef void *(threadpool_task_callback)(void *args);

/**
 * @brief Task complete callback.
 * @details A task to complete. It is called when the task is completed.
 *
 * @param result Task result.
 * @param args Captured arguments.
 */
typedef void(threadpool_complete_callback)(void *result, void *args);

/**
 * @brief Task release callback.
 * @details A task to release. It is called when the task is completed.
 *
 * @param task Task.
 */
typedef void(threadpool_release_callback)(p_task task);

/*********************************************************************************************
 * FUNCTIONS DECLARATIONS
 ********************************************************************************************/

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
 * @param timeout Task waiting timeout.
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
 * @param args Task arguments
 * @return Task.
 */
extern p_task make_task(threadpool_task_callback task_callback, void *args);

/**
 * @brief Task completion callback.
 * @details A task to complete. It is called when there is a free thread.
 * @details You can add such callback as you want.
 *
 * @param task Task.
 * @param complete_callback Task completion callback.
 * @param args Captured arguments.
 *
 * @return Task.
 */
extern p_task on_complete(p_task task, threadpool_complete_callback complete_callback, void *args);

/**
 * @brief Task completion callback.
 * @details Same as on_complete, but prefered for releasing resources.
 *
 * @return Task.
 */
extern p_task on_final(p_task task, threadpool_release_callback release_callback);

/**
 * @brief Run task.
 *
 * @param task Task.
 * @return Task.
 */
extern p_task run_task(p_task task);

/**
 * @brief Execute on_complete and on_final immediately after task completion.
 * @details You not need to call await_task() in this case.

 *
 * @param task Task.
 * @return Task result.
 */
extern p_task as_immediate(p_task task);

/**
 * @brief Never execute on_complete and on_final.
 * @details Manual resource releasing.
 *
 * @param task Task.
 * @return Task.
 */
extern p_task as_manual(p_task task);

/**
 * @brief Run task with custom arguments.
 *
 * @param task Task.
 * @param args Custom arguments.
 * @return Task.
 */
extern p_task run_task_with_args(p_task task, void *args);

/**
 * @brief Start new task.
 * @details It is an equivalent for run_task(make_task(...)).
 *
 * @param task_callback Task callback.
 * @param args Task arguments.
 * @return Task.
 */
extern p_task start_task(threadpool_task_callback task_callback, void *args);

/**
 * @brief Await task completion.
 *
 * @param task Task.
 * @return Task result.
 */
extern void *await_task(p_task task);

/**
 * @brief Destroy task pool.
 */
extern void destroy_thread_pool(void);

#endif // IPEE_THREADPOOL_H