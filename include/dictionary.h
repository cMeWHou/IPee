/*********************************************************************************************
 * @file dictionary.h
 * @author chcp (cmewhou@yandex.ru)
 * @brief Common implementation of an <key:value> pair collection based on a linked list.
 * @version 1.3
 * @date 2024-11-18
 *
 * @copyright Copyright (c) 2024
 ********************************************************************************************/

#ifndef IPEE_DICTIONARY_H
#define IPEE_DICTIONARY_H

/*********************************************************************************************
 * STRUCTS DECLARATIONS
 ********************************************************************************************/

/**
 * @brief Record.
 *
 * @details
 * Record is a node of linked list.
 * Record is <key:value> pair. It contains a key, value and reference to next
 * node. key - string value or pointer-value. value - pointer to any type value.
 * next - reference to next node.
 * prev - reference to previous node.
 *
 * @warning
 * The implementation is mutable, not thread-safe and not reentrant.
 */
typedef struct record_s {
    struct record_s *next; // Next node reference.
    struct record_s *prev; // Previous node reference.
    char *key;             // String key.
    void *value;           // Any type value.
    void *metadata;        // Metadata.
} record_t, *p_record;

/**
 * @brief Dictionary collection.
 *
 * @details
 * Collection is a linked list of records. Records are not unique.
 * It contains a pointer to actual head record and current size of collection.
 * head - pointer to the head record.
 * tail - pointer to the tail record.
 * size - count of records in collection.
 *
 * @warning
 * The implementation is mutable, not thread-safe and not reentrant.
 */
typedef struct dictionary_s {
    p_record head;  // Head of the dictionary.
    p_record tail;  // Tail of the dictionary.
    int size;       // Size of the dictionary.
    void *metadata; // Metadata.
} dictionary_t, *p_dictionary;

/***********************************************************************************************
 * FUNCTION TYPEDEFS
 **********************************************************************************************/

/**
 * @brief Callback function for processing keys and values of collection
 * records.
 *
 * @param key Record key.
 * @param value Record value.
 */
typedef void (*dictionary_iteration_callback)(char *key, void *value);

/**
 * @brief Callback function for processing keys and values of collection records
 * with arguments.
 *
 * @param key Record key.
 * @param value Record value.
 * @param args Arguments for callback function.
 */
typedef void (*dictionary_iteration_callback_with_args)(char *key, void *value, void *args);

/**
 * @brief Callback function for processing keys of collection records.
 *
 * @param value Record value.
 */
typedef void (*dictionary_iteration_keys_callback)(char *key);

/**
 * @brief Callback function for processing keys of collection records with
 * arguments.
 *
 * @param value Record value.
 * @param args Arguments for callback function.
 */
typedef void (*dictionary_iteration_keys_callback_with_args)(char *key, void *args);

/**
 * @brief Callback function for processing values of collection records.
 *
 * @param value Record value.
 */
typedef void (*dictionary_iteration_values_callback)(void *value);

/**
 * @brief Callback function for processing values of collection records with
 * arguments.
 *
 * @param value Record value.
 * @param args Arguments for callback function.
 */
typedef void (*dictionary_iteration_values_callback_with_args)(void *value, void *args);

/**
 * @brief Callback function for processing records of collection.
 *
 * @param record Record.
 */
typedef void (*dictionary_iteration_records_callback)(p_record record);

/**
 * @brief Callback function for processing records of collection with arguments.
 *
 * @param record Record.
 * @param args Arguments for callback function.
 */
typedef void (*dictionary_iteration_records_callback_with_args)(p_record record, void *args);

/**
 * @brief Callback function for mapping records of collection.
 *
 * @param record Record.
 * @param index Record index.
 * @param dict Dictionary.
 * @return Mapped record.
 */
typedef p_record (*dictionary_iteration_callback_map)(
    const p_record record, int index, const p_dictionary dict);

