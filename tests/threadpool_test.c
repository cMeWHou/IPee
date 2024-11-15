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
 * @brief Task test callback.
 * 
 * @param args Arguments.
 * @return Return value.
 */
static void *threadpool_test_callback(void *args);

/*********************************************************************************************
 * FUNCTIONS DECLARATIONS
 ********************************************************************************************/

/** @brief Threadpool test. */
int threadpool_awaitTask_OK(void);

/*********************************************************************************************
 * FUNCTIONS DEFINITIONS
 ********************************************************************************************/

int threadpool_test(int argc, char *argv[]) {
    int exit_result = 0;

    exit_result |= threadpool_awaitTask_OK();

    return exit_result;
}

int threadpool_awaitTask_OK(void) {
    init_thread_pool();

    char *expected = "testValue";
    void *actual[10];

    p_task task = start_task_with_args(threadpool_test_callback, actual);
    void *res = await_task(task);

    destroy_thread_pool();

    const int result = is_equal(expected, actual) && is_equal(expected, res);
    return ORDER_RESULT(result, 0);
}

/***********************************************************************************************
 * STATIC FUNCTIONS DEFINITIONS
 **********************************************************************************************/

static void *threadpool_test_callback(void *args) {
    strcpy(args, "testValue");
    return args;
}