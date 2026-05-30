#include <bitset.h>

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>

#include <macro.h>

#define BITSET_BITS_PER_CHAR 8
#define BITSET_BITS_PER_CHAR_WITH_OFFSET (BITSET_BITS_PER_CHAR - 1)

/*********************************************************************************************
 * FUNCTIONS DEFINITIONS
 ********************************************************************************************/

p_bitset init_bitset(const size_t capacity) {
    p_bitset bitset = (p_bitset)malloc(sizeof(bitset_t));
    if (!bitset) {
        return NULL;
    }

    const size_t num_bytes = capacity / BITSET_BITS_PER_CHAR +
                         (capacity % BITSET_BITS_PER_CHAR == 0 ? 0 : 1);
    bitset->capacity = capacity;
    bitset->buffer = (uint8_t *)malloc(num_bytes * sizeof(uint8_t));
    if (!bitset->buffer) {
        free(bitset);
        return NULL;
    }
    memset(bitset->buffer, 0, num_bytes);

    return bitset;
}

void release_bitset(p_bitset bitset) {
    if (!bitset) {
        return;
    }

    if (bitset->buffer) {
        free(bitset->buffer);
    }

    free(bitset);
}

int8_t get_bit(p_bitset bitset, int64_t index) {
    if (!bitset) {
        return IPEE_ERROR_CODE__BITSET__NOT_EXISTS;
    }
    if (index < 0 || (size_t)index >= bitset->capacity) {
        return IPEE_ERROR_CODE__BITSET__INDEX_OUT_OF_RANGE;
    }

    size_t byte_index = (size_t)index / BITSET_BITS_PER_CHAR;
    size_t bit_index = (size_t)index % BITSET_BITS_PER_CHAR;

    return (bitset->buffer[byte_index] >> (BITSET_BITS_PER_CHAR_WITH_OFFSET - bit_index)) & 1u;
}

int set_bit(p_bitset bitset, int64_t index) {
    if (!bitset) {
        return IPEE_ERROR_CODE__BITSET__NOT_EXISTS;
    }
    if (index < 0 || (size_t)index >= bitset->capacity) {
        return IPEE_ERROR_CODE__BITSET__INDEX_OUT_OF_RANGE;
    }

    size_t byte_index = (size_t)index / BITSET_BITS_PER_CHAR;
    size_t bit_index = (size_t)index % BITSET_BITS_PER_CHAR;

    bitset->buffer[byte_index] |= (uint8_t)(1u << (BITSET_BITS_PER_CHAR_WITH_OFFSET - bit_index));
    return 0;
}

int reset_bit(p_bitset bitset, int64_t index) {
    if (!bitset) {
        return IPEE_ERROR_CODE__BITSET__NOT_EXISTS;
    }
    if (index < 0 || (size_t)index >= bitset->capacity) {
        return IPEE_ERROR_CODE__BITSET__INDEX_OUT_OF_RANGE;
    }

    size_t byte_index = (size_t)index / BITSET_BITS_PER_CHAR;
    size_t bit_index = (size_t)index % BITSET_BITS_PER_CHAR;

    bitset->buffer[byte_index] &= (uint8_t)~(1u << (BITSET_BITS_PER_CHAR_WITH_OFFSET - bit_index));
    return 0;
}

int invert_bit(p_bitset bitset, int64_t index) {
    if (!bitset) {
        return IPEE_ERROR_CODE__BITSET__NOT_EXISTS;
    }
    if (index < 0 || (size_t)index >= bitset->capacity) {
        return IPEE_ERROR_CODE__BITSET__INDEX_OUT_OF_RANGE;
    }

    size_t byte_index = (size_t)index / BITSET_BITS_PER_CHAR;
    size_t bit_index = (size_t)index % BITSET_BITS_PER_CHAR;

    bitset->buffer[byte_index] ^= (uint8_t)(1u << (BITSET_BITS_PER_CHAR_WITH_OFFSET - bit_index));
    return 0;
}

int clear_bitset(p_bitset bitset, int64_t start, int64_t end) {
    if (!bitset) {
        return IPEE_ERROR_CODE__BITSET__NOT_EXISTS;
    }
    if (start < 0 || end < 0 || end < start || (size_t)end >= bitset->capacity) {
        return IPEE_ERROR_CODE__BITSET__INDEX_OUT_OF_RANGE;
    }

    for (int64_t i = start; i <= end; i++) {
        reset_bit(bitset, i);
    }
    return 0;
}

int fill_bitset(p_bitset bitset, int64_t start, int64_t end) {
    if (!bitset) {
        return IPEE_ERROR_CODE__BITSET__NOT_EXISTS;
    }
    if (start < 0 || end < 0 || end < start || (size_t)end >= bitset->capacity) {
        return IPEE_ERROR_CODE__BITSET__INDEX_OUT_OF_RANGE;
    }

    for (int64_t i = start; i <= end; i++) {
        set_bit(bitset, i);
    }
    return 0;
}

int invert_bitset(p_bitset bitset, int64_t start, int64_t end) {
    if (!bitset) {
        return IPEE_ERROR_CODE__BITSET__NOT_EXISTS;
    }
    if (start < 0 || end < 0 || end < start || (size_t)end >= bitset->capacity) {
        return IPEE_ERROR_CODE__BITSET__INDEX_OUT_OF_RANGE;
    }

    for (int64_t i = start; i <= end; i++) {
        invert_bit(bitset, i);
    }
    return 0;
}

int64_t get_first_free_bit(p_bitset bitset) {
    if (!bitset) {
        return IPEE_ERROR_CODE__BITSET__NOT_EXISTS;
    }

    for (int64_t i = 0; i < (int64_t)bitset->capacity; i++) {
        if (get_bit(bitset, i) == 0) {
            return i;
        }
    }

    return -1;
}

int64_t get_last_free_bit(p_bitset bitset) {
    if (!bitset) {
        return IPEE_ERROR_CODE__BITSET__NOT_EXISTS;
    }

    for (int64_t i = (int64_t)bitset->capacity - 1; i >= 0; i--) {
        if (get_bit(bitset, i) == 0) {
            return i;
        }
    }

    return -1;
}

int64_t get_first_busy_bit(p_bitset bitset) {
    if (!bitset) {
        return IPEE_ERROR_CODE__BITSET__NOT_EXISTS;
    }

    for (int64_t i = 0; i < (int64_t)bitset->capacity; i++) {
        if (get_bit(bitset, i) == 1) {
            return i;
        }
    }

    return -1;
}

int64_t get_last_busy_bit(p_bitset bitset) {
    if (!bitset) {
        return IPEE_ERROR_CODE__BITSET__NOT_EXISTS;
    }

    for (int64_t i = (int64_t)bitset->capacity - 1; i >= 0; i--) {
        if (get_bit(bitset, i) == 1) {
            return i;
        }
    }

    return -1;
}
