/*********************************************************************************************
 * @file bitmap.h
 * @author chcp (cmewhou@yandex.ru)
 * @brief Bitmap collection.
 * @version 1.0
 * @date 2025-04-08
 *
 * @copyright Copyright (c) 2024
 ********************************************************************************************/

#ifndef IPEE_BITMAP_H
#define IPEE_BITMAP_H

/*********************************************************************************************
 * STRUCTS DECLARATIONS
 ********************************************************************************************/

/**
 * @brief Bitmap collection.
 */
typedef struct bitmap_s {
    unsigned char *buffer;
    int capacity;
} bitmap_t, *p_bitmap;

/*********************************************************************************************
 * FUNCTIONS DECLARATIONS
 ********************************************************************************************/

/**
 * @brief Initialize bitmap collection.
 *
 * @details
 * Create new bitmap collection.
 *
 * @param capacity Bitmap capacity.
 */
extern p_bitmap init_bitmap(const int capacity);

/**
 * @brief Release container by reference.
 *
 * @param bitmap Bitmap collection.
 */
extern void release_bitmap(p_bitmap bitmap);

/**
 * @brief Get bit in bitmap by index.
 *
 * @param bitmap Bitmap collection.
 * @param index Bit index.
 */
extern char get_bit(p_bitmap bitmap, int index);

/**
 * @brief Set bit in bitmap by index.
 *
 * @param bitmap Bitmap collection.
 * @param index Bit index.
 */
extern void set_bit(p_bitmap bitmap, int index);

/**
 * @brief Reset bit in bitmap by index.
 *
 * @param bitmap Bitmap collection.
 * @param index Bit index.
 */
extern void reset_bit(p_bitmap bitmap, int index);

/**
 * @brief Invert bit in bitmap by index.
 *
 * @param bitmap Bitmap collection.
 * @param index Bit index.
 */
extern void invert_bit(p_bitmap bitmap, int index);

/**
 * @brief Reset bit range in bitmap.
 *
 * @param bitmap Bitmap collection.
 * @param start Begin range index.
 * @param end End range index.
 */
extern void clear_bitmap(p_bitmap bitmap, int start, int end);

/**
 * @brief Set bit range in bitmap.
 *
 * @param bitmap Bitmap collection.
 * @param start Begin range index.
 * @param end End range index.
 */
extern void fill_bitmap(p_bitmap bitmap, int start, int end);

/**
 * @brief Invert bit range in bitmap.
 *
 * @param bitmap Bitmap collection.
 * @param start Begin range index.
 * @param end End range index.
 */
extern void invert_bitmap(p_bitmap bitmap, int start, int end);

/**
 * @brief Return first free bit index.
 *
 * @param bitmap Bitmap collection.
 */
extern int get_first_free_bit(p_bitmap bitmap);

/**
 * @brief Return last free bit index.
 *
 * @param bitmap Bitmap collection.
 */
extern int get_last_free_bit(p_bitmap bitmap);

/**
 * @brief Return first busy bit index.
 *
 * @param bitmap Bitmap collection.
 */
extern int get_first_busy_bit(p_bitmap bitmap);

/**
 * @brief Return last busy bit index.
 *
 * @param bitmap Bitmap collection.
 */
extern int get_last_busy_bit(p_bitmap bitmap);

#endif // IPEE_BITMAP_H