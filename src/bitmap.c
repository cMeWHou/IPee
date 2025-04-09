#include <bitmap.h>

#include <stdlib.h>
#include <string.h>

#include <macro.h>

#define BITMAP_BITS_PER_CHAR 8
#define BITMAP_BITS_PER_CHAR_WITH_OFFSET BITMAP_BITS_PER_CHAR - 1

/*********************************************************************************************
 * FUNCTIONS DEFINITIONS
 ********************************************************************************************/

p_bitmap init_bitmap(const int capacity) {
    p_bitmap bitmap = (p_bitmap)malloc(sizeof(bitmap_t));
    if (!bitmap)
        return NULL;

    const int num_bytes = capacity / BITMAP_BITS_PER_CHAR + (capacity % BITMAP_BITS_PER_CHAR == 0 ? 0 : 1);
    bitmap->capacity = capacity;
    bitmap->buffer = (char *)malloc(num_bytes * sizeof(char));
    memset(bitmap->buffer, 0, num_bytes);

    return bitmap;
}

void release_bitmap(p_bitmap bitmap) {
    if (!bitmap)
        return;

    if (bitmap->buffer)
        free(bitmap->buffer);

    free(bitmap);
}

char get_bit(p_bitmap bitmap, int index) {
    if (!bitmap)
        exit(IPEE_ERROR_CODE__BITMAP__NOT_EXISTS);
    if (index < 0 || index >= bitmap->capacity)
        exit(IPEE_ERROR_CODE__BITMAP__INDEX_OUT_OF_RANGE);

    int byte_index = index / BITMAP_BITS_PER_CHAR;
    int bit_index = index % BITMAP_BITS_PER_CHAR;

    return (bitmap->buffer[byte_index] >> (BITMAP_BITS_PER_CHAR_WITH_OFFSET - bit_index)) & 1;
}

void set_bit(p_bitmap bitmap, int index) {
    if (!bitmap)
        exit(IPEE_ERROR_CODE__BITMAP__NOT_EXISTS);
    if (index < 0 || index >= bitmap->capacity)
        exit(IPEE_ERROR_CODE__BITMAP__INDEX_OUT_OF_RANGE);

    int byte_index = index / BITMAP_BITS_PER_CHAR;
    int bit_index = index % BITMAP_BITS_PER_CHAR;

    bitmap->buffer[byte_index] |= (1 << (BITMAP_BITS_PER_CHAR_WITH_OFFSET - bit_index));
}

void reset_bit(p_bitmap bitmap, int index) {
    if (!bitmap)
        exit(IPEE_ERROR_CODE__BITMAP__NOT_EXISTS);
    if (index < 0 || index >= bitmap->capacity)
        exit(IPEE_ERROR_CODE__BITMAP__INDEX_OUT_OF_RANGE);

    int byte_index = index / BITMAP_BITS_PER_CHAR;
    int bit_index = index % BITMAP_BITS_PER_CHAR;

    bitmap->buffer[byte_index] &= ~(1 << (BITMAP_BITS_PER_CHAR_WITH_OFFSET - bit_index));
}

void invert_bit(p_bitmap bitmap, int index) {
    if (!bitmap)
        exit(IPEE_ERROR_CODE__BITMAP__NOT_EXISTS);
    if (index < 0 || index >= bitmap->capacity)
        exit(IPEE_ERROR_CODE__BITMAP__INDEX_OUT_OF_RANGE);

    int byte_index = index / BITMAP_BITS_PER_CHAR;
    int bit_index = index % BITMAP_BITS_PER_CHAR;

    bitmap->buffer[byte_index] ^= (1 << (BITMAP_BITS_PER_CHAR_WITH_OFFSET - bit_index));
}

void clear_bitmap(p_bitmap bitmap, int start, int end) {
    if (!bitmap)
        exit(IPEE_ERROR_CODE__BITMAP__NOT_EXISTS);
    if (start < 0 || end < 0 || end < start || end >= bitmap->capacity)
        exit(IPEE_ERROR_CODE__BITMAP__INDEX_OUT_OF_RANGE);

    for (int i = start; i <= end; i++)
        reset_bit(bitmap, i);
}

void fill_bitmap(p_bitmap bitmap, int start, int end) {
    if (!bitmap)
        exit(IPEE_ERROR_CODE__BITMAP__NOT_EXISTS);
    if (start < 0 || end < 0 || end < start || end >= bitmap->capacity)
        exit(IPEE_ERROR_CODE__BITMAP__INDEX_OUT_OF_RANGE);

    for (int i = start; i <= end; i++)
        set_bit(bitmap, i);
}

void invert_bitmap(p_bitmap bitmap, int start, int end) {
    if (!bitmap)
        exit(IPEE_ERROR_CODE__BITMAP__NOT_EXISTS);
    if (start < 0 || end < 0 || end < start || end >= bitmap->capacity)
        exit(IPEE_ERROR_CODE__BITMAP__INDEX_OUT_OF_RANGE);

    for (int i = start; i <= end; i++)
        invert_bit(bitmap, i);
}

int get_first_free_bit(p_bitmap bitmap) {
    if (!bitmap)
        exit(IPEE_ERROR_CODE__BITMAP__NOT_EXISTS);

    for (int i = 0; i < bitmap->capacity; i++)
        if (get_bit(bitmap, i) == 0)
            return i;

    return -1;
}

int get_last_free_bit(p_bitmap bitmap) {
    if (!bitmap)
        exit(IPEE_ERROR_CODE__BITMAP__NOT_EXISTS);

    for (int i = bitmap->capacity - 1; i >= 0; i--)
        if (get_bit(bitmap, i) == 0)
            return i;

    return -1;
}
int get_first_busy_bit(p_bitmap bitmap) {
    if (!bitmap)
        exit(IPEE_ERROR_CODE__BITMAP__NOT_EXISTS);

    for (int i = 0; i < bitmap->capacity; i++)
        if (get_bit(bitmap, i) == 1)
            return i;

    return -1;
}

int get_last_busy_bit(p_bitmap bitmap) {
    if (!bitmap)
        exit(IPEE_ERROR_CODE__BITMAP__NOT_EXISTS);

    for (int i = bitmap->capacity - 1; i >= 0; i--)
        if (get_bit(bitmap, i) == 1)
            return i;

    return -1;
}
