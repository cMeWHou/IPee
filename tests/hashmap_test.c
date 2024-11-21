/**
 * @file hashmap_test.c
 * @author Bulat Ramazanov (ramzesbulman@gmail.com)
 * @brief Hashmap tests
 * @version 1.0
 * @date 2024-11-21
 *
 * @copyright Copyright (c) 2024
 */

#include "utils/helper.h"

#include <string.h>

#include <hashmap.h>

/*********************************************************************************************
 * FUNCTIONS DECLARATIONS
 ********************************************************************************************/

/**
 * @brief Check hashmap collection to get integer value by key.
 * 
 * @return Error code.
 */
int hashmap_getIntValidValue_OK(void);

/**
 * @brief Check hashmap collection to get string value by key.
 * 
 * @return Error code.
 */
int hashmap_getStrValidValue_OK(void);

/*********************************************************************************************
 * FUNCTIONS DEFINITIONS
 ********************************************************************************************/

int hashmap_test(int argc, char *argv[]) {
    int exit_result = 0;

    exit_result |= hashmap_getIntValidValue_OK();
    exit_result |= hashmap_getStrValidValue_OK();

    return exit_result;
}

int hashmap_getIntValidValue_OK(void) {
    p_hashmap map = hashmap_create();
    
    int *key = 1;

    hashmap_set_entry(map, HASHMAP_STATIC_KEY(*key++), 100);
    hashmap_set_entry(map, HASHMAP_STATIC_KEY(*key++), 200);
    hashmap_set_entry(map, HASHMAP_STATIC_KEY(*key++), 300);
    hashmap_set_entry(map, HASHMAP_STATIC_KEY(*key++), 400);
    hashmap_set_entry(map, HASHMAP_STATIC_KEY(*key++), 500);

    const int *expected_key = 3;
    const int *expected_value = 300;
    const int *actual = hashmap_get_entry(map, HASHMAP_STATIC_KEY(expected_key));

    const int result = expected_value == actual;

    hashmap_remove(&map);

    return ORDER_RESULT(result, 0);
}

int hashmap_getStrValidValue_OK(void) {
    p_hashmap map = hashmap_create();

    hashmap_set_entry(map, HASHMAP_STR_KEY("firstKey"), "firstValue");
    hashmap_set_entry(map, HASHMAP_STR_KEY("secondKey"), "secondValue");
    hashmap_set_entry(map, HASHMAP_STR_KEY("thirdKey"), "thirdValue");
    hashmap_set_entry(map, HASHMAP_STR_KEY("fourthKey"), "fourthValue");
    hashmap_set_entry(map, HASHMAP_STR_KEY("fifthKey"), "fifthValue");

    const char *expected = "thirdValue";
    const char *actual = hashmap_get_entry(map, HASHMAP_STR_KEY("thirdKey"));

    const int result = is_equal(expected, actual);

    hashmap_remove(&map);

    return ORDER_RESULT(result, 1);
}