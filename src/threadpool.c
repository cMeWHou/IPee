#include <threadpool.h>

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <dictionary.h>
#include <bitset.h>
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
 * @return 1 if thread is available, 0 otherwise.
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
 * @brief The task bitset.
 */
static p_bitset task_bitset = NULL;

/**
 * @brief The mutex.
 */
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief Signaled when a thread becomes free.
 */
static pthread_cond_t pool_cond = PTHREAD_COND_INITIALIZER;

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

/*********************************************************************************************
 * FUNCTIONS DEFINITIONS
 ********************************************************************************************/

int set_threadpool_size(int size) {
    if (thread_pool)
        return IPEE_ERROR_CODE__THREADPOOL__SERVICE_ALREADY_INITIALIZED;
    THREAD_POOL_SIZE = size;
    return 0;
}

int set_internal_task_counter_limit(int limit) {
    if (thread_pool)
        return IPEE_ERROR_CODE__THREADPOOL__SERVICE_ALREADY_INITIALIZED;
    INTERNAL_TASK_COUNTER_LIMIT = limit;
    return 0;
}

void set_task_waiting_timeout(int timeout) {
    TASK_WAITING_TIMEOUT = timeout;
}

int init_thread_pool(void) {
    if (thread_pool)
        return 0;

    pthread_mutex_init(&mutex, NULL);
    thread_pool = create_dictionary();
    task_bitset = init_bitset(INTERNAL_TASK_COUNTER_LIMIT);

    for (int i = 0; i < THREAD_POOL_SIZE; i++) {
        pthread_mutex_lock(&mutex);

        p_thread thread = (p_thread)malloc(sizeof(thread_t));
        if (!thread) {
            pthread_mutex_unlock(&mutex);
            return IPEE_ERROR_CODE__THREADPOOL__THREAD_CREATION_ERROR;
        }

        thread->is_busy = 0;
        thread->is_running = 1;
        thread->task = NULL;
        pthread_cond_init(&thread->cond, NULL);

        pthread_create(&thread->thread, NULL, task_invoker, thread);
        add_record_to_dictionary(thread_pool, i, thread);

        pthread_mutex_unlock(&mutex);
    }
    return 0;
}

p_task make_task(threadpool_task_callback callback, void *args) {
    p_task task = (p_task)malloc(sizeof(task_t));
    if (!task)
        return NULL;

    task->is_done = 0;
    task->is_running = 0;
    task->result = NULL;
    task->metadata = NULL;

    p_task_metadata metadata = (p_task_metadata)malloc(sizeof(task_metadata_t));
    if (!metadata) {
        free(task);
        return NULL;
    }

    int internal_task_counter = get_first_free_bit(task_bitset);
    if (internal_task_counter == -1) {
        free(metadata);
        free(task);
        return NULL;
    }

    set_bit(task_bitset, internal_task_counter);

    metadata->task_id = internal_task_counter;
    metadata->task_event_name = prepare_event_name(
        threadpool_context_name, threadpool_complete_event_name, metadata->task_id);
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
    if (!thread_pool || !task)
        return NULL;

    pthread_mutex_lock(&mutex);

    struct timespec deadline;
    clock_gettime(CLOCK_REALTIME, &deadline);
    deadline.tv_sec  += TASK_WAITING_TIMEOUT / 1000;
    deadline.tv_nsec += (TASK_WAITING_TIMEOUT % 1000) * 1000000L;
    if (deadline.tv_nsec >= 1000000000L) {
        deadline.tv_sec++;
        deadline.tv_nsec -= 1000000000L;
    }

    p_dictionary available_threads = filter_dictionary(thread_pool, filter_threads);
    while (!available_threads->size) {
        delete_dictionary(available_threads);
        if (pthread_cond_timedwait(&pool_cond, &mutex, &deadline) == ETIMEDOUT) {
            pthread_mutex_unlock(&mutex);
            return NULL;
        }
        available_threads = filter_dictionary(thread_pool, filter_threads);
    }

    task->metadata->args = args;
    p_thread thread = get_head_value_from_dictionary(available_threads);
    delete_dictionary(available_threads);

    if (!thread) {
        pthread_mutex_unlock(&mutex);
        return NULL;
    }
    set_task_to_thread(thread, task);

    pthread_mutex_unlock(&mutex);

    return task;
}

p_task start_task(threadpool_task_callback callback, void *args) {
    return run_task(make_task(callback, args));
}

void *await_task(p_task task) {
    if (!task)
        return NULL;

    p_task_metadata metadata = task->metadata;

    clock_t before = clock();
    long current_timestamp = 0;
    do {
        clock_t diff = clock() - before;
        current_timestamp = diff * 1000 / CLOCKS_PER_SEC;
    } while (metadata && !task->is_done && current_timestamp < TASK_WAITING_TIMEOUT);

    if (!metadata)
        return NULL;

    if (task->is_running && !task->is_done) {
        cancel_task(task);
        return NULL;
    }

    void *result = task->result;

    if (task->metadata->release_type == TASK_RELEASE_TYPE_DEFAULT) {
        emit_on_complete(task);
        if (task->metadata->release_callback)
            task->metadata->release_callback(task);
    }

    return result;
}