/**
 * @brief Callback function for mapping records of collection.
 *
 * @param record Record.
 * @param index Record index.
 * @param dict Dictionary.
 * @param args Arguments for callback function.
 * @return Mapped record.
 */
typedef p_record (*dictionary_iteration_callback_map_with_args)(
    const p_record record, int index, const p_dictionary dict, void *args);

/**
 * @brief Callback function for filtering records of collection.
 *
 * @param record Record.
 * @param index Record index.
 * @param dict Dictionary.
 * @return Result of comparsion.
 */
typedef int (*dictionary_iteration_callback_filter)(
    const p_record record, int index, const p_dictionary dict);

/**
 * @brief Callback function for filtering records of collection.
 *
 * @param record Record.
 * @param index Record index.
 * @param dict Dictionary.
 * @param args Arguments for callback function.
 * @return Result of comparsion.
 */
typedef int (*dictionary_iteration_callback_filter_with_args)(
    const p_record record, int index, const p_dictionary dict, void *args);

/**
 * @brief Callback function for reducing records of collection.
 *
 * @param acc Accumulator.
 * @param record Record.
 * @param index Record index.
 * @param dict Dictionary.
 * @return Reduced record.
 */
typedef p_record (*dictionary_iteration_callback_reduce)(
    void *acc, const p_record record, int index, const p_dictionary dict);

/**
 * @brief Callback function for reducing records of collection.
 *
 * @param acc Accumulator.
 * @param record Record.
 * @param index Record index.
 * @param dict Dictionary.
 * @param args Arguments for callback function.
 * @return Reduced record.
 */
typedef p_record (*dictionary_iteration_callback_reduce_with_args)(
    void *acc, const p_record record, int index, const p_dictionary dict, void *args);

/**
 * @brief Callback function for sorting records of collection.
 *
 * @param record1 Record 1.
 * @param record2 Record 2.
 * @return Result of comparsion.
 */
typedef int (*dictionary_iteration_callback_sort)(
    const p_record record1, const p_record record2);

/**
 * @brief Callback function for sorting records of collection.
 *
 * @param record1 Record 1.
 * @param record2 Record 2.
 * @param args Arguments for callback function.
 * @return Result of comparsion.
 */
typedef int (*dictionary_iteration_callback_sort_with_args)(
    const p_record record1, const p_record record2, void *args);

/*********************************************************************************************
 * FUNCTIONS DECLARATIONS
 ********************************************************************************************/

/**
 * @brief Create dictionary.
 *
 * @details
 * Create a new empty dictionary.
 * Fields are NULLs by default.
 *
 * @return Dictionary.
 */
extern p_dictionary create_dictionary(void);

/**
 * @brief Create dictionary.
 *
 * @details
 * Create a new empty dictionary.
 * Fields are NULLs by default.
 *
 * @param metadata Metadata.
 *
 * @return Dictionary.
 */
extern p_dictionary create_dictionary_with_metadata(void *metadata);

/**
 * @brief Delete dictionary.
 *
 * @details
 * Delete dictionary and all its records.
 *
 * @param dict Dictionary object reference.
 */
extern void delete_dictionary(p_dictionary *dict);

/**
 * @brief Add record to dictionary.
 *
 * @details
 * Add a new record to specified dictionary.
 * The record is identified by string key and contains value of any type.
 *
 * @param dict Dictionary object.
 * @param key Record key.
 * @param value Record value.
 */
extern void add_record_to_dictionary(const p_dictionary dict, char *key, void *value);

/**
 * @brief Add record to begin of dictionary.
 *
 * @details
 * Add a new record to specified dictionary.
 * The record is identified by string key and contains value of any type.
 *
 * @param dict Dictionary object.
 * @param key Record key.
 * @param value Record value.
 */
extern void emplace_record_to_dictionary(const p_dictionary dict, char *key, void *value);

/**
 * @brief Add record to dictionary.
 *
 * @details
 * Add a new record to specified dictionary.
 * The record is identified by string key and contains value of any type.
 *
 * @param dict Dictionary object.
 * @param key Record key.
 * @param value Record value.
 * @param metadata Record metadata.
 */
