#include <threadpool.h>

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <dictionary.h>
#include <bitset.h>
#include <event.h>
#include <macro.h>

/*******************************************************************************
 * STRUCTS DECLARATIONS
 ******************************************************************************/

typedef struct __impl_thread_s __impl_thread_t, *__p_impl_thread;

/**
 * @brief Task metadata.
 */
typedef struct __impl_task_metadata_s {
    char               *task_event_name;             // Task event name.
    int                 task_priority;               // Task priority.
    long                task_delay;                  // Task delay.
    long                task_interval;               // Task timeout.
    long                task_interval_left_count;    // Task interval count limit.
    int                 task_interval_infinity_loop; // Task interval count limit.
    long                last_checking_time;          // Task last checking time.
    long                left_time_to_invokation;     // Time to invokation.
    __p_impl_thread     thread;                      // Thread.
    void               *(*callback)(void *args);     // Task callback.
    void               *args;                        // Task callback arguments.
    task_release_type_t release_type;                // Task release type.
} __impl_task_metadata_t, *__p_impl_task_metadata;

/**
 * @brief Task information.
 */
typedef struct __impl_task_s {
    volatile int    is_done;         // Task is done.
    volatile int    is_running;      // Task is running.
    int             id;              // Task identifier.
    void           *result;          // Task result.
    __p_impl_task_metadata metadata; // Task metainformation.
} __impl_task_t, *__p_impl_task;

/**
 * @brief Thread data.
 */
typedef struct __impl_thread_s {
    pthread_t              thread;     // Thread Id.
    int                    is_buzy;    // Thread is buzy.
    volatile int           is_running; // Thread is running.
    volatile __p_impl_task task;       // Current task.
} __impl_thread_t, *__p_impl_thread;

/*******************************************************************************
 * STATIC FUNCTIONS DECLARATIONS
 ******************************************************************************/

/**
 * @brief Invokes callback of a task in a thread.
 *
 * @param thread The thread.
 *
 * @return Unused.
 */
static void *task_processor(__p_impl_thread thread);

/**
 * @brief Set the task to thread object.
 *
 * @param thread The thread.
 * @param task   The task.
 */
static void set_task_to_thread(__p_impl_thread thread, __p_impl_task task);

/**
 * @brief Filter available threads.
 *
 * @param record Record.
 * @param _1     Unused.
 * @param _2     Unused.
 *
 * @return Result of comparsion.
 */
static int filter_threads(const p_record record, int _1, const p_dictionary _2);

/**
 * @brief Free task.
 *
 * @param task The task.
 */
static void task_release_callback(__p_impl_task task);

/**
 * @brief Destroy thread.
 *
 * @param thread The thread.
 */
static void destroy_thread(const __p_impl_thread thread);

/**
 * @brief Emit on complete task.
 *
 * @param task The task.
 */
static void emit_on_complete(__p_impl_task task);

/**
 * @brief Controls the task queue.
 */
static void *task_queue_scheduler();

/**
 * @brief Controls the task queue.
 */
static void *task_queue_scheduler();

/**
 * @brief Sorts the task queue by priority.
 */
static int sort_task_queue(const p_record record1, const p_record record2);

/**
 * @brief Updates the priority of queue tasks.
 */
static void update_task_priority(void *value);

/**
 * @brief Returns current clocks.
 */
static inline long current_clock_ms();

/**
 * @brief Renew task.
 *
 * @param task Source task.
 *
 * @return Updated task.
 */
static __p_impl_task prepare_next_iteration_task(__p_impl_task task);

/*******************************************************************************
 * STATIC VARIABLES
 ******************************************************************************/

/**
 * @brief The thread pool.
 */
static p_dictionary thread_pool = NULL;

/**
 * @brief The task queue.
 */
static p_dictionary task_queue = NULL;

/**
 * @brief The task bitset.
 */
static p_bitset task_bitset = NULL;

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

/*******************************************************************************
 * FUNCTIONS DEFINITIONS
 ******************************************************************************/

void set_threadpool_size(int size) {
    if (thread_pool)
        exit(IPEE_ERROR_CODE__THREADPOOL__SERVICE_ALREADY_INITIALIZED);

    THREAD_POOL_SIZE = size;
}

void set_internal_task_counter_limit(int limit) {
    if (thread_pool)
        exit(IPEE_ERROR_CODE__THREADPOOL__SERVICE_ALREADY_INITIALIZED);

    INTERNAL_TASK_COUNTER_LIMIT = limit;
}