int cancel_task(p_task task) {
    if (!thread_pool)
        return IPEE_ERROR_CODE__THREADPOOL__SERVICE_UNINITIALIZED;

    if (!task || !task->metadata || !task->is_running)
        return 0;

    p_thread thread = task->metadata->thread;
    if (!thread)
        return 0;

    int cancel_result = pthread_cancel(thread->thread);
    if (cancel_result)
        return 0;

    thread->task = NULL;
    thread->is_busy = 0;
    thread->is_running = 1;
    pthread_create(&thread->thread, NULL, task_invoker, thread);

    default_task_release_callback(task);

    return 1;
}

p_task on_complete(p_task task, threadpool_complete_callback complete_callback, void *args) {
    if (!task)
        return NULL;

    subscribe_with_args(threadpool_context_name, task->metadata->task_event_name,
                        (observable_callback_with_args)complete_callback, args);

    return task;
}

p_task on_final(p_task task, threadpool_release_callback release_callback) {
    if (!task)
        return NULL;

    task->metadata->release_callback = release_callback;

    return task;
}

p_task as_immediate(p_task task) {
    if (!task)
        return NULL;

    task->metadata->release_type = TASK_RELEASE_TYPE_IMMEDIATE;

    return task;
}

p_task as_manual(p_task task) {
    if (!task)
        return NULL;

    task->metadata->release_type = TASK_RELEASE_TYPE_MANUAL;

    return task;
}

int destroy_thread_pool(void) {
    if (!thread_pool)
        return IPEE_ERROR_CODE__THREADPOOL__SERVICE_UNINITIALIZED;

    iterate_over_dictionary_values(
        thread_pool, (dictionary_iteration_values_callback)destroy_thread);
    pthread_cond_destroy(&pool_cond);
    pthread_mutex_destroy(&mutex);
    delete_dictionary(thread_pool);
    release_bitset(task_bitset);

    task_bitset = NULL;
    thread_pool = NULL;
    return 0;
}

/***********************************************************************************************
 * STATIC FUNCTIONS DEFINITIONS
 **********************************************************************************************/

static void *task_invoker(p_thread thread) {
    if (!thread)
        return NULL;

    pthread_mutex_lock(&mutex);
    while (thread->is_running) {
        while (thread->is_running && !thread->task)
            pthread_cond_wait(&thread->cond, &mutex);

        if (!thread->task)
            break;

        p_task task = thread->task;
        pthread_mutex_unlock(&mutex);

        p_task_metadata metadata = task->metadata;
        if (metadata) {
            task->is_running = 1;
            void *result = metadata->callback(metadata->args);
            task->result = result;
            task->is_done = 1;
            if (task->metadata->release_type == TASK_RELEASE_TYPE_IMMEDIATE) {
                emit_on_complete(task);
                if (task->metadata->release_callback)
                    task->metadata->release_callback(task);
            }
        }

        pthread_mutex_lock(&mutex);
        thread->task = NULL;
        thread->is_busy = 0;
        pthread_cond_signal(&pool_cond);
    }
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
    return NULL;
}

static void set_task_to_thread(p_thread thread, p_task task) {
    if (!thread || !task)
        return;

    thread->is_busy = 1;
    thread->task = task;
    task->metadata->thread = thread;
    pthread_cond_signal(&thread->cond);
}

static int filter_threads(const p_record record, int _1, const p_dictionary _2) {
    if (!record || !record->value)
        return 0;

    const p_thread thread = (p_thread)record->value;
    return !thread->is_busy;
}

static void destroy_thread(p_thread thread) {
    if (!thread)
        return;

    pthread_mutex_lock(&mutex);
    thread->is_running = 0;
    pthread_cond_signal(&thread->cond);
    pthread_mutex_unlock(&mutex);

    pthread_join(thread->thread, NULL);
    pthread_cond_destroy(&thread->cond);
    free(thread);
}

static void default_task_release_callback(p_task task) {
    if (!task || !task->metadata)
        return;

    p_task_metadata metadata = task->metadata;
    free(task->metadata->task_event_name);
    task->metadata->task_event_name = NULL;
    task->metadata->thread = NULL;
    reset_bit(task_bitset, task->metadata->task_id);
    free(metadata);

    task->metadata = NULL;
    task->is_done = 0;
    task->is_running = 0;
    task->result = NULL;
    free(task);
}

static void emit_on_complete(p_task task) {
    if (!task)
        return;

    const char *event_name = task->metadata->task_event_name;
    p_dictionary subscribers = get_context_event_subscribers(threadpool_context_name, event_name);
    if (subscribers) {
        notify(threadpool_context_name, event_name, task->result);
        unsubscribe_from_event(threadpool_context_name, event_name);
    }
}