extern void add_record_to_dictionary_with_metadata(
    const p_dictionary dict, char *key, void *value, void *metadata);

/**
 * @brief Add record to begin of dictionary.
 *
 * @details
 * Add a new record to specified dictionary.
 * The record is identified by string key and contains value of any type.
 *
 * @param dict Dictionary object.
 * @param key Record key.
 * @param value Record value.
 * @param metadata Record metadata.
 */
extern void emplace_record_to_dictionary_with_metadata(
    const p_dictionary dict, char *key, void *value, void *metadata);

/**
 * @brief Add record to dictionary by index.
 *
 * @details
 * Add a new record to specified dictionary by index.
 * The record is identified by string key and contains value of any type.
 *
 * @param dict Dictionary object.
 * @param index Record index.
 * @param key Record key.
 * @param value Record value.
 */
extern void add_record_to_dictionary_by_index(
    const p_dictionary dict, int index, char *key, void *value);

/**
 * @brief Add record to dictionary by index.
 *
 * @details
 * Add a new record to specified dictionary by index.
 * The record is identified by string key and contains value of any type.
 *
 * @param dict Dictionary object.
 * @param index Record index.
 * @param key Record key.
 * @param value Record value.
 * @param metadata Record metadata.
 */
extern void add_record_to_dictionary_by_index_with_metadata(
    const p_dictionary dict, int index, char *key, void *value, void *metadata);

/**
 * @brief Remove record from dictionary.
 *
 * @details
 * Remove a record from the specified dictionary.
 *
 * @param dict Dictionary object.
 * @param key Record key.
 *
 * @return Value of removed record.
 */
extern void *remove_record_from_dictionary(const p_dictionary dict, char *key);

/**
 * @brief Remove record from dictionary by index.
 *
 * @details
 * Remove a record from the specified dictionary by index.
 *
 * @param dict Dictionary object.
 * @param index Record index.
 *
 * @return Value of removed record
 */
extern void *remove_record_from_dictionary_by_index(const p_dictionary dict, int index);

/**
 * @brief Update record in dictionary.
 *
 * @details
 * Update a record in the specified dictionary.
 *
 * @param dict Dictionary object.
 * @param key Record key.
 * @param value Record value.
 *
 * @return Value of updated record.
 */
extern void *update_record_in_dictionary(const p_dictionary dict, char *key, void *value);

/**
 * @brief Update record in dictionary by index.
 *
 * @details
 * Update a record in the specified dictionary by index.
 *
 * @param dict Dictionary object.
 * @param index Record index.
 * @param value Record value.
 *
 * @return Value of updated record.
 */
extern void *update_record_in_dictionary_by_index(const p_dictionary dict, int index, void *value);

/**
 * @brief Check if dictionary contains specified key.
 *
 * @param dict Dictionary object.
 * @param key Record key.
 * @return 1 if dictionary contains key, 0 otherwise.
 */
extern int contains_key_in_dictionary(const p_dictionary dict, char *key);

/**
 * @brief Check if dictionary contains specified value.
 *
 * @param dict Dictionary object.
 * @param value Record value.
 * @return 1 if dictionary contains value, 0 otherwise.
 */
extern int contains_value_in_dictionary(const p_dictionary dict, void *value);

/**
 * @brief Get value from dictionary by key.
 *
 * @details
 * Get first matching record from specified dictionary.
 * Return the value of found record, if it exists else NULL.
 *
 * @param dict Dictionary.
 * @param key Record key.
 * @return Record value.
 */
extern void *get_value_from_dictionary(const p_dictionary dict, char *key);

/**
 * @brief Get value of head record from dictionary.
 *
 * @details
 * Get the head record from specified dictionary.
 * Return the value of head record, if it exists else NULL.
 *
 * @param dict Dictionary.
 * @return Record value.
 */
extern void *get_head_value_from_dictionary(const p_dictionary dict);