void set_task_waiting_timeout(int timeout) {
    TASK_WAITING_TIMEOUT = timeout;
}

void init_thread_pool(void) {
    if (!thread_pool && THREAD_POOL_SIZE > 0) {
        pthread_mutex_init(&mutex, NULL);
        pthread_mutex_lock(&mutex);

        thread_pool = create_dictionary();
        task_queue = create_dictionary();
        task_bitset = create_bitset(INTERNAL_TASK_COUNTER_LIMIT);

        p_task scheduler_task = (p_task)make_task(task_queue_scheduler, NULL);

        for (int i = 0; i < THREAD_POOL_SIZE; i++) {
            __p_impl_thread thread = (__p_impl_thread)malloc(sizeof(__impl_thread_t));
            if (!thread)
                exit(IPEE_ERROR_CODE__THREADPOOL__THREAD_CREATION_ERROR);

            thread->is_buzy = 0;
            thread->is_running = 1;
            thread->task = NULL;

            pthread_create(&thread->thread, NULL, task_processor, thread);
            add_record_to_dictionary(thread_pool, i, thread);
        }

        __p_impl_thread thread = NULL;
        if (thread = get_head_value_from_dictionary(thread_pool)) {
            set_task_to_thread(thread, scheduler_task);
        }

        pthread_mutex_unlock(&mutex);
    }
}

p_task make_task(threadpool_task_callback callback, void *args) {
    __p_impl_task impl_task = (__p_impl_task)malloc(sizeof(__impl_task_t));
    if (!impl_task)
        return NULL;

    impl_task->is_done = 0;
    impl_task->is_running = 0;
    impl_task->result = NULL;
    impl_task->metadata = NULL;

    int internal_task_counter = get_first_free_bit(task_bitset);
    if (internal_task_counter == -1)
        return NULL;

    set_bit(task_bitset, internal_task_counter);
    impl_task->id = internal_task_counter;

    __p_impl_task_metadata metadata = (__p_impl_task_metadata)malloc(sizeof(__impl_task_metadata_t));
    if (!metadata)
        return NULL;

    metadata->task_event_name = prepare_event_name(threadpool_context_name, threadpool_complete_event_name, impl_task->id);
    metadata->callback = callback;
    metadata->args = args;
    metadata->thread = NULL;
    metadata->release_type = TASK_RELEASE_TYPE_DEFAULT;
    metadata->task_priority = 5;
    metadata->task_delay = 0;
    metadata->task_interval = 0;
    metadata->last_checking_time = 0;
    metadata->left_time_to_invokation = 0;
    metadata->task_interval_left_count = 1;
    metadata->task_interval_infinity_loop = 0;

    impl_task->metadata = metadata;

    return (p_task)impl_task;
}

int cancel_task(p_task task) {
    __p_impl_task impl_task = (__p_impl_task)task;
    if (!thread_pool)
        exit(IPEE_ERROR_CODE__THREADPOOL__SERVICE_UNINITIALIZED);

    if (!impl_task || !impl_task->metadata || !impl_task->is_running)
        return 0;

    __p_impl_thread thread = impl_task->metadata->thread;
    if (!thread)
        return 0;

    int cancel_result = pthread_cancel(thread->thread);
    if (cancel_result)
        return 0;

    thread->task = NULL;
    thread->is_buzy = 0;
    thread->is_running = 1;
    pthread_create(&thread->thread, NULL, task_processor, thread);

    task_release_callback(impl_task);

    return 1;
}

p_task on_complete(p_task task, threadpool_complete_callback complete_callback, void *args) {
    __p_impl_task impl_task = (__p_impl_task)task;
    if (!impl_task)
        exit(IPEE_ERROR_CODE__THREADPOOL__INVALID_TASK);

    subscribe_with_args(threadpool_context_name, impl_task->metadata->task_event_name, complete_callback, args);

    return task;
}

p_task with_delay(p_task task, int delay) {
    __p_impl_task impl_task = (__p_impl_task)task;
    if (!impl_task)
        exit(IPEE_ERROR_CODE__THREADPOOL__INVALID_TASK);

    impl_task->metadata->task_delay = delay;

    return task;
}

p_task with_interval(p_task task, int interval, int count) {
    __p_impl_task impl_task = (__p_impl_task)task;
    if (!impl_task)
        exit(IPEE_ERROR_CODE__THREADPOOL__INVALID_TASK);

    impl_task->metadata->task_interval = interval;
    impl_task->metadata->task_interval_left_count = count;
    if (!count)
        impl_task->metadata->task_interval_infinity_loop = 1;

    return task;
}

