/**
* @file threadpool.test.c
* @author chcp (cmewhou@yandex.ru)
* @brief Threadpool tests.
* @version 1.0
* @date 2024-11-13
*
* @copyright Copyright (c) 2024
*/

#include "utils/helper.h"

#include <threadpool.h>

#include <string.h>
#include <stdlib.h>

/***********************************************************************************************
* STATIC FUNCTIONS DECLARATIONS
**********************************************************************************************/

/**
* @brief Task function.
* 
* @param args Arguments.
* @return Return value.
*/
static void *threadpool_function_callback(void *args);

/**
* @brief Task complete function.
* 
* @param result Task result.
* @param args Arguments.
* @return User-defined value.
*/
static void threadpool_complete_function_callback(void *result, void *args);

/**
* @brief Exceeding threads callback.
* 
* @param args Arguments.
* @return User-defined value.
*/
static void *threadpool_exceeding_callback(void *args);

/*********************************************************************************************
* FUNCTIONS DECLARATIONS
********************************************************************************************/

/** @brief Threadpool test. */
int threadpool_awaitTask_OK(void);

/** @brief Threadpool test. */
int threadpool_onComplete_OK(void);

/** @brief Threadpool test. */
int threadpool_exceedingThreads_OK(void);

/*********************************************************************************************
* FUNCTIONS DEFINITIONS
********************************************************************************************/

int threadpool_test(int argc, char *argv[]) {
    int exit_result = 0;
    set_threadpool_size(4);
    init_thread_pool();
    
    exit_result |= threadpool_awaitTask_OK();
    exit_result |= threadpool_onComplete_OK();
    exit_result |= threadpool_exceedingThreads_OK();
    
    destroy_thread_pool();
    return exit_result;
}

int threadpool_awaitTask_OK(void) {
    char *expected = "testValue";
    void *actual[10];
    
    p_task task = start_task(threadpool_function_callback, actual);
    void *res = await_task(task);
    
    const int result = is_equal(expected, actual) && is_equal(expected, res);
    return ORDER_RESULT(result, 0);
}

int threadpool_onComplete_OK(void) {
    char *expected = "valueTest";
    void *actual[10];
    
    p_task task = make_task(threadpool_function_callback, actual);
    on_complete(task, threadpool_complete_function_callback, NULL);
    run_task(task);
    void *res = await_task(task);
    
    const int result = is_equal(expected, res);
    return ORDER_RESULT(result, 1);
}

int threadpool_exceedingThreads_OK(void) {
    p_task task0 = run_task(make_task(threadpool_exceeding_callback, "actual"));
    p_task task1 = run_task(make_task(threadpool_exceeding_callback, "actual"));
    p_task task2 = run_task(make_task(threadpool_exceeding_callback, "actual"));
    p_task task3 = run_task(make_task(threadpool_exceeding_callback, "actual"));
    p_task task4 = run_task(make_task(threadpool_exceeding_callback, "actual"));
    p_task task5 = run_task(make_task(threadpool_exceeding_callback, "actual"));
    p_task task6 = run_task(make_task(threadpool_exceeding_callback, "actual"));
    p_task task7 = run_task(make_task(threadpool_exceeding_callback, "actual"));
    p_task task8 = run_task(make_task(threadpool_exceeding_callback, "actual"));
    p_task task9 = run_task(make_task(threadpool_exceeding_callback, "actual"));
    p_task task10 = run_task(make_task(threadpool_exceeding_callback, "actual"));
    p_task task11 = run_task(make_task(threadpool_exceeding_callback, "actual"));
    p_task task12 = run_task(make_task(threadpool_exceeding_callback, "actual"));
    p_task task13 = run_task(make_task(threadpool_exceeding_callback, "actual"));
    p_task task14 = run_task(make_task(threadpool_exceeding_callback, "actual"));
    p_task task15 = run_task(make_task(threadpool_exceeding_callback, "actual"));
    
    void *res = await_task(task15);
    return ORDER_RESULT(1, 2);
}

/***********************************************************************************************
* STATIC FUNCTIONS DEFINITIONS
**********************************************************************************************/

static void *threadpool_function_callback(void *args) {
    strcpy(args, "testValue");
    return args;
}

static void threadpool_complete_function_callback(void *result, void *args) {
    strcpy(result, "valueTest");
}

static void *threadpool_exceeding_callback(void *args) {
    return args;
}