/**
 * @file threadpool.h
 * @author chcp (cmewhou@yandex.ru)
 * @brief Common threadpool implementation.
 * @version 1.0
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
 * @brief Task information.
 */
typedef struct task_s {
    volatile int is_done;   // Task is done.
    void *result;           // Task result.
} task_t, *p_task;

/**
 * @brief Task metainformation.
 */
typedef struct task_data_s {
    p_task task;                        // Task information.
    void *(*task_callback)(void *args); // Task callback.
    void *args;                         // Task arguments.
} task_data_t, *p_task_data;

/**
 * @brief Thread data.
 */
typedef struct thread_s {
    int is_buzy;                    // Thread is buzy.
    pthread_t thread;               // Thread ID.
    volatile p_task_data task_data; // Task metainformation.
} thread_t, *p_thread;

/***********************************************************************************************
 * FUNCTION TYPEDEFS
 **********************************************************************************************/

typedef void *(threadpool_task_callback)(void *args);

/*********************************************************************************************
 * FUNCTIONS DECLARATIONS
 ********************************************************************************************/

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