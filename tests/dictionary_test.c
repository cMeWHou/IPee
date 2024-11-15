/**
 * @file dictionary.test.c
 * @author chcp (cmewhou@yandex.ru)
 * @brief Dictionary tests
 * @version 1.0
 * @date 2024-11-13
 *
 * @copyright Copyright (c) 2024
 */

#include "utils/helper.h"

#include <dictionary.h>

/*********************************************************************************************
 * FUNCTIONS DECLARATIONS
 ********************************************************************************************/

/** @brief Dictionary collection initialisation test. */
int dictionary_getValidValue_OK(void);

/*********************************************************************************************
 * FUNCTIONS DEFINITIONS
 ********************************************************************************************/

int dictionary_test(int argc, char *argv[]) {
    int exit_result = 0;

    exit_result |= dictionary_getValidValue_OK();

    return exit_result;
}

int dictionary_getValidValue_OK(void) {
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