/**
 * @brief Get value of tail record from dictionary.
 *
 * @details
 * Get the tail record from specified dictionary.
 * Return the value of tail record, if it exists else NULL.
 *
 * @param dict Dictionary.
 * @return Record value.
 */
extern void *get_tail_value_from_dictionary(const p_dictionary dict);

/**
 * @brief Get value of next record after the current one.
 *
 * @details
 * Get the next record after the current one from specified dictionary by key of
 * current record. Return the value of next record, if it exists else NULL.
 *
 * @param dict Dictionary.
 * @param key Record key.
 * @return Next record value.
 */
extern void *get_next_value_from_dictionary(const p_dictionary dict, char *key);

/**
 * @brief Get value of previous record before the current one.
 *
 * @details
 * Get the previous record before the current one from specified dictionary by
 * key of current record. Return the value of previous record, if it exists else
 * NULL.
 *
 * @param dict Dictionary.
 * @param key Record key.
 * @return Previous record value.
 */
extern void *get_prev_value_from_dictionary(const p_dictionary dict, char *key);

/**
 * @brief Get head key from dictionary.
 *
 * @details
 * Get the head record from specified dictionary.
 * Return the key of head record, if it exists else NULL.
 *
 * @param dict Dictionary.
 * @return Record key.
 */
extern void *get_head_key_from_dictionary(const p_dictionary dict);

/**
 * @brief Get tail key from dictionary.
 *
 * @details
 * Get the tail record from specified dictionary.
 * Return the key of tail record, if it exists else NULL.
 *
 * @param dict Dictionary.
 * @return Record key.
 */
extern void *get_tail_key_from_dictionary(const p_dictionary dict);

/**
 * @brief Get key of next record after the current one.
 *
 * @details
 * Get the next record after the current one from specified dictionary by key of
 * current record. Return the key of next record, if it exists else NULL.
 *
 * @param dict Dictionary.
 * @param key Record key.
 * @return Next record key.
 */
extern void *get_next_key_from_dictionary(const p_dictionary dict, char *key);

/**
 * @brief Get key of previous record before the current one.
 *
 * @details
 * Get the previous record before the current one from specified dictionary by
 * key of current record. Return the key of previous record, if it exists else
 * NULL.
 *
 * @param dict Dictionary.
 * @param key Record key.
 * @return Previous record key.
 */
extern void *get_prev_key_from_dictionary(const p_dictionary dict, char *key);

/**
 * @brief Get record from dictionary.
 *
 * @details
 * Get first matching record from specified dictionary by key.
 * Return the record, if it exists else NULL.
 *
 * @param dict Dictionary.
 * @param key Record key.
 * @return Record.
 */
extern p_record get_record_from_dictionary(const p_dictionary dict, char *key);

/**
 * @brief Get records from dictionary by key.
 *
 * @details
 * Get all matching records from specified dictionary by key.
 * Return the dictionary of records, if it exists else NULL.
 *
 * @param dict Dictionary.
 * @param key Record key.
 * @return Dictionary of records.
 */
extern p_dictionary get_records_from_dictionary(const p_dictionary dict, char *key);

/**
 * @brief Get record from dictionary by value.
 *
 * @details
 * Get first matching record from specified dictionary by value.
 * Return the record, if it exists else NULL.
 *
 * @param dict Dictionary.
 * @param value Record value.
 * @return Record.
 */
extern p_record get_record_from_dictionary_by_value(const p_dictionary dict, void *value);

/**
 * @brief Get records from dictionary by value.
 *
 * @details
 * Get all matching records from specified dictionary by value.
 * Return the dictionary of records, if it exists else NULL.
 *
 * @param dict Dictionary.
 * @param value Record value.
 * @return Dictionary of records.
 */
extern p_dictionary get_records_from_dictionary_by_value(const p_dictionary dict, void *value);

/**
 * @brief Get head record from dictionary.
 *
 * @details
 * Get the head record from specified dictionary.
 * Return the head record, if it exists else NULL.
 *
 * @param dict Dictionary.
 * @return Record.
 */
