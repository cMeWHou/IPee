/*******************************************************************************
 * @file bitset.h
 * @author chcp (cmewhou@yandex.ru)
 * @brief Bitset collection.
 * @version 1.0
 * @date 2025-04-08
 *
 * @copyright Copyright (c) 2025
 ******************************************************************************/

#ifndef IPEE_BITSET_H
#define IPEE_BITSET_H

/*******************************************************************************
 * STRUCTS DECLARATIONS
 ******************************************************************************/

/**
 * @brief Bitset collection.
 */
typedef struct bitset_s {
    int capacity; // Collection capacity.
} bitset_t, *p_bitset;

/*******************************************************************************
 * FUNCTIONS DECLARATIONS
 ******************************************************************************/

/**
 * @brief Initialize bitset collection.
 *
 * @param capacity Bitset capacity.
 */
extern p_bitset create_bitset(const int capacity);

/**
 * @brief Release container by reference.
 *
 * @param bitset Bitset collection.
 */
extern void delete_bitset(p_bitset bitset);

/**
 * @brief Get bit in bitset by index.
 *
 * @param bitset Bitset collection.
 * @param index  Bit index.
 */
extern char get_bit(p_bitset bitset, int index);

/**
 * @brief Set bit in bitset by index.
 *
 * @param bitset Bitset collection.
 * @param index  Bit index.
 */
extern void set_bit(p_bitset bitset, int index);

/**
 * @brief Reset bit in bitset by index.
 *
 * @param bitset Bitset collection.
 * @param index  Bit index.
 */
extern void reset_bit(p_bitset bitset, int index);

/**
 * @brief Invert bit in bitset by index.
 *
 * @param bitset Bitset collection.
 * @param index  Bit index.
 */
extern void invert_bit(p_bitset bitset, int index);

/**
 * @brief Reset bit range in bitset.
 *
 * @param bitset Bitset collection.
 * @param start  Begin range index.
 * @param end    End range index.
 */
extern void clear_bitset(p_bitset bitset, int start, int end);

/**
 * @brief Set bit range in bitset.
 *
 * @param bitset Bitset collection.
 * @param start  Begin range index.
 * @param end    End range index.
 */
extern void fill_bitset(p_bitset bitset, int start, int end);

/**
 * @brief Invert bit range in bitset.
 *
 * @param bitset Bitset collection.
 * @param start  Begin range index.
 * @param end    End range index.
 */
extern void invert_bitset(p_bitset bitset, int start, int end);

/**
 * @brief Return first free bit index.
 *
 * @param bitset Bitset collection.
 * 
 * @return Free bit index.
 */
extern int get_first_free_bit(p_bitset bitset);

/**
 * @brief Return last free bit index.
 *
 * @param bitset Bitset collection.
 * 
 * @return Free bit index.
 */
extern int get_last_free_bit(p_bitset bitset);

/**
 * @brief Return first busy bit index.
 *
 * @param bitset Bitset collection.
 * 
 * @return Busy bit index.
 */
extern int get_first_busy_bit(p_bitset bitset);

/**
 * @brief Return last busy bit index.
 *
 * @param bitset Bitset collection.
 * 
 * @return Busy bit index.
 */
extern int get_last_busy_bit(p_bitset bitset);

#endif // IPEE_BITSET_H