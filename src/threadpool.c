#include <threadpool.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#include <dictionary.h>
#include <event.h>
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
 * @param record Record.
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
static void default_task_release_callback(p_task task);

/**
 * @brief Destroy thread.
 * 
 * @param thread The thread.
 */
static void destroy_thread(const p_thread thread);

/**
 * @brief Emit on complete task.
 * 
 * @param task The task.
 */
static void emit_on_complete(p_task task);


/*********************************************************************************************
 * STATIC VARIABLES
 ********************************************************************************************/

/**
 * @brief The thread pool.
 */
static p_dictionary thread_pool = NULL;

/**
 * @brief The mutex.
 */
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief The thread pool size.
 */
static int THREAD_POOL_SIZE = 8;

/**
 * @brief The internal task counter limit.
 */
static int INTERNAL_TASK_COUNTER_LIMIT = 100;

/**
 * @brief The task waiting timeout (in ms).
 */
static int TASK_WAITING_TIMEOUT = 15 * 1000;

/**
 * @brief The event context name.
 */
static char *threadpool_context_name = "threadpool";
static char *threadpool_complete_event_name = "on_complete";

/**
 * @brief The internal task counter.
 */
static int internal_task_counter = 1;


/*********************************************************************************************
 * FUNCTIONS DEFINITIONS
 ********************************************************************************************/

void set_threadpool_size(int size) {
    if (thread_pool) exit(-1);
    THREAD_POOL_SIZE = size;
}

void set_internal_task_counter_limit(int limit) {
    if (thread_pool) exit(IPEE_ERROR_CODE__THREADPOOL__SERVICE_ALREADY_INITIALIZED);
    INTERNAL_TASK_COUNTER_LIMIT = limit;
}

void set_task_waiting_timeout(int timeout) {
    if (thread_pool) exit(IPEE_ERROR_CODE__THREADPOOL__SERVICE_ALREADY_INITIALIZED);
    TASK_WAITING_TIMEOUT = timeout;
}

void init_thread_pool(void) {
    if (!thread_pool) {
        pthread_mutex_init(&mutex, NULL);
        thread_pool = create_dictionary();

        for (int i = 0; i < THREAD_POOL_SIZE; i++) {
            pthread_mutex_lock(&mutex);

            p_thread thread = (p_thread)malloc(sizeof(thread_t));
            if (!thread) exit(IPEE_ERROR_CODE__THREADPOOL__THREAD_CREATION_ERROR);

            thread->is_buzy = 0;
            thread->is_running = 1;
            thread->task = NULL;
            
            pthread_create(&thread->thread, NULL, task_invoker, thread);
            add_record_to_dictionary(thread_pool, i, thread);
            
            pthread_mutex_unlock(&mutex);
        }
    }
}

p_task make_task(threadpool_task_callback callback, void *args) {
    p_task task = (p_task)malloc(sizeof(task_t));
    if (!task) return NULL;

    task->is_done = 0;
    task->result = NULL;
    task->metadata = NULL;

    p_task_metadata metadata = (p_task_metadata)malloc(sizeof(task_metadata_t));
    if (!metadata) return NULL;

    if (internal_task_counter >= INTERNAL_TASK_COUNTER_LIMIT)
        internal_task_counter = 1;

    metadata->task_id = internal_task_counter++;
    metadata->callback = callback;
    metadata->args = args;
    metadata->thread = NULL;
    metadata->release_callback = default_task_release_callback;
    metadata->release_type = TASK_RELEASE_TYPE_DEFAULT;

    task->metadata = metadata;

    return task;
}

p_task run_task(p_task task) {
    return run_task_with_args(task, task->metadata->args);
}

p_task run_task_with_args(p_task task, void *args) {
    if (!thread_pool) exit(IPEE_ERROR_CODE__THREADPOOL__SERVICE_UNINITIALIZED);
    if (!task) exit(IPEE_ERROR_CODE__THREADPOOL__INVALID_TASK);

    p_dictionary available_threads = filter_dictionary(thread_pool, filter_threads);
    if (available_threads->size) {
        task->metadata->args = args;
        p_thread thread = get_head_value_from_dictionary(available_threads);
        if (thread)
            set_task_to_thread(thread, task);
        else
            exit(IPEE_ERROR_CODE__THREADPOOL__INVALID_THREAD);

        return task;
    }
    return NULL;
}

p_task start_task(threadpool_task_callback callback, void *args) {
    return run_task(make_task(callback, args));
}

