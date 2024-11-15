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
    TASK_RELEASE_TYPE_IMMEDIATELY = 0, // Release task immediately after completion.
    TASK_RELEASE_TYPE_MANUAL = 1,      // Release task manually.
} task_release_type_t, *p_task_release_type;

/**
 * @brief Task metainformation.
 */
typedef struct task_metadata_s {
    void *(*callback)(void *args); // Task callback.
    void *args;                    // Task arguments.
    pthread_t *thread;             // Thread Id.
} task_metadata_t, *p_task_metadata;

/**
 * @brief Task information.
 */
typedef struct task_s {
    volatile int is_done;                          // Task is done.
    void *result;                                  // Task result.
    void (*release_callback)(struct task_s *task); // Task release callback.
    task_release_type_t release_type;              // Task release type.
    p_task_metadata metadata;                      // Task metainformation.
} task_t, *p_task;

/**
 * @brief Thread data.
 */
typedef struct thread_s {
    int is_buzy;          // Thread is buzy.
    int is_running;       // Thread is running.
    pthread_t thread;     // Thread Id.
    volatile p_task task; // Task metainformation.
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
 * @brief Task release callback.
 * @details A task to release. It is called when the task is completed.
 * 
 * @param task Task.
 */
typedef void (threadpool_release_callback)(p_task task);

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
 * @brief Initialize thread pool.
 */
extern void init_thread_pool(void);

/**
 * @brief Create new task.
 * 
 * @param task_callback Task callback.
 * @return Task.
 */
extern p_task make_task(threadpool_task_callback task_callback);

/**
 * @brief Create new task with arguments.
 * 
 * @param task_callback Task callback.
 * @param args Task arguments.
 * @return Task.
 */
extern p_task make_task_with_args(threadpool_task_callback task_callback, void *args);

/**
 * @brief Create new task with release callback.
 * 
 * @param task_callback Task callback.
 * @param args Task arguments.
 * @param release_callback Task release callback.
 * @return Task.
 */
extern p_task make_task_with_release_callback(threadpool_task_callback task_callback, void *args, threadpool_release_callback release_callback);

/**
 * @brief Run task.
 * 
 * @param task Task.
 * @return Task.
 */
extern p_task run_task(p_task task);

/**
 * @brief Run task with arguments.
 * 
 * @param task Task.
 * @param args Task arguments.
 * @return Task.
 */
extern p_task run_task_with_args(p_task task, void *args);

/**
 * @brief Run task with release callback.
 * 
 * @param task Task.
 * @param args Task arguments.
 * @param release_callback Task release callback.
 * @return Task.
 */
extern p_task run_task_with_release_callback(p_task task, void *args, threadpool_release_callback release_callback);

/**
 * @brief Start new task.
 * 
 * @param task_callback Task callback.
 * @return Task.
 */
extern p_task start_task(threadpool_task_callback task_callback);

/**
 * @brief Start new task with arguments.
 * 
 * @param task_callback Task callback.
 * @param args Task arguments.
 * @return Task.
 */
extern p_task start_task_with_args(threadpool_task_callback task_callback, void *args);

/**
 * @brief Start new task with release callback.
 * 
 * @param task_callback Task callback.
 * @param args Task arguments.
 * @param release_callback Task release callback.
 * @return Task.
 */
extern p_task start_task_with_release_callback(threadpool_task_callback task_callback, void *args, threadpool_release_callback release_callback);

/**
 * @brief Release task after completion emmediately.
 * 
 * @param task Task.
 * @return Task result.
 */
extern p_task as_immediate(p_task task);

/**
 * @brief Await task.
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