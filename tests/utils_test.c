/**
 * @file utils.test.c
 * @author chcp (cmewhou@yandex.ru)
 * @brief Utils and miscellaneous tests.
 * @version 1.0
 * @date 2024-11-22
 *
 * @copyright Copyright (c) 2024
 */

#include "utils/helper.h"

#include <stdlib.h>

#include <utils.h>

/*********************************************************************************************
 * FUNCTIONS DECLARATIONS
 ********************************************************************************************/

/** @brief Dictionary collection initialisation test. */
int utils_tryDetectValueLocation_OK(int a);

/** @brief Compare two allocation types arrays. */
static int is_arrays_equal(allocation_type_t a[], allocation_type_t b[], int size);

int global_value_init = 0;
int global_value;

char *global_dynamic_init = 0;
char *global_dynamic;

char global_array_init[3] = { '1', '2', '3' };
char global_array[5];

static int global_static_init = 0;
static int global_static;

/*********************************************************************************************
 * FUNCTIONS DEFINITIONS
 ********************************************************************************************/

int utils_test(int argc, char *argv[]) {
    init_memory_config();

    int exit_result = 0;

    exit_result |= utils_tryDetectValueLocation_OK(1);

    return exit_result;
}

int utils_tryDetectValueLocation_OK(int a) {
    allocation_type_t expected[35];
    allocation_type_t actual[35];

    char *heap_value = malloc(1);
    (*heap_value) = '9';
    const char *heap_value3;
    const char *stack_value = "3";
    const char *stack_value3;
    const char simple_value_init = 0; 
    const char simple_value; 
    static int static_value_init = 0;
    static int static_value;   

    int index = 0;
    expected[index] = ALLOCATION_TYPE_NUMBER;
    actual[index++] = get_allocation_type(global_value_init);

    expected[index] = ALLOCATION_TYPE_GLOBAL;
    actual[index++] = get_allocation_type(&global_value_init);

    expected[index] = ALLOCATION_TYPE_NUMBER;
    actual[index++] = get_allocation_type(global_value);

    expected[index] = ALLOCATION_TYPE_GLOBAL;
    actual[index++] = get_allocation_type(&global_value);
    
    expected[index] = ALLOCATION_TYPE_NUMBER;
    actual[index++] = get_allocation_type(global_dynamic_init);

    expected[index] = ALLOCATION_TYPE_GLOBAL;
    actual[index++] = get_allocation_type(&global_dynamic_init);

    expected[index] = ALLOCATION_TYPE_NUMBER;
    actual[index++] = get_allocation_type(global_dynamic);
    
    expected[index] = ALLOCATION_TYPE_GLOBAL;
    actual[index++] = get_allocation_type(&global_dynamic);
    
    expected[index] = ALLOCATION_TYPE_GLOBAL;
    actual[index++] = get_allocation_type(global_array_init);
    
    expected[index] = ALLOCATION_TYPE_NUMBER;
    actual[index++] = get_allocation_type(*global_array_init);
    
    expected[index] = ALLOCATION_TYPE_GLOBAL;
    actual[index++] = get_allocation_type(&global_array_init);
    
    expected[index] = ALLOCATION_TYPE_GLOBAL;
    actual[index++] = get_allocation_type(global_array);
    
    expected[index] = ALLOCATION_TYPE_GLOBAL;
    actual[index++] = get_allocation_type(&global_array);
    
    expected[index] = ALLOCATION_TYPE_NUMBER;
    actual[index++] = get_allocation_type(global_static_init);
    
    expected[index] = ALLOCATION_TYPE_GLOBAL;
    actual[index++] = get_allocation_type(&global_static_init);

    expected[index] = ALLOCATION_TYPE_NUMBER;
    actual[index++] = get_allocation_type(global_static);
    
    expected[index] = ALLOCATION_TYPE_GLOBAL;
    actual[index++] = get_allocation_type(&global_static);
    
    expected[index] = ALLOCATION_TYPE_HEAP;
    actual[index++] = get_allocation_type(heap_value);
    
    expected[index] = ALLOCATION_TYPE_NUMBER;
    actual[index++] = get_allocation_type(*heap_value);
    
    expected[index] = ALLOCATION_TYPE_LOCAL;
    actual[index++] = get_allocation_type(&heap_value);
    
    expected[index] = ALLOCATION_TYPE_HEAP;
    actual[index++] = get_allocation_type(heap_value3);
    
    expected[index] = ALLOCATION_TYPE_LOCAL;
    actual[index++] = get_allocation_type(&heap_value3);
    
    expected[index] = ALLOCATION_TYPE_GLOBAL;
    actual[index++] = get_allocation_type(stack_value);
    
    expected[index] = ALLOCATION_TYPE_CHARACTER;
    actual[index++] = get_allocation_type(*stack_value);
    
    expected[index] = ALLOCATION_TYPE_LOCAL;
    actual[index++] = get_allocation_type(&stack_value);
    
    expected[index] = ALLOCATION_TYPE_UNKNOWN;
    actual[index++] = get_allocation_type(stack_value3);
    
    expected[index] = ALLOCATION_TYPE_LOCAL;
    actual[index++] = get_allocation_type(&stack_value3);
    
    expected[index] = ALLOCATION_TYPE_NULLPTR;
    actual[index++] = get_allocation_type(simple_value_init);
    
    expected[index] = ALLOCATION_TYPE_LOCAL;
    actual[index++] = get_allocation_type(&simple_value_init);
    
    expected[index] = ALLOCATION_TYPE_UNKNOWN;
    actual[index++] = get_allocation_type(simple_value);
    
    expected[index] = ALLOCATION_TYPE_LOCAL;
    actual[index++] = get_allocation_type(&simple_value);
    
    expected[index] = ALLOCATION_TYPE_NULLPTR;
    actual[index++] = get_allocation_type(static_value_init);
    
    expected[index] = ALLOCATION_TYPE_GLOBAL;
    actual[index++] = get_allocation_type(&static_value_init);
    
    expected[index] = ALLOCATION_TYPE_NULLPTR;
    actual[index++] = get_allocation_type(static_value);
    
    expected[index] = ALLOCATION_TYPE_GLOBAL;
    actual[index++] = get_allocation_type(&static_value);

    int result = is_arrays_equal(expected, actual, index);
    free(heap_value);

    return ORDER_RESULT(result, 0);
}

static int is_arrays_equal(allocation_type_t a[], allocation_type_t b[], int size) {
    for (int i = 0; i < size; i++)
        if (b[i] & a[i] == 0 && b[i] != ALLOCATION_TYPE_UNKNOWN)
            return 0;
    return 1;
}