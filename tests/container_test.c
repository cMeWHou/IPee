/**
 * @file container.test.c
 * @author chcp (cmewhou@yandex.ru)
 * @brief Container tests
 * @version 1.0
 * @date 2024-11-13
 *
 * @copyright Copyright (c) 2024
 */

#include "utils/helper.h"

#include <container.h>

#include <stdlib.h>

/*********************************************************************************************
 * FUNCTIONS DECLARATIONS
 ********************************************************************************************/

/** @brief Dictionary collection test. */
int container_getValidValue_OK(void);

/*********************************************************************************************
 * FUNCTIONS DEFINITIONS
 ********************************************************************************************/

int container_test(int argc, char *argv[]) {
    int exit_result = 0;

    exit_result |= container_getValidValue_OK();

    return exit_result;
}

int container_getValidValue_OK(void) {
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
    return ORDER_RESULT(result, 0);
}