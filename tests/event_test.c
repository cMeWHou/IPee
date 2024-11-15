/**
 * @file event.test.c
 * @author chcp (cmewhou@yandex.ru)
 * @brief Event tests
 * @version 1.0
 * @date 2024-11-13
 *
 * @copyright Copyright (c) 2024
 */

#include "utils/helper.h"

#include <event.h>

#include <string.h>
#include <stdlib.h>

/***********************************************************************************************
 * STATIC FUNCTIONS DECLARATIONS
 **********************************************************************************************/

/**
 * @brief Event test callback.
 * 
 * @param args Arguments.
 * @param captured_args Captured arguments.
 */
static void event_test_callback(void *args, void *captured_args);

/*********************************************************************************************
 * FUNCTIONS DECLARATIONS
 ********************************************************************************************/

/** @brief Send notification. */
int event_notify_OK(void);

/*********************************************************************************************
 * FUNCTIONS DEFINITIONS
 ********************************************************************************************/

int event_test(int argc, char *argv[]) {
    int exit_result = 0;

    exit_result |= event_notify_OK();

    return exit_result;
}

int event_notify_OK(void) {
    const char *event = "test";
    char actual[10];
    global_subscribe_with_args(event, event_test_callback, actual);

    const char *expected = "testValue";
    global_notify(event, NULL);
    global_unsubscribe(event, event_test_callback);

    const int result = is_equal(expected, actual);
    return ORDER_RESULT(result, 0);
}

/***********************************************************************************************
 * STATIC FUNCTIONS DEFINITIONS
 **********************************************************************************************/

static void event_test_callback(void *arg, void *captured_args) {
    strcpy(captured_args, "testValue");
}