extern p_record get_head_record_from_dictionary(const p_dictionary dict);

/**
 * @brief Get tail record from dictionary.
 *
 * @details
 * Get the tail record from specified dictionary.
 * Return the tail record, if it exists else NULL.
 *
 * @param dict Dictionary.
 * @return Record.
 */
extern p_record get_tail_record_from_dictionary(const p_dictionary dict);

/**
 * @brief Get record from dictionary by index.
 *
 * @details
 * Get first matching record from specified dictionary by index.
 * Return the record, if it exists else NULL.
 *
 * @param dict Dictionary.
 * @param index Record index.
 * @return Record.
 */
extern p_record get_record_from_dictionary_by_index(const p_dictionary dict, int index);

/**
 * @brief Get index from dictionary by key.
 *
 * @details
 * Get index of first matching record from specified dictionary by key.
 * Return the index, if it exists else -1.
 *
 * @param dict Dictionary.
 * @param key Record key.
 * @return Record index.
 */
extern int get_index_from_dictionary_by_key(const p_dictionary dict, char *key);

/**
 * @brief Get index from dictionary by record.
 *
 * @details
 * Get index of first matching record from specified dictionary by record.
 * Return the index, if it exists else -1.
 *
 * @param dict Dictionary.
 * @param record Record.
 * @return Record index.
 */
extern int get_index_from_dictionary_by_ref_record(const p_dictionary dict, p_record record);

/**
 * @brief Iterate over dictionary.
 *
 * @details
 * Iterate over all records of specfied dictionary.
 * Apply a callback function to each record.
 *
 * @param dict Dictionary.
 * @param callback Callback function.
 */
extern void iterate_over_dictionary(const p_dictionary dict, dictionary_iteration_callback callback);

/**
 * @brief Iterate over dictionary with arguments.
 *
 * @details
 * Iterate over all records of specfied dictionary.
 * Apply a callback function to each record.
 *
 * @param dict Dictionary.
 * @param callback Callback function.
 * @param args Arguments for callback function.
 */
extern void iterate_over_dictionary_with_args(
    const p_dictionary dict, dictionary_iteration_callback_with_args callback, void *args);

/**
 * @brief Iterate over keys of dictionary.
 *
 * @details
 * Iterate over all keys of specfied dictionary.
 * Apply a callback function to each key.
 *
 * @param dict Dictionary.
 * @param callback Callback function.
 */
extern void iterate_over_dictionary_keys(
    const p_dictionary dict, dictionary_iteration_keys_callback callback);

/**
 * @brief Iterate over dictionary keys with arguments.
 *
 * @details
 * Iterate over all keys of specfied dictionary.
 * Apply a callback function to each key.
 *
 * @param dict Dictionary.
 * @param callback Callback function.
 * @param args Arguments for callback function.
 */
extern void iterate_over_dictionary_keys_with_args(
    const p_dictionary dict, dictionary_iteration_keys_callback_with_args callback, void *args);

/**
 * @brief Iterate over values of dictionary.
 *
 * @details
 * Iterate over all values of specfied dictionary.
 * Apply a callback function to each value.
 *
 * @param dict Dictionary.
 * @param callback Callback function.
 */
extern void iterate_over_dictionary_values(
    const p_dictionary dict, dictionary_iteration_values_callback callback);

/**
 * @brief Iterate over dictionary values with arguments.
 *
 * @details
 * Iterate over all values of specfied dictionary.
 * Apply a callback function to each value.
 *
 * @param dict Dictionary.
 * @param callback Callback function.
 * @param args Arguments for callback function.
 */
extern void iterate_over_dictionary_values_with_args(
    const p_dictionary dict, dictionary_iteration_values_callback_with_args callback, void *args);

/**
 * @brief Iterate over dictionary records.
 *
 * @details
 * Iterate over all records of specfied dictionary.
 * Apply a callback function to each record.
 *
 * @param dict Dictionary.
 * @param callback Callback function.
 */
