/**
 * @file utils.h
 * @author chcp (cmewhou@yandex.ru)
 * @brief Utitlity functions.
 * @version 1.0
 * @date 2024-11-29
 *
 * @copyright Copyright (c) 2024
 */

#ifndef IPEE_UTILS_H
#define IPEE_UTILS_H

/*********************************************************************************************
 * STRUCTS DECLARATIONS
 ********************************************************************************************/

/**
 * @brief Allocation type.
 * 
 * @details
 * ALLOCATION_TYPE_HEAP - variable on heap.
 * ALLOCATION_TYPE_LOCAL - variable on stack.
 * ALLOCATION_TYPE_GLOBAL - global variable.
 * ALLOCATION_TYPE_NUMBER - number (x < 10000).
 * ALLOCATION_TYPE_CHARACTER - character (number 32 <= x <= 127).
 * ALLOCATION_TYPE_NULLPTR - null pointer (number 0).
 * ALLOCATION_TYPE_UNKNOWN - unknown value.
 */
typedef enum allocation_type_e {
    ALLOCATION_TYPE_HEAP = 1,
    ALLOCATION_TYPE_LOCAL = 2,
    ALLOCATION_TYPE_GLOBAL = 4,
    ALLOCATION_TYPE_NUMBER = 8,
    ALLOCATION_TYPE_CHARACTER = 16,
    ALLOCATION_TYPE_NULLPTR = 32,
    ALLOCATION_TYPE_UNKNOWN = 64,
} allocation_type_t, *p_allocation_type;

/*********************************************************************************************
 * FUNCTIONS DECLARATIONS
 ********************************************************************************************/

/**
 * @brief Return allocation type.
 * @details You should call init_memory_config() in the beginning of your program.
 */
extern void init_memory_config();

/**
 * @brief Return allocation type.
 * 
 * @param address Specified address (it also can be a number).
 * @return Allocation type.
 */
extern int get_allocation_type(unsigned long long address);

#endif // IPEE_UTILS_H