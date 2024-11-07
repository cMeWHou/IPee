#include <threadpool.h>

#include <stdlib.h>
#include <pthread.h>

#include <dictionary.h>
#include <macro.h>

/***********************************************************************************************
 * STATIC FUNCTIONS DECLARATIONS
 **********************************************************************************************/

/**
 * @brief Invokes callback of a task in a thread.
 * 
 * @param thread The thread.
 * @return Stub.
 */
static void *task_invoker(p_thread thread);

/**
 * @brief Set the task to thread object.
 * 
 * @param thread The thread.
 * @param task The task.
 */
static void set_task_to_thread(p_thread thread, p_task task);

/**
 * @brief Filter available threads.
 * 
 * @param record 
 * @param _1 Unused.
 * @param _2 Unused.
 * @return Result of comparsion.
 */
static int filter_threads(const p_record record, int _1, const p_dictionary _2);

/**
 * @brief Free task.
 * 
 * @param task The task.
 */
static void free_task(p_task task);

/**
 * @brief Destroy thread.
 * 
 * @param thread The thread.
 */
static void destroy_thread(const p_thread thread);

/*********************************************************************************************
 * STATIC VARIABLES
 ********************************************************************************************/

/**
 * @brief The thread pool.
 */
static p_dictionary thread_pool = NULL;

/**
 * @brief The thread pool size.
 */
static int THREAD_POOL_SIZE = 8;

/*********************************************************************************************
 * FUNCTIONS DEFINITIONS
 ********************************************************************************************/

void set_threadpool_size(int size) {
    THREAD_POOL_SIZE = size;
}

void init_thread_pool(void) {
    if (!thread_pool) {
        thread_pool = create_dictionary();
        for (int i = 0; i < THREAD_POOL_SIZE; i++) {
            p_thread thread = (p_thread)malloc(sizeof(thread_t));
            thread->is_buzy = 0;
            thread->task = NULL;
            
            pthread_create(&thread->thread, NULL, task_invoker, thread);
            add_record_to_dictionary(thread_pool, i, thread);
        }
    }
}

p_task make_task(threadpool_task_callback callback, void *args) {
    p_task task = (p_task)malloc(sizeof(task_t));
    task->is_done = 0;
    task->result = NULL;

    p_task_metadata metadata = (p_task_metadata)malloc(sizeof(task_metadata_t));
    metadata->callback = callback;
    metadata->args = args;
    metadata->thread = 0;

    task->metadata = metadata;
    return task;
}

p_task run_task(p_task task) {
    const p_dictionary available_threads = filter_dictionary(thread_pool, filter_threads);
    if (available_threads->size) {
        p_thread thread = get_head_value_from_dictionary(available_threads);
        set_task_to_thread(thread, task);
        return task;
    }
    return NULL;
}

p_task start_task(threadpool_task_callback callback, void *args) {
    return run_task(make_task(callback, args));
}

void *await_task(p_task task) {
    while (!task->is_done);

    void *result = task->result;
    free_task(task);
    return result;
}

void destroy_thread_pool(void) {
    if (thread_pool->size) {
        iterate_over_dictionary_values(thread_pool, destroy_thread);
        delete_dictionary(thread_pool);
    }
    thread_pool = NULL;
}

/***********************************************************************************************
 * STATIC FUNCTIONS DEFINITIONS
 **********************************************************************************************/

static void *task_invoker(p_thread thread) {
    while (1) {
        p_task task = thread->task;
        if (task) {
            p_task_metadata metadata = task->metadata;
            if (!metadata) continue;

            void *result = metadata->callback(metadata->args);
            task->is_done = 1;
            task->result = result;

            metadata->thread = 0;
            thread->is_buzy = 0;
            thread->task = NULL;
        }
    }
    return NULL;
}

static void set_task_to_thread(p_thread thread, p_task task) {
    thread->is_buzy = 1;
    thread->task = task;
    task->metadata->thread = &thread;
}

static int filter_threads(const p_record record, int _1, const p_dictionary _2) {
    const p_thread thread = (p_thread)record->value;
    return !thread->is_buzy;
}

static void destroy_thread(p_thread thread) {
    if (thread) {
        pthread_cancel(thread->thread);
        free_task(thread->task);
        free(thread);
    }
}

static void free_task(p_task task) {
    if (task) {
        if (task->metadata) {
            free(task->metadata);
        }
        free(task);
    }
}