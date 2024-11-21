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
#include <stdio.h>

#include <hashmap.h>

typedef struct str_type_s {
    p_key key;
    char *val;
} str_type_t, *p_str_type;

// typedef struct test_type_s {
//     int a;
//     char b;
//     void *ptr;
// } test_type_t, *p_test_type;

/*********************************************************************************************
 * STATIC VARIABLES
 ********************************************************************************************/

static str_type_t str_arr[5] = {{.key = "firstKey", .val = "firstValue"},
                                {.key = "secondKey", .val = "secondValue"},
                                {.key = "thirdKey", .val = "thirdValue"},
                                {.key = "fourthKey", .val = "fourthValue"},
                                {.key = "fifthKey", .val = "fifthValue"}};

/***********************************************************************************************
 * STATIC FUNCTIONS DECLARATIONS
 **********************************************************************************************/

static void iterate_str_map_callback(p_key key, void *value);

/*********************************************************************************************
 * FUNCTIONS DECLARATIONS
 ********************************************************************************************/

/**
 * @brief Check hashmap collection to get string value by key.
 * 
 * @return Error code.
 */
int hashmap_getStrValidValue_OK(void);

/**
 * @brief Check hashmap collection to remove string value by key.
 * 
 * @return Error code.
 */
int hashmap_removeStrValue_OK(void);

/**
 * @brief Check hashmap collection to remove all string values.
 * 
 * @return Error code.
 */
int hashmap_removeAllEntries_OK(void);

/**
 * @brief Check hashmap collection to get count of items.
 * 
 * @return Error code.
 */
int hashmap_getCount_OK(void);

/**
 * @brief Check hashmap collection to iterate over.
 * 
 * @return Error code.
 */
int hashmap_iterateCallback_OK(void);

/*********************************************************************************************
 * FUNCTIONS DEFINITIONS
 ********************************************************************************************/

int hashmap_test(int argc, char *argv[]) {
    int exit_result = 0;

    exit_result |= hashmap_getStrValidValue_OK();
    exit_result |= hashmap_removeStrValue_OK();
    exit_result |= hashmap_removeAllEntries_OK();
    exit_result |= hashmap_getCount_OK();
    exit_result |= hashmap_iterateCallback_OK();

    return exit_result;
}

int hashmap_getStrValidValue_OK(void) {
    p_hashmap map = hashmap_create();

    for (int i = 0; i < 5; i++) {
        hashmap_set_entry(map, str_arr[i].key, str_arr[i].val);
    }

    const char *expected = "thirdValue";
    const char *actual = hashmap_get_entry(map, "thirdKey");

    const int result = !actual ? 0 : is_equal(expected, actual);

    hashmap_remove(&map);

    return ORDER_RESULT(result, 0);
}

int hashmap_removeStrValue_OK(void) {
    p_hashmap map = hashmap_create();

    for (int i = 0; i < 5; i++) {
        hashmap_set_entry(map, str_arr[i].key, str_arr[i].val);
    }

    hashmap_remove_entry(map, "thirdKey");

    const char *expected = "thirdValue";
    const char *actual = hashmap_get_entry(map, "thirdKey");

    const int result = !actual;

    hashmap_remove(&map);

    return ORDER_RESULT(result, 1);
}

int hashmap_removeAllEntries_OK(void) {
    char flag = 1;
    p_hashmap map = hashmap_create();

    for (int i = 0; i < 5; i++) {
        hashmap_set_entry(map, str_arr[i].key, str_arr[i].val);
    }

    hashmap_remove_all_entries(map);

    for (int i = 0; i < 5; i++) {
        const char *actual = hashmap_get_entry(map, str_arr[i].key);

        if (actual) {
            flag = 0;
        }
    }

    const int result = flag;

    hashmap_remove(&map);

    return ORDER_RESULT(result, 3);
}

int hashmap_getCount_OK(void) {
    char flag = 0;
    p_hashmap map = hashmap_create();

    for (int i = 0; i < 5; i++) {
        hashmap_set_entry(map, str_arr[i].key, str_arr[i].val);
    }

    int count = hashmap_get_count(map);

    int result = count == 5;

    hashmap_remove_entry(map, "thirdKey");
    count = hashmap_get_count(map);

    result &= count == 4;

    hashmap_remove(&map);

    return ORDER_RESULT(result, 4);
}

int hashmap_iterateCallback_OK(void) {
    p_hashmap map = hashmap_create();

    for (int i = 0; i < 5; i++) {
        hashmap_set_entry(map, str_arr[i].key, str_arr[i].val);
    }

    hashmap_iterate(map, iterate_str_map_callback);

    const char *expected = "HhirdValue";
    const char *actual = hashmap_get_entry(map, "thirdKey");

    const int result = !actual ? 0 : is_equal(expected, actual);

    hashmap_remove(&map);

    return ORDER_RESULT(result, 5);
}

/***********************************************************************************************
 * STATIC FUNCTIONS DEFINITIONS
 **********************************************************************************************/

static void iterate_str_map_callback(p_key key, void *value) {
    char *str = (char*)value;
    // char *test = "Tokarev";
    
    if (is_equal(key, "thirdKey")) {
        // memcpy(value, test, strlen(test) + 1);
        // sprintf(value, "%s", test);
        str[0] = 'H';
    }
}