void *await_task(p_task task) {
    if (!task) exit(IPEE_ERROR_CODE__THREADPOOL__INVALID_TASK);
    
    clock_t before = clock(); 
    long current_timestamp = 0;
    do {
        clock_t diff = clock() - before;
        current_timestamp = diff * 1000 / CLOCKS_PER_SEC;
    } while (!task->is_done && current_timestamp < TASK_WAITING_TIMEOUT);

    void *result = task->result;

    if (task->metadata->release_type == TASK_RELEASE_TYPE_DEFAULT) {
        emit_on_complete(task);
        if (task->metadata->release_callback)
            task->metadata->release_callback(task);
    }

    return result;
}

p_task on_complete(p_task task, threadpool_complete_callback complete_callback, void *args) {
    if (!task) exit(IPEE_ERROR_CODE__THREADPOOL__INVALID_TASK);

    const char *event_name = prepare_event_name(threadpool_context_name, threadpool_complete_event_name, task->metadata->task_id);
    subscribe_with_args(threadpool_context_name, event_name, complete_callback, args);

    return task;
}

p_task on_final(p_task task, threadpool_release_callback release_callback) {
    if (!task) exit(IPEE_ERROR_CODE__THREADPOOL__INVALID_TASK);

    task->metadata->release_callback = release_callback;

    return task;
}

p_task as_immediate(p_task task) {
    if (!task) exit(IPEE_ERROR_CODE__THREADPOOL__INVALID_TASK);
    
    task->metadata->release_type = TASK_RELEASE_TYPE_IMMEDIATE;

    return task;
}

p_task as_manual(p_task task) {
    if (!task) exit(IPEE_ERROR_CODE__THREADPOOL__INVALID_TASK);
    
    task->metadata->release_type = TASK_RELEASE_TYPE_MANUAL;
    
    return task;
}


void destroy_thread_pool(void) {
    if (!thread_pool) exit(IPEE_ERROR_CODE__THREADPOOL__SERVICE_UNINITIALIZED);

    iterate_over_dictionary_values(thread_pool, destroy_thread);
    pthread_mutex_destroy(&mutex);
    delete_dictionary(thread_pool);
    thread_pool = NULL;
}


/***********************************************************************************************
 * STATIC FUNCTIONS DEFINITIONS
 **********************************************************************************************/

static void *task_invoker(p_thread thread) {
    if (!thread) exit(IPEE_ERROR_CODE__THREADPOOL__INVALID_THREAD);

    while (thread->is_running) {
        p_task task = thread->task;
        if (task) {
            p_task_metadata metadata = task->metadata;
            if (!metadata) continue;

            void *result = metadata->callback(metadata->args);

            task->is_done = 1;
            task->result = result;
            if (task->metadata->release_type == TASK_RELEASE_TYPE_IMMEDIATE) {
                emit_on_complete(task);
                if (task->metadata->release_callback)
                    task->metadata->release_callback(task);
            }
            thread->task = NULL;
            thread->is_buzy = 0;
        }
    }

    pthread_exit(NULL);
    return NULL;
}

static void set_task_to_thread(p_thread thread, p_task task) {
    if (!thread) exit(IPEE_ERROR_CODE__THREADPOOL__INVALID_THREAD);
    if (!task) exit(IPEE_ERROR_CODE__THREADPOOL__INVALID_TASK);

    thread->is_buzy = 1;
    thread->task = task;
    task->metadata->thread = thread;
}

static int filter_threads(const p_record record, int _1, const p_dictionary _2) {
    const p_thread thread = (p_thread)record->value;
    if (!record || !record->value) exit(IPEE_ERROR_CODE__THREADPOOL__INVALID_THREAD);

    return !thread->is_buzy;
}

static void destroy_thread(p_thread thread) {
    if (!thread) exit(IPEE_ERROR_CODE__THREADPOOL__INVALID_THREAD);
        
    pthread_mutex_lock(&mutex);

    thread->is_running = 0;
    pthread_cancel(thread->thread);
    free(thread);

    pthread_mutex_unlock(&mutex);
}

static void default_task_release_callback(p_task task) {
    if (!task) exit(IPEE_ERROR_CODE__THREADPOOL__INVALID_TASK);

    if (task->metadata)
        free(task->metadata);
    free(task);
}

static void emit_on_complete(p_task task) {
    if (!task) exit(IPEE_ERROR_CODE__THREADPOOL__INVALID_TASK);

    const char *event_name = prepare_event_name(threadpool_context_name, threadpool_complete_event_name, task->metadata->task_id);
    p_dictionary subscribers = get_context_event_subscribers(threadpool_context_name, event_name);
    if (subscribers) {
        notify(threadpool_context_name, event_name, task->result);
        unsubscribe_from_event(threadpool_context_name, event_name);
    }
    free(event_name);
}