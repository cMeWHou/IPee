/**
 * @file threadpool.h
 * @author chcp (cmewhou@yandex.ru)
 * @brief Common threadpool implementation.
 * @version 1.1
 * @date 2024-08-04
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
    volatile int is_done;               // Task is done.
    void *result;                       // Task result.
    p_task_metadata metadata;           // Task metainformation.
} task_t, *p_task;

/**
 * @brief Thread data.
 */
typedef struct thread_s {
    int is_buzy;          // Thread is buzy.
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
 * @param args Task arguments.
 * @return Task.
 */
extern p_task make_task(threadpool_task_callback task_callback, void *args);

/**
 * @brief Run task.
 * 
 * @param task Task.
 * @return Task.
 */
extern p_task run_task(p_task task);

/**
 * @brief Start new task.
 * 
 * @param task_callback Task callback.
 * @param args Task arguments.
 * @return Task.
 */
extern p_task start_task(threadpool_task_callback task_callback, void *args);

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