extern void iterate_over_dictionary_records(
    const p_dictionary dict, dictionary_iteration_records_callback callback);

/**
 * @brief Iterate over dictionary records with arguments.
 *
 * @details
 * Iterate over all records of specfied dictionary.
 * Apply a callback function to each record.
 *
 * @param dict Dictionary.
 * @param callback Callback function.
 * @param args Arguments for callback function.
 */
extern void iterate_over_dictionary_records_with_args(
    const p_dictionary dict, dictionary_iteration_records_callback_with_args callback, void *args);

/**
 * @brief Map dictionary records.
 *
 * @details
 * Map all records of specfied dictionary.
 * Apply a callback function to each record.
 *
 * @param dict Dictionary.
 * @param callback Callback function.
 * @return Mapped dictionary.
 */
extern p_dictionary map_dictionary(
    const p_dictionary dict, dictionary_iteration_callback_map callback);

/**
 * @brief Map dictionary records with arguments
 *
 * @details
 * Map all records of specfied dictionary.
 * Apply a callback function to each record.
 *
 * @param dict Dictionary.
 * @param callback Callback function.
 * @param args Arguments for callback function.
 * @return Mapped dictionary.
 */
extern p_dictionary map_dictionary_with_args(
    const p_dictionary dict, dictionary_iteration_callback_map_with_args callback, void *args);

/**
 * @brief Filter dictionary records.
 *
 * @details
 * Filter all records of specfied dictionary.
 * Apply a callback function to each record.
 *
 * @param dict Dictionary.
 * @param callback Callback function.
 * @return Filtered dictionary.
 */
extern p_dictionary filter_dictionary(
    const p_dictionary dict, dictionary_iteration_callback_filter callback);

/**
 * @brief Filter dictionary records with arguments
 *
 * @details
 * Filter all records of specfied dictionary.
 * Apply a callback function to each record.
 *
 * @param dict Dictionary.
 * @param callback Callback function.
 * @param args Arguments for callback function.
 * @return Filtered dictionary.
 */
extern p_dictionary filter_dictionary_with_args(
    const p_dictionary dict, dictionary_iteration_callback_filter_with_args callback, void *args);

/**
 * @brief Reduce dictionary records.
 *
 * @details
 * Reduce all records of specfied dictionary.
 * Apply a callback function to each record.
 *
 * @param dict Dictionary.
 * @param callback Callback function.
 * @param acc Initial accumulator value.
 * @return Reduced dictionary.
 */
extern void *reduce_dictionary(
    const p_dictionary dict, dictionary_iteration_callback_reduce callback, void *acc);

/**
 * @brief Reduce dictionary records with arguments
 *
 * @details
 * Reduce all records of specfied dictionary.
 * Apply a callback function to each record.
 *
 * @param dict Dictionary.
 * @param callback Callback function.
 * @param acc Initial accumulator value.
 * @param args Arguments for callback function.
 * @return Reduced dictionary.
 */
extern void *reduce_dictionary_with_args(
    const p_dictionary dict,
    dictionary_iteration_callback_reduce_with_args callback,
    void *acc, void *args);

/**
 * @brief Sort dictionary records.
 *
 * @details
 * Sort all records of specfied dictionary.
 * Apply a callback function to each record.
 *
 * @param dict Dictionary.
 * @param callback Callback function.
 */
extern p_dictionary sort_dictionary(
    const p_dictionary dict, dictionary_iteration_callback_sort callback);

/**
 * @brief Sort dictionary records with arguments.
 *
 * @details
 * Sort all records of specfied dictionary.
 * Apply a callback function to each record.
 *
 * @param dict Dictionary.
 * @param callback Callback function.
 * @param args Arguments for callback function.
 */
extern p_dictionary sort_dictionary_with_args(
    const p_dictionary dict, dictionary_iteration_callback_sort_with_args callback, void *args);

#endif // IPEE_DICTIONARY_H