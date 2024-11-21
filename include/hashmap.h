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



/*********************************************************************************************
 * STRUCTS DECLARATIONS
 ********************************************************************************************/

typedef struct hashmap_s hashmap_t, *p_hashmap;

typedef const void * p_key;

/***********************************************************************************************
 * FUNCTION TYPEDEFS
 **********************************************************************************************/

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
 * @param value     Value in bucket entry.
 */
extern void hashmap_set_entry(p_hashmap map, p_key key, void *value);

/**
 * @brief Get entry in hashmap.
 * 
 * @param map       Pointer to hashmap.
 * @param key       Pointer to key for bucket entry.
 * 
 * @return Pointer to bucket value.
 */
extern void *hashmap_get_entry(p_hashmap map, p_key key);

/**
 * @brief Remove entry in hashmap.
 * 
 * @param map       Pointer to hashmap.
 * @param key       Pointer to key for bucket entry.
 */
extern void hashmap_remove_entry(p_hashmap map, p_key key);

/**
 * @brief Remove all entries in hashmap.
 * 
 * @param map       Pointer to hashmap.
 */
extern void hashmap_remove_all_entries(p_hashmap map);

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
extern int hashmap_get_count(p_hashmap map);

/**
 * @brief Iterate over hashmap.
 * 
 * @param map       Pointer to hashmap.
 * @param callback  Callback function.
 */
extern void hashmap_iterate(p_hashmap map, hashmap_iteration_callback callback);

#endif // IPEE_HASHMAP_H