p_task with_priority(p_task task, int priority) {
    __p_impl_task impl_task = (__p_impl_task)task;
    if (!impl_task)
        exit(IPEE_ERROR_CODE__THREADPOOL__INVALID_TASK);

    impl_task->metadata->task_priority = priority;

    return task;
}

p_task as_manual(p_task task) {
    __p_impl_task impl_task = (__p_impl_task)task;
    if (!impl_task)
        exit(IPEE_ERROR_CODE__THREADPOOL__INVALID_TASK);

    impl_task->metadata->release_type = TASK_RELEASE_TYPE_MANUAL;

    return task;
}

p_task run_task(p_task task) {
    __p_impl_task impl_task = (__p_impl_task)task;
    return run_task_with_args(task, impl_task->metadata->args);
}

p_task run_task_with_args(p_task task, void *args) {
    __p_impl_task impl_task = (__p_impl_task)task;
    if (!thread_pool)
        exit(IPEE_ERROR_CODE__THREADPOOL__SERVICE_UNINITIALIZED);
    if (!impl_task)
        exit(IPEE_ERROR_CODE__THREADPOOL__INVALID_TASK);

    pthread_mutex_lock(&mutex);

    impl_task->metadata->args = args;
    impl_task->metadata->last_checking_time = current_clock_ms();
    impl_task->metadata->left_time_to_invokation = impl_task->metadata->task_delay;
    add_record_to_dictionary(task_queue, impl_task->id, impl_task);

    pthread_mutex_unlock(&mutex);

    return task;
}

p_task start_task(threadpool_task_callback callback, void *args) {
    return run_task(make_task(callback, args));
}

void *await_task(p_task task) {
    __p_impl_task impl_task = (__p_impl_task)task;
    if (!impl_task)
        exit(IPEE_ERROR_CODE__THREADPOOL__INVALID_TASK);

    __p_impl_task_metadata metadata = impl_task->metadata;

    long before = current_clock_ms();
    long current_timestamp = 0;
    do {
        current_timestamp = current_clock_ms() - before;
    } while (metadata && !impl_task->is_done && current_timestamp < TASK_WAITING_TIMEOUT);

    if (!metadata)
        return NULL;

    if (impl_task->is_running && !impl_task->is_done) {
        cancel_task(task);
        return NULL;
    }

    return task->result;
}

void destroy_thread_pool(void) {
    if (!thread_pool)
        exit(IPEE_ERROR_CODE__THREADPOOL__SERVICE_UNINITIALIZED);

    delete_dictionary(task_queue);
    task_queue = NULL;

    iterate_over_dictionary_values(thread_pool, destroy_thread);
    pthread_mutex_destroy(&mutex);
    delete_dictionary(thread_pool);
    thread_pool = NULL;

    delete_bitset(task_bitset);
    task_bitset = NULL;
}

/*******************************************************************************
 * STATIC FUNCTIONS DEFINITIONS
 ******************************************************************************/

static void *task_processor(__p_impl_thread thread) {
    if (!thread)
        exit(IPEE_ERROR_CODE__THREADPOOL__INVALID_THREAD);

    while (thread->is_running) {
        __p_impl_task task = thread->task;
        if (!task || task->is_running)
            continue;
        task->is_running = 1;

        __p_impl_task_metadata metadata = task->metadata;
        if (!metadata)
            continue;

        void *result = metadata->callback(metadata->args);
        task->result = result;
        task->is_done = 1;

        emit_on_complete(task);
        if (metadata->task_interval_left_count > 1 || metadata->task_interval_infinity_loop) {
            __p_impl_task next_task = prepare_next_iteration_task(task);
            run_task(next_task);
        } else if (task->metadata->release_type == TASK_RELEASE_TYPE_DEFAULT) {
            task_release_callback(task);
        }

        thread->task = NULL;
        thread->is_buzy = 0;
    }

    pthread_exit(NULL);
    return NULL;
}

static void set_task_to_thread(__p_impl_thread thread, __p_impl_task task) {
    if (!thread)
        exit(IPEE_ERROR_CODE__THREADPOOL__INVALID_THREAD);
    if (!task)
        exit(IPEE_ERROR_CODE__THREADPOOL__INVALID_TASK);

    thread->is_buzy = 1;
    thread->task = task;
    task->metadata->thread = thread;
}

