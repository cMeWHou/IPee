/*********************************************************************************************
 * @file hashmap.h
 * @author Bulat Ramazanov (ramzesbulman@gmail.com)
 * @brief Implemented hashmap collection
 * @version 1.0
 * @date 2024-11-21
 * 
 * @copyright Copyright (c) 2024
 ********************************************************************************************/

#ifndef IPEE_HASHMAP_H
#define IPEE_HASHMAP_H

/*********************************************************************************************
 * INCLUDES DECLARATIONS
 ********************************************************************************************/

#include <stdint.h>

/*********************************************************************************************
 * MACROS DECLARATIONS
 ********************************************************************************************/

#define HASHMAP_STR_KEY(str) (&str), sizeof(str) - 1
#define HASHMAP_STATIC_KEY(entry) (&entry), sizeof(entry)
#define HASHMAP_DYNAMIC_KEY(entry) (&entry), sizeof(*entry)

/*********************************************************************************************
 * STRUCTS DECLARATIONS
 ********************************************************************************************/

typedef struct hashmap_s hashmap_t, *p_hashmap;

typedef const void * p_key;

/***********************************************************************************************
 * FUNCTION TYPEDEFS
 **********************************************************************************************/

/**
 * @brief Callback function to allocate memory for dynamic entries of hashmap collection.
 * 
 * @param value     Value object reference in bucket entry.
 * 
 * @return Pointer to allocated address in heap.
 */
typedef void (*hashmap_malloc_entry_callback)(void **value);

/**
 * @brief Callback function to free memory for dynamic entries of hashmap collection.
 * 
 * @param value     Value object reference in bucket entry.
 */
typedef void (*hashmap_free_entry_callback)(void **value);

/**
 * @brief Callback function for processing keys and values of hashmap collection.
 * 
 * @param key       Pointer to key for bucket entry.
 * @param value     Pointer to value in bucket entry.
 */
typedef void (*hashmap_iteration_callback)(p_key key, void *value);

/*********************************************************************************************
 * FUNCTIONS DECLARATIONS
 ********************************************************************************************/

/**
 * @brief Create hashmap with callback functions to handle allocating and freeing memory for entries.
 * 
 * @details
 * Use this function only for dynamic entries.
 * 
 * @param malloc    Pointer to callback function of memory allocation in heap.
 * @param free      Pointer to callback function of memory freeing in heap.
 * 
 * @return Pointer to hashmap.
 */
extern p_hashmap hashmap_create_for_dynamic_entry(hashmap_malloc_entry_callback entry_malloc,
                                                  hashmap_free_entry_callback entry_free);

/**
 * @brief Create hashmap.
 * 
 * @return Pointer to hashmap.
 */
extern p_hashmap hashmap_create(void);

/**
 * @brief Set entry in hashmap.
 * 
 * @param map       Pointer to hashmap.
 * @param key       Pointer to key for bucket entry.
 * @param ksize     Key size for bucket entry.
 * @param value     Value in bucket entry.
 */
void hashmap_set_entry(p_hashmap map, p_key *key, size_t ksize, void *value);

/**
 * @brief Get entry in hashmap.
 * 
 * @param map       Pointer to hashmap.
 * @param key       Pointer to key for bucket entry.
 * @param ksize     Key size for bucket entry.
 * 
 * @return Pointer to bucket value.
 */
void *hashmap_get_entry(p_hashmap map, p_key key, size_t ksize);

/**
 * @brief Remove entry in hashmap.
 * 
 * @param map       Pointer to hashmap.
 * @param key       Pointer to key for bucket entry.
 * @param ksize     Key size for bucket entry.
 */
void hashmap_remove_entry(p_hashmap map, p_key key, size_t ksize);

/**
 * @brief Remove all entries in hashmap.
 * 
 * @param map       Pointer to hashmap.
 */
void hashmap_remove_all_entries(p_hashmap map);

/**
 * @brief Remove hashmap.
 * 
 * @param map       Hashmap object reference.
 */
extern void hashmap_remove(p_hashmap *map);

/**
 * @brief Get number of items in hashmap.
 * 
 * @param map       Pointer to hashmap.
 * 
 * @return Number of items
 */
int hashmap_get_size(p_hashmap map);

/**
 * @brief Iterate over hashmap.
 * 
 * @param map       Pointer to hashmap.
 * @param callback  Callback function.
 */
extern void hashmap_iterate(p_hashmap map, hashmap_iteration_callback callback);

#endif // IPEE_HASHMAP_H
