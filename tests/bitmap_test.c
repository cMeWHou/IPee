/**
* @file bitmap.test.c
* @author chcp (cmewhou@yandex.ru)
* @brief Bitmap tests
* @version 1.0
* @date 2025-04-08
*
* @copyright Copyright (c) 2024
*/

#include "utils/helper.h"

#include <bitmap.h>

#include <string.h>

/*********************************************************************************************
* FUNCTIONS DECLARATIONS
********************************************************************************************/

/** @brief Bitmap collection test. */
int bitmap_setSpecialBits_OK(void);

/** @brief Bitmap collection test. */
int bitmap_clearFillBitmap_OK(void);

/** @brief Bitmap collection test. */
int bitmap_getFirstOrLastBit_OK(void);

/** @brief Bitmap collection test. */
int bitmap_invertBit_OK(void);

/*********************************************************************************************
* FUNCTIONS DEFINITIONS
********************************************************************************************/

int bitmap_test(int argc, char *argv[]) {
    int exit_result = 0;
    
    exit_result |= bitmap_setSpecialBits_OK();
    exit_result |= bitmap_clearFillBitmap_OK();
    exit_result |= bitmap_getFirstOrLastBit_OK();
    exit_result |= bitmap_invertBit_OK();
    
    return exit_result;
}

int bitmap_setSpecialBits_OK(void) {
    const int size = 10;
    p_bitmap bitmap = init_bitmap(size);
    
    set_bit(bitmap, 2);
    set_bit(bitmap, bitmap->capacity - 1);
    
    const char expected[] = {[0] = 0b00100000, [1] = 0b01000000};
    
    int result = 0;
    if (memcmp(expected, bitmap->buffer, 2) == 0)
    result = 1;
    
    return ORDER_RESULT(result, 0);
}

int bitmap_getFirstOrLastBit_OK(void) {
    const int size = 10;
    p_bitmap bitmap = init_bitmap(size);
    
    int result = 1;
    fill_bitmap(bitmap, 0, bitmap->capacity - 1);
    if (bitmap->buffer[0] != (unsigned char)0xFF || bitmap->buffer[1] != (unsigned char)0xC0)
    result = 0;
    
    clear_bitmap(bitmap, 0, bitmap->capacity - 1);
    if (bitmap->buffer[0] != 0x0 || bitmap->buffer[1] != 0x0)
    result = 0;
    
    return ORDER_RESULT(result, 1);
}

int bitmap_clearFillBitmap_OK(void) {
    const int size = 10;
    p_bitmap bitmap = init_bitmap(size);
    
    fill_bitmap(bitmap, 0, bitmap->capacity - 1);
    reset_bit(bitmap, 6);
    reset_bit(bitmap, 9);
    
    int firstFreeBit = get_first_free_bit(bitmap);
    int lastFreeBit = get_last_free_bit(bitmap);
    
    clear_bitmap(bitmap, 0, bitmap->capacity - 1);
    set_bit(bitmap, 6);
    set_bit(bitmap, 9);
    int firstBusyBit = get_first_busy_bit(bitmap);
    int lastBusyBit = get_last_busy_bit(bitmap);
    
    int result = firstFreeBit == 6 && lastFreeBit == 9 && firstBusyBit == 6 && lastBusyBit == 9;
    return ORDER_RESULT(result, 2);
}

int bitmap_invertBit_OK(void) {
    const int size = 10;
    p_bitmap bitmap = init_bitmap(size);
    
    fill_bitmap(bitmap, 0, bitmap->capacity - 1);
    reset_bit(bitmap, 6);
    reset_bit(bitmap, 9);
    invert_bitmap(bitmap, 6, 9);
    
    int firstFreeBit = get_first_free_bit(bitmap);
    int lastFreeBit = get_last_free_bit(bitmap);
    
    int result = firstFreeBit == 7 && lastFreeBit == 8;
    return ORDER_RESULT(result, 3);
}