static int filter_threads(const p_record record, int _1, const p_dictionary _2) {
    const __p_impl_thread thread = (__p_impl_thread)record->value;
    if (!record || !record->value)
        exit(IPEE_ERROR_CODE__THREADPOOL__INVALID_THREAD);

    return !thread->is_buzy;
}

static void destroy_thread(__p_impl_thread thread) {
    if (!thread)
        exit(IPEE_ERROR_CODE__THREADPOOL__INVALID_THREAD);

    pthread_mutex_lock(&mutex);

    thread->is_running = 0;
    if (thread->is_buzy) {
        p_task task = (p_task)thread->task;
        if (task)
            cancel_task(task);
    }
    pthread_cancel(thread->thread);
    free(thread);

    pthread_mutex_unlock(&mutex);
}

static void task_release_callback(__p_impl_task task) {
    if (!task || !task->metadata)
        exit(IPEE_ERROR_CODE__THREADPOOL__INVALID_TASK);

    __p_impl_task_metadata metadata = task->metadata;
    free(task->metadata->task_event_name);
    task->metadata->task_event_name = NULL;
    task->metadata->thread = NULL;
    reset_bit(task_bitset, task->id);
    free(metadata);

    task->metadata = NULL;
    task->is_done = 0;
    task->is_running = 0;
    task->result = NULL;
    free(task);
}

static void emit_on_complete(__p_impl_task task) {
    if (!task)
        exit(IPEE_ERROR_CODE__THREADPOOL__INVALID_TASK);

    const char *event_name = task->metadata->task_event_name;
    p_dictionary subscribers = get_context_event_subscribers(threadpool_context_name, event_name);
    if (subscribers) {
        notify(threadpool_context_name, event_name, task->result);
        unsubscribe_from_event(threadpool_context_name, event_name);
    }
}

static void *task_queue_scheduler() {
    while(thread_pool) {
        if (!task_queue->size)
            continue;
            
        const p_dictionary sorted_queue = sort_dictionary(task_queue, sort_task_queue);
        iterate_over_dictionary_values(sorted_queue, update_task_priority);
        p_dictionary old_queue = task_queue;
        task_queue = sorted_queue;
        delete_dictionary(old_queue);

        __p_impl_task task = (__p_impl_task)get_head_value_from_dictionary(task_queue);
        if (!task)
            continue;

        __p_impl_task_metadata metadata = task->metadata;
        if (metadata->left_time_to_invokation > 5)
            continue;

        p_dictionary available_threads = NULL;
        do {
            if (available_threads)
                delete_dictionary(available_threads);
            available_threads = filter_dictionary(thread_pool, filter_threads);
        } while (!available_threads->size);

        __p_impl_thread thread = get_head_value_from_dictionary(available_threads);
        delete_dictionary(available_threads);

        if (thread) {
            remove_record_from_dictionary_by_index(task_queue, 0);
            set_task_to_thread(thread, task);
        } else
            exit(IPEE_ERROR_CODE__THREADPOOL__INVALID_THREAD);
    }
}

static int sort_task_queue(const p_record record1, const p_record record2) {
    __p_impl_task task1 = (__p_impl_task)record1->value;
    __p_impl_task task2 = (__p_impl_task)record2->value;

    return task1->metadata->task_priority > task2->metadata->task_priority;
}

static void update_task_priority(void *value) {
    __p_impl_task task = (__p_impl_task)value;

    const long current_time_in_ms = current_clock_ms();
    const long last_checking_time = task->metadata->last_checking_time;
    task->metadata->last_checking_time = current_time_in_ms;

    const long diff = current_time_in_ms - last_checking_time;
    task->metadata->left_time_to_invokation -= diff;

    if (task->metadata->left_time_to_invokation <= 5)
        task->metadata->task_priority = 0;
    else if (task->metadata->left_time_to_invokation >= 5000)
        task->metadata->task_priority++;
    else if (task->metadata->task_priority > 0)
        task->metadata->task_priority--;
}

static inline long current_clock_ms() {
    return clock() * 1000 / CLOCKS_PER_SEC;
}

static __p_impl_task prepare_next_iteration_task(__p_impl_task task) {
    task->is_done = 0;
    task->is_running = 0;
    task->result = NULL;

    __p_impl_task_metadata metadata = task->metadata;
    metadata->thread = NULL;
    metadata->task_priority = 5;
    metadata->task_delay = metadata->task_interval;
    metadata->task_interval_left_count--;

    task->metadata = metadata;

    return task;
}