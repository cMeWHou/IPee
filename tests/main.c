/**
 * @file test.c
 * @author chcp (cmewhou@yandex.ru)
 * @brief Common container for global access to application services.
 * @version 1.0
 * @date 2024-09-30
 *
 * @copyright Copyright (c) 2024
 */

#include <string.h>

#include <container.h>
#include <dictionary.h>
#include <event.h>
#include <threadpool.h>

#define ORDER_RESULT(result, order_num) (!result) << order_num

/***********************************************************************************************
 * STATIC FUNCTIONS DECLARATIONS
 **********************************************************************************************/

/**
 * @brief Compare two strings.
 * 
 * @param a First string.
 * @param b Second string.
 * @return 1 if equal, 0 else.
 */
static int is_equal(const char *a, const char *b);

/**
 * @brief Event test callback.
 * 
 * @param args Arguments.
 * @param captured_args Captured arguments.
 */
static void event_test_callback(void *args, void *captured_args);

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

/** @brief Dictionary collection test. */
int test_dictionary_OK(void);

/** @brief Container test. */
int test_container_OK(void);

/** @brief Event test. */
int test_event_OK(void);

/** @brief Threadpool test. */
int test_threadpool_OK(void);

/*********************************************************************************************
 * FUNCTIONS DEFINITIONS
 ********************************************************************************************/

int main(void) {
    int exit_result = 0;

    exit_result |= test_dictionary_OK();
    exit_result |= test_container_OK();
    exit_result |= test_event_OK();
    exit_result |= test_threadpool_OK();

    return exit_result;
}

int test_dictionary_OK(void) {
    p_dictionary dictionary = create_dictionary();

    add_record_to_dictionary(dictionary, "firstKey", "firstValue");
    add_record_to_dictionary(dictionary, "secondKey", "secondValue");
    add_record_to_dictionary(dictionary, "thirdKey", "thirdValue");
    add_record_to_dictionary(dictionary, "fourthKey", "fourthValue");

    const char *expected = "thirdValue";
    const char *actual = get_value_from_dictionary(dictionary, "thirdKey");
    delete_dictionary(dictionary);

    const int result = is_equal(expected, actual);
    return ORDER_RESULT(result, 0);
}

int test_container_OK(void) {
    const char *context = "test";
    p_container container = init_container(context);

    add_glblvalue_to_container(container, "firstKey", "firstValue", NULL);
    add_glblvalue_to_container(container, "secondKey", "secondValue", NULL);
    add_glblvalue_to_container(container, "thirdKey", "thirdValue", NULL);
    add_glblvalue_to_container(container, "fourthKey", "fourthValue", NULL);

    const char *expected = "thirdValue";
    const char *actual = get_service_from_container(container, "thirdKey");
    release_all_containers();

    const int result = is_equal(expected, actual);
    return ORDER_RESULT(result, 1);
}

int test_event_OK(void) {
    const char *event = "test";
    char actual[10];
    global_subscribe_with_args(event, event_test_callback, actual);

    const char *expected = "testValue";
    global_notify(event, NULL);
    global_unsubscribe(event, event_test_callback);

    const int result = is_equal(expected, actual);
    return ORDER_RESULT(result, 2);
}

int test_threadpool_OK(void) {
    init_thread_pool();
    const char *event = "test";

    char *expected = "testValue";
    p_task task = make_task(threadpool_test_callback, expected);
    char *actual = await_task(task);
    destroy_thread_pool();

    const int result = is_equal(expected, actual);
    return ORDER_RESULT(result, 3);
}

/***********************************************************************************************
 * STATIC FUNCTIONS DEFINITIONS
 **********************************************************************************************/

static int is_equal(const char *a, const char *b) { 
    return strcmp(a, b) == 0; 
}

static void event_test_callback(void *arg, void *captured_args) {
    strcpy(captured_args, "testValue");
}

static void *threadpool_test_callback(void *args) {
    return args; 
}