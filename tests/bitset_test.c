/**
 * @file bitset.test.c
 * @author chcp (cmewhou@yandex.ru)
 * @brief Bitset tests
 * @version 1.0
 * @date 2025-04-08
 *
 * @copyright Copyright (c) 2024
 */

#include "utils/helper.h"

#include <bitset.h>

#include <string.h>

/*********************************************************************************************
 * FUNCTIONS DECLARATIONS
 ********************************************************************************************/

/** @brief Bitset collection test. */
int bitset_setSpecialBits_OK(void);

/** @brief Bitset collection test. */
int bitset_clearFillBitset_OK(void);

/** @brief Bitset collection test. */
int bitset_getFirstOrLastBit_OK(void);

/** @brief Bitset collection test. */
int bitset_invertBit_OK(void);

/*********************************************************************************************
 * FUNCTIONS DEFINITIONS
 ********************************************************************************************/

int bitset_test(int argc, char *argv[]) {
    int exit_result = 0;

    exit_result |= bitset_setSpecialBits_OK();
    exit_result |= bitset_getFirstOrLastBit_OK();
    exit_result |= bitset_clearFillBitset_OK();
    exit_result |= bitset_invertBit_OK();

    return exit_result;
}

int bitset_setSpecialBits_OK(void) {
    const int size = 10;
    p_bitset bitset = create_bitset(size);

    set_bit(bitset, 2);
    set_bit(bitset, bitset->capacity - 1);

    int result = get_bit(bitset, 2) == 1 && get_bit(bitset, 9) == 1;

    delete_bitset(bitset);
    return ORDER_RESULT(result, 0);
}

int bitset_getFirstOrLastBit_OK(void) {
    const int size = 10;
    p_bitset bitset = create_bitset(size);

    int result = 1;
    fill_bitset(bitset, 0, bitset->capacity - 1);
    if (get_bit(bitset, 0) != 1 || get_bit(bitset, 1) != 1)
        result = 0;

    clear_bitset(bitset, 0, bitset->capacity - 1);
    if (get_bit(bitset, 0) != 0 || get_bit(bitset, 1) != 0)
        result = 0;

    delete_bitset(bitset);
    return ORDER_RESULT(result, 1);
}

int bitset_clearFillBitset_OK(void) {
    const int size = 10;
    p_bitset bitset = create_bitset(size);

    fill_bitset(bitset, 0, bitset->capacity - 1);
    reset_bit(bitset, 6);
    reset_bit(bitset, 9);

    int firstFreeBit = get_first_free_bit(bitset);
    int lastFreeBit = get_last_free_bit(bitset);

    clear_bitset(bitset, 0, bitset->capacity - 1);
    set_bit(bitset, 6);
    set_bit(bitset, 9);
    int firstBusyBit = get_first_busy_bit(bitset);
    int lastBusyBit = get_last_busy_bit(bitset);

    delete_bitset(bitset);
    int result = firstFreeBit == 6 && lastFreeBit == 9 && firstBusyBit == 6 && lastBusyBit == 9;
    return ORDER_RESULT(result, 2);
}

int bitset_invertBit_OK(void) {
    const int size = 10;
    p_bitset bitset = create_bitset(size);

    fill_bitset(bitset, 0, bitset->capacity - 1);
    reset_bit(bitset, 6);
    reset_bit(bitset, 9);
    invert_bitset(bitset, 6, 9);

    int firstFreeBit = get_first_free_bit(bitset);
    int lastFreeBit = get_last_free_bit(bitset);

    delete_bitset(bitset);
    int result = firstFreeBit == 7 && lastFreeBit == 8;
    return ORDER_RESULT(result, 3);
}