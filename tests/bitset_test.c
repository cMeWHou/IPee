/**
 * @file bitset_test.c
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

/** @brief Set bits. */
int bitset_setSpecialBits_OK(void);

/** @brief Fill and clear bit range. */
int bitset_clearFillBitset_OK(void);

/** @brief Get first/last free and busy bit index. */
int bitset_getFirstOrLastBit_OK(void);

/** @brief Invert bits. */
int bitset_invertBit_OK(void);

/*********************************************************************************************
 * FUNCTIONS DEFINITIONS
 ********************************************************************************************/

int bitset_test(int argc, char *argv[]) {
    int exit_result = 0;

    exit_result |= bitset_setSpecialBits_OK();
    exit_result |= bitset_clearFillBitset_OK();
    exit_result |= bitset_getFirstOrLastBit_OK();
    exit_result |= bitset_invertBit_OK();

    return exit_result;
}

int bitset_setSpecialBits_OK(void) {
    const int size = 10;
    p_bitset bitset = init_bitset(size);

    set_bit(bitset, 2);
    set_bit(bitset, bitset->capacity - 1);

    const char expected[] = {[0] = 0b00100000, [1] = 0b01000000};

    int result = 0;
    if (memcmp(expected, bitset->buffer, 2) == 0) {
        result = 1;
    }

    release_bitset(bitset);
    return ORDER_RESULT(result, 0);
}

int bitset_clearFillBitset_OK(void) {
    const int size = 10;
    p_bitset bitset = init_bitset(size);

    int result = 1;
    fill_bitset(bitset, 0, bitset->capacity - 1);
    if (bitset->buffer[0] != (unsigned char)0xFF ||
        bitset->buffer[1] != (unsigned char)0xC0) {
        result = 0;
    }

    clear_bitset(bitset, 0, bitset->capacity - 1);
    if (bitset->buffer[0] != 0x0 || bitset->buffer[1] != 0x0) {
        result = 0;
    }

    release_bitset(bitset);
    return ORDER_RESULT(result, 1);
}

int bitset_getFirstOrLastBit_OK(void) {
    const int size = 10;
    p_bitset bitset = init_bitset(size);

    fill_bitset(bitset, 0, bitset->capacity - 1);
    reset_bit(bitset, 6);
    reset_bit(bitset, 9);

    int64_t firstFreeBit = get_first_free_bit(bitset);
    int64_t lastFreeBit = get_last_free_bit(bitset);

    clear_bitset(bitset, 0, bitset->capacity - 1);
    set_bit(bitset, 6);
    set_bit(bitset, 9);
    int64_t firstBusyBit = get_first_busy_bit(bitset);
    int64_t lastBusyBit = get_last_busy_bit(bitset);

    release_bitset(bitset);
    int result = firstFreeBit == 6 && lastFreeBit == 9 && firstBusyBit == 6 &&
                 lastBusyBit == 9;
    return ORDER_RESULT(result, 2);
}

int bitset_invertBit_OK(void) {
    const int size = 10;
    p_bitset bitset = init_bitset(size);

    fill_bitset(bitset, 0, bitset->capacity - 1);
    reset_bit(bitset, 6);
    reset_bit(bitset, 9);
    invert_bitset(bitset, 6, 9);

    int64_t firstFreeBit = get_first_free_bit(bitset);
    int64_t lastFreeBit = get_last_free_bit(bitset);

    release_bitset(bitset);
    int result = firstFreeBit == 7 && lastFreeBit == 8;
    return ORDER_RESULT(result, 3);
}
