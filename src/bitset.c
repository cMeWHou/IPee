#include <bitset.h>

#include <stdlib.h>
#include <string.h>

#include <macro.h>

#define BITSET_BITS_PER_CHAR 8
#define BITSET_BITS_PER_CHAR_WITH_OFFSET BITSET_BITS_PER_CHAR - 1

/*******************************************************************************
 * STRUCTS DECLARATIONS
 ******************************************************************************/

/**
 * @brief Bitset collection.
 */
typedef struct __impl_bitset_s {
    int            capacity;
    unsigned char *buffer;
} __impl_bitset_t, *__p_impl_bitset;

/*******************************************************************************
 * FUNCTIONS DEFINITIONS
 ******************************************************************************/

p_bitset create_bitset(const int capacity) {
    __p_impl_bitset impl_bitset = (__p_impl_bitset)malloc(sizeof(__impl_bitset_t));
    if (!impl_bitset)
        return NULL;

    const int num_bytes = capacity / BITSET_BITS_PER_CHAR
        + (capacity % BITSET_BITS_PER_CHAR == 0 ? 0 : 1);
    impl_bitset->capacity = capacity;
    impl_bitset->buffer = (char *)malloc(num_bytes * sizeof(char));
    memset(impl_bitset->buffer, 0, num_bytes);

    return (p_bitset)impl_bitset;
}

void delete_bitset(p_bitset bitset) {
    __p_impl_bitset impl_bitset = (__p_impl_bitset)bitset;
    if (!impl_bitset)
        return;

    if (impl_bitset->buffer)
        free(impl_bitset->buffer);

    free(impl_bitset);
}

char get_bit(p_bitset bitset, int index) {
    __p_impl_bitset impl_bitset = (__p_impl_bitset)bitset;
    if (!impl_bitset)
        exit(IPEE_ERROR_CODE__BITSET__NOT_EXISTS);
    if (index < 0 || index >= impl_bitset->capacity)
        exit(IPEE_ERROR_CODE__BITSET__INDEX_OUT_OF_RANGE);

    int byte_index = index / BITSET_BITS_PER_CHAR;
    int bit_index = index % BITSET_BITS_PER_CHAR;

    return (impl_bitset->buffer[byte_index] >>
        (BITSET_BITS_PER_CHAR_WITH_OFFSET - bit_index)) & 1;
}

void set_bit(p_bitset bitset, int index) {
    __p_impl_bitset impl_bitset = (__p_impl_bitset)bitset;
    if (!impl_bitset)
        exit(IPEE_ERROR_CODE__BITSET__NOT_EXISTS);
    if (index < 0 || index >= impl_bitset->capacity)
        exit(IPEE_ERROR_CODE__BITSET__INDEX_OUT_OF_RANGE);

    int byte_index = index / BITSET_BITS_PER_CHAR;
    int bit_index = index % BITSET_BITS_PER_CHAR;

    impl_bitset->buffer[byte_index] |= 
        (1 << (BITSET_BITS_PER_CHAR_WITH_OFFSET - bit_index));
}

void reset_bit(p_bitset bitset, int index) {
    __p_impl_bitset impl_bitset = (__p_impl_bitset)bitset;
    if (!impl_bitset)
        exit(IPEE_ERROR_CODE__BITSET__NOT_EXISTS);
    if (index < 0 || index >= impl_bitset->capacity)
        exit(IPEE_ERROR_CODE__BITSET__INDEX_OUT_OF_RANGE);

    int byte_index = index / BITSET_BITS_PER_CHAR;
    int bit_index = index % BITSET_BITS_PER_CHAR;

    impl_bitset->buffer[byte_index] &=
        ~(1 << (BITSET_BITS_PER_CHAR_WITH_OFFSET - bit_index));
}

void invert_bit(p_bitset bitset, int index) {
    __p_impl_bitset impl_bitset = (__p_impl_bitset)bitset;
    if (!impl_bitset)
        exit(IPEE_ERROR_CODE__BITSET__NOT_EXISTS);
    if (index < 0 || index >= impl_bitset->capacity)
        exit(IPEE_ERROR_CODE__BITSET__INDEX_OUT_OF_RANGE);

    int byte_index = index / BITSET_BITS_PER_CHAR;
    int bit_index = index % BITSET_BITS_PER_CHAR;

    impl_bitset->buffer[byte_index] ^=
        (1 << (BITSET_BITS_PER_CHAR_WITH_OFFSET - bit_index));
}

void clear_bitset(p_bitset bitset, int start, int end) {
    if (!bitset)
        exit(IPEE_ERROR_CODE__BITSET__NOT_EXISTS);
    if (start < 0 || end < 0 || end < start || end >= bitset->capacity)
        exit(IPEE_ERROR_CODE__BITSET__INDEX_OUT_OF_RANGE);

    for (int i = start; i <= end; i++)
        reset_bit(bitset, i);
}

void fill_bitset(p_bitset bitset, int start, int end) {
    if (!bitset)
        exit(IPEE_ERROR_CODE__BITSET__NOT_EXISTS);
    if (start < 0 || end < 0 || end < start || end >= bitset->capacity)
        exit(IPEE_ERROR_CODE__BITSET__INDEX_OUT_OF_RANGE);

    for (int i = start; i <= end; i++)
        set_bit(bitset, i);
}

void invert_bitset(p_bitset bitset, int start, int end) {
    if (!bitset)
        exit(IPEE_ERROR_CODE__BITSET__NOT_EXISTS);
    if (start < 0 || end < 0 || end < start || end >= bitset->capacity)
        exit(IPEE_ERROR_CODE__BITSET__INDEX_OUT_OF_RANGE);

    for (int i = start; i <= end; i++)
        invert_bit(bitset, i);
}

int get_first_free_bit(p_bitset bitset) {
    if (!bitset)
        exit(IPEE_ERROR_CODE__BITSET__NOT_EXISTS);

    for (int i = 0; i < bitset->capacity; i++)
        if (get_bit(bitset, i) == 0)
            return i;

    return -1;
}

int get_last_free_bit(p_bitset bitset) {
    if (!bitset)
        exit(IPEE_ERROR_CODE__BITSET__NOT_EXISTS);

    for (int i = bitset->capacity - 1; i >= 0; i--)
        if (get_bit(bitset, i) == 0)
            return i;

    return -1;
}
int get_first_busy_bit(p_bitset bitset) {
    if (!bitset)
        exit(IPEE_ERROR_CODE__BITSET__NOT_EXISTS);

    for (int i = 0; i < bitset->capacity; i++)
        if (get_bit(bitset, i) == 1)
            return i;

    return -1;
}

int get_last_busy_bit(p_bitset bitset) {
    if (!bitset)
        exit(IPEE_ERROR_CODE__BITSET__NOT_EXISTS);

    for (int i = bitset->capacity - 1; i >= 0; i--)
        if (get_bit(bitset, i) == 1)
            return i;

    return -1;
}
