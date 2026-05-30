/*********************************************************************************************
 * @file bitset.h
 * @author chcp (cmewhou@yandex.ru)
 * @brief Bitset collection.
 * @version 1.0
 * @date 2025-04-08
 *
 * @copyright Copyright (c) 2024
 ********************************************************************************************/

#ifndef IPEE_BITSET_H
#define IPEE_BITSET_H

#include <stddef.h>
#include <stdint.h>

/*********************************************************************************************
 * ERROR CODES
 ********************************************************************************************/

typedef enum ipee_bitset_error_code_e {
    IPEE_ERROR_CODE__BITSET__NOT_EXISTS         = -1, // Bitset does not exist.
    IPEE_ERROR_CODE__BITSET__INDEX_OUT_OF_RANGE = -2, // Index is out of valid range.
} ipee_bitset_error_code_t, *p_bitset_error_code;

/*********************************************************************************************
 * STRUCTS DECLARATIONS
 ********************************************************************************************/

/**
 * @brief Bitset collection.
 */
typedef struct bitset_s {
    uint8_t *buffer; // Internal buffer.
    size_t capacity; // Predefined buffer size in bits.
} bitset_t, *p_bitset;

/*********************************************************************************************
 * FUNCTIONS DECLARATIONS
 ********************************************************************************************/

/**
 * @brief Initialize bitset collection.
 *
 * @details
 * Create new bitset collection.
 *
 * @param capacity Bitset capacity in bits.
 */
extern p_bitset init_bitset(const size_t capacity);

/**
 * @brief Release container by reference.
 *
 * @param bitset Bitset collection.
 */
extern void release_bitset(p_bitset bitset);

/**
 * @brief Get bit in bitset by index.
 *
 * @param bitset Bitset collection.
 * @param index Bit index.
 * @return 0 or 1 on success, or a negative error code.
 */
extern int8_t get_bit(p_bitset bitset, int64_t index);

/**
 * @brief Set bit in bitset by index.
 *
 * @param bitset Bitset collection.
 * @param index Bit index.
 * @return 0 on success, or a negative error code.
 */
extern int set_bit(p_bitset bitset, int64_t index);

/**
 * @brief Reset bit in bitset by index.
 *
 * @param bitset Bitset collection.
 * @param index Bit index.
 * @return 0 on success, or a negative error code.
 */
extern int reset_bit(p_bitset bitset, int64_t index);

/**
 * @brief Invert bit in bitset by index.
 *
 * @param bitset Bitset collection.
 * @param index Bit index.
 * @return 0 on success, or a negative error code.
 */
extern int invert_bit(p_bitset bitset, int64_t index);

/**
 * @brief Reset bit range in bitset.
 *
 * @param bitset Bitset collection.
 * @param start Begin range index (inclusive).
 * @param end End range index (inclusive).
 * @return 0 on success, or a negative error code.
 */
extern int clear_bitset(p_bitset bitset, int64_t start, int64_t end);

/**
 * @brief Set bit range in bitset.
 *
 * @param bitset Bitset collection.
 * @param start Begin range index (inclusive).
 * @param end End range index (inclusive).
 * @return 0 on success, or a negative error code.
 */
extern int fill_bitset(p_bitset bitset, int64_t start, int64_t end);

/**
 * @brief Invert bit range in bitset.
 *
 * @param bitset Bitset collection.
 * @param start Begin range index (inclusive).
 * @param end End range index (inclusive).
 * @return 0 on success, or a negative error code.
 */
extern int invert_bitset(p_bitset bitset, int64_t start, int64_t end);

/**
 * @brief Return first free bit index.
 *
 * @param bitset Bitset collection.
 * @return Bit index, or -1 if all bits are set.
 */
extern int64_t get_first_free_bit(p_bitset bitset);

/**
 * @brief Return last free bit index.
 *
 * @param bitset Bitset collection.
 * @return Bit index, or -1 if all bits are set.
 */
extern int64_t get_last_free_bit(p_bitset bitset);

/**
 * @brief Return first busy bit index.
 *
 * @param bitset Bitset collection.
 * @return Bit index, or -1 if no bits are set.
 */
extern int64_t get_first_busy_bit(p_bitset bitset);

/**
 * @brief Return last busy bit index.
 *
 * @param bitset Bitset collection.
 * @return Bit index, or -1 if no bits are set.
 */
extern int64_t get_last_busy_bit(p_bitset bitset);

#endif // IPEE_BITSET_H
