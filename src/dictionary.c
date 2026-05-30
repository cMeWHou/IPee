#include <dictionary.h>

#include <stdlib.h>
#include <string.h>

#include <macro.h>

/***********************************************************************************************
 * FUNCTIONS DEFINITIONS
 **********************************************************************************************/

p_dictionary create_dictionary(void) {
    return create_dictionary_with_metadata(NULL);
}

p_dictionary create_dictionary_with_metadata(void *metadata) {
    p_dictionary dict = (p_dictionary)malloc(sizeof(dictionary_t));
    if (!dict) {
        return NULL;
    }

    dict->size = 0;
    dict->head = NULL;
    dict->tail = NULL;
    dict->metadata = metadata;

    return dict;
}

int delete_dictionary(p_dictionary dict) {
    if (!dict) {
        return IPEE_ERROR_CODE__DICTIONARY__NOT_EXISTS;
    }

    p_record current = dict->head;
    while (current) {
        p_record next = current->next;
        free(current);
        current = next;
    }

    free(dict);
    return 0;
}

int add_record_to_dictionary(const p_dictionary dict, char *key, void *value) {
    return add_record_to_dictionary_with_metadata(dict, key, value, NULL);
}

int add_record_to_dictionary_with_metadata(
    const p_dictionary dict, char *key, void *value, void *metadata) {
    if (!dict)
        return IPEE_ERROR_CODE__DICTIONARY__NOT_EXISTS;

    p_record record = (p_record)malloc(sizeof(record_t));
    if (!record)
        return IPEE_ERROR_CODE__DICTIONARY__RECORD_CREATION_ERROR;

    record->key = key;
    record->value = value;
    record->next = NULL;
    record->prev = dict->tail;
    record->metadata = metadata;

    if (!dict->size) {
        dict->head = record;
        dict->tail = record;
    } else {
        dict->tail->next = record;
        dict->tail = record;
    }
    dict->size++;
    return 0;
}

int emplace_record_to_dictionary(const p_dictionary dict, char *key, void *value) {
    return add_record_to_dictionary_by_index(dict, 0, key, value);
}

int emplace_record_to_dictionary_with_metadata(
    const p_dictionary dict, char *key, void *value, void *metadata) {
    return add_record_to_dictionary_by_index_with_metadata(dict, 0, key, value, metadata);
}

int add_record_to_dictionary_by_index(
    const p_dictionary dict, int index, char *key, void *value) {
    return add_record_to_dictionary_by_index_with_metadata(dict, index, key, value, NULL);
}

int add_record_to_dictionary_by_index_with_metadata(
    const p_dictionary dict, int index, char *key, void *value, void *metadata) {
    if (!dict)
        return IPEE_ERROR_CODE__DICTIONARY__NOT_EXISTS;
    if (index < 0 || index > dict->size)
        return IPEE_ERROR_CODE__DICTIONARY__INDEX_OUT_OF_RANGE;

    p_record record = (p_record)malloc(sizeof(record_t));
    if (!record)
        return IPEE_ERROR_CODE__DICTIONARY__RECORD_CREATION_ERROR;

    record->key = key;
    record->value = value;
    record->next = NULL;
    record->prev = NULL;
    record->metadata = metadata;

    if (!dict->size || index == dict->size) {
        record->prev = dict->tail;
        if (dict->tail)
            dict->tail->next = record;
        else
            dict->head = record;
        dict->tail = record;
        dict->size++;
        return 0;
    }

    p_record current = get_record_from_dictionary_by_index(dict, index);
    if (!current) {
        free(record);
        return IPEE_ERROR_CODE__DICTIONARY__INDEX_OUT_OF_RANGE;
    }

    if (dict->head == current) {
        record->next = current;
        current->prev = record;
        dict->head = record;
    } else {
        record->next = current;
        record->prev = current->prev;
        if (current->prev)
            current->prev->next = record;
        current->prev = record;
    }
    dict->size++;
    return 0;
}

void *remove_record_from_dictionary(const p_dictionary dict, char *key) {
    if (!dict)
        return NULL;

    p_record current = dict->head;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            if (current->prev)
                current->prev->next = current->next;
            else
                dict->head = current->next;

            if (current->next)
                current->next->prev = current->prev;
            else
                dict->tail = current->prev;

            void *removed_value = current->value;
            free(current);
            dict->size--;
            return removed_value;
        }
        current = current->next;
    }
    return NULL;
}

void *remove_record_from_dictionary_by_index(const p_dictionary dict, int index) {
    if (!dict)
        return NULL;
    if (index < 0 || index >= dict->size)
        return NULL;

    p_record current = dict->head;
    for (int i = 0; i < index; i++) {
        if (!current)
            return NULL;
        current = current->next;
    }

    if (current->prev)
        current->prev->next = current->next;
    else
        dict->head = current->next;

    if (current->next)
        current->next->prev = current->prev;
    else
        dict->tail = current->prev;

    void *removed_value = current->value;
    free(current);
    dict->size--;
    return removed_value;
}

void *update_record_in_dictionary(const p_dictionary dict, char *key, void *value) {
    void *old_value = NULL;
    p_record record = get_record_from_dictionary(dict, key);
    if (record) {
        old_value = record->value;
        record->value = value;
    }
    return old_value;
}

void *update_record_in_dictionary_by_index(const p_dictionary dict, int index, void *value) {
    void *old_value = NULL;
    p_record record = get_record_from_dictionary_by_index(dict, index);
    if (record) {
        old_value = record->value;
        record->value = value;
    }
    return old_value;
}

int contains_key_in_dictionary(const p_dictionary dict, char *key) {
    return get_record_from_dictionary(dict, key) != NULL;
}

int contains_value_in_dictionary(const p_dictionary dict, void *value) {
    return get_record_from_dictionary_by_value(dict, value) != NULL;
}

void *get_value_from_dictionary(const p_dictionary dict, char *key) {
    p_record record = get_record_from_dictionary(dict, key);
    return record ? record->value : NULL;
}

void *get_head_value_from_dictionary(const p_dictionary dict) {
    if (!dict)
        return NULL;

    return dict->head ? dict->head->value : NULL;
}

void *get_tail_value_from_dictionary(const p_dictionary dict) {
    if (!dict)
        return NULL;

    return dict->tail ? dict->tail->value : NULL;
}

void *get_next_value_from_dictionary(const p_dictionary dict, char *key) {
    p_record record = get_record_from_dictionary(dict, key);
    return record && record->next ? record->next->value : NULL;
}

void *get_prev_value_from_dictionary(const p_dictionary dict, char *key) {
    p_record record = get_record_from_dictionary(dict, key);
    return record && record->prev ? record->prev->value : NULL;
}

char *get_head_key_from_dictionary(const p_dictionary dict) {
    if (!dict)
        return NULL;

    return dict->head ? dict->head->key : NULL;
}

char *get_tail_key_from_dictionary(const p_dictionary dict) {
    if (!dict)
        return NULL;

    return dict->tail ? dict->tail->key : NULL;
}

char *get_next_key_from_dictionary(const p_dictionary dict, char *key) {
    p_record record = get_record_from_dictionary(dict, key);
    return record && record->next ? record->next->key : NULL;
}

char *get_prev_key_from_dictionary(const p_dictionary dict, char *key) {
    p_record record = get_record_from_dictionary(dict, key);
    return record && record->prev ? record->prev->key : NULL;
}

p_record get_record_from_dictionary(const p_dictionary dict, char *key) {
    if (!dict)
        return NULL;

    p_record record = dict->head;
    while (record) {
        if (strcmp(record->key, key) == 0)
            return record;
        record = record->next;
    }
    return NULL;
}

p_dictionary get_records_from_dictionary(const p_dictionary dict, char *key) {
    if (!dict)
        return NULL;

    p_dictionary records_dict = create_dictionary();
    p_record record = dict->head;
    while (record) {
        if (strcmp(record->key, key) == 0)
            add_record_to_dictionary(records_dict, record->key, record->value);
        record = record->next;
    }
    return records_dict;
}

p_record get_record_from_dictionary_by_value(const p_dictionary dict, void *value) {
    if (!dict)
        return NULL;

    p_record record = dict->head;
    while (record) {
        if (record->value == value)
            return record;
        record = record->next;
    }
    return NULL;
}

p_dictionary get_records_from_dictionary_by_value(const p_dictionary dict, void *value) {
    if (!dict)
        return NULL;

    p_dictionary records_dict = create_dictionary();
    p_record record = dict->head;
    while (record) {
        if (record->value == value)
            add_record_to_dictionary(records_dict, record->key, record->value);
        record = record->next;
    }
    return records_dict;
}

p_record get_head_record_from_dictionary(const p_dictionary dict) {
    if (!dict)
        return NULL;

    return dict->head;
}

p_record get_tail_record_from_dictionary(const p_dictionary dict) {
    if (!dict)
        return NULL;

    return dict->tail;
}

p_record get_record_from_dictionary_by_index(const p_dictionary dict, int index) {
    if (!dict)
        return NULL;
    if (index < 0 || index >= dict->size)
        return NULL;

    p_record record = dict->head;
    for (int i = 0; i < index; i++) {
        if (!record)
            return NULL;
        record = record->next;
    }
    return record;
}

int get_index_from_dictionary_by_key(const p_dictionary dict, char *key) {
    if (!dict)
        return IPEE_ERROR_CODE__DICTIONARY__NOT_EXISTS;

    p_record record = dict->head;
    int index = 0;
    while (record) {
        if (strcmp(record->key, key) == 0)
            return index;
        record = record->next;
        index++;
    }
    return -1;
}

int get_index_from_dictionary_by_ref_record(const p_dictionary dict, p_record record) {
    if (!dict)
        return IPEE_ERROR_CODE__DICTIONARY__NOT_EXISTS;

    p_record current = dict->head;
    int index = 0;
    while (current) {
        if (current == record)
            return index;
        current = current->next;
        index++;
    }
    return -1;
}

int iterate_over_dictionary(const p_dictionary dict, dictionary_iteration_callback callback) {
    if (!dict)
        return IPEE_ERROR_CODE__DICTIONARY__NOT_EXISTS;

    p_record current = dict->head;
    while (current) {
        p_record next = current->next;
        callback(current->key, current->value);
        current = next;
    }
    return 0;
}

int iterate_over_dictionary_with_args(
    const p_dictionary dict, dictionary_iteration_callback_with_args callback,
    void *args) {
    if (!dict)
        return IPEE_ERROR_CODE__DICTIONARY__NOT_EXISTS;

    p_record current = dict->head;
    while (current) {
        p_record next = current->next;
        callback(current->key, current->value, args);
        current = next;
    }
    return 0;
}

int iterate_over_dictionary_keys(
    const p_dictionary dict, dictionary_iteration_keys_callback callback) {
    if (!dict)
        return IPEE_ERROR_CODE__DICTIONARY__NOT_EXISTS;

    p_record current = dict->head;
    while (current) {
        p_record next = current->next;
        callback(current->key);
        current = next;
    }
    return 0;
}

int iterate_over_dictionary_keys_with_args(
    const p_dictionary dict, dictionary_iteration_keys_callback_with_args callback, void *args) {
    if (!dict)
        return IPEE_ERROR_CODE__DICTIONARY__NOT_EXISTS;

    p_record current = dict->head;
    while (current) {
        p_record next = current->next;
        callback(current->key, args);
        current = next;
    }
    return 0;
}

int iterate_over_dictionary_values(
    const p_dictionary dict, dictionary_iteration_values_callback callback) {
    if (!dict)
        return IPEE_ERROR_CODE__DICTIONARY__NOT_EXISTS;

    p_record current = dict->head;
    while (current) {
        p_record next = current->next;
        callback(current->value);
        current = next;
    }
    return 0;
}

int iterate_over_dictionary_values_with_args(
    const p_dictionary dict, dictionary_iteration_values_callback_with_args callback, void *args) {
    if (!dict)
        return IPEE_ERROR_CODE__DICTIONARY__NOT_EXISTS;

    p_record current = dict->head;
    while (current) {
        p_record next = current->next;
        callback(current->value, args);
        current = next;
    }
    return 0;
}

int iterate_over_dictionary_records(
    const p_dictionary dict, dictionary_iteration_records_callback callback) {
    if (!dict)
        return IPEE_ERROR_CODE__DICTIONARY__NOT_EXISTS;

    p_record current = dict->head;
    while (current) {
        p_record next = current->next;
        callback(current);
        current = next;
    }
    return 0;
}

int iterate_over_dictionary_records_with_args(
    const p_dictionary dict, dictionary_iteration_records_callback_with_args callback, void *args) {
    if (!dict)
        return IPEE_ERROR_CODE__DICTIONARY__NOT_EXISTS;

    p_record current = dict->head;
    while (current) {
        p_record next = current->next;
        callback(current, args);
        current = next;
    }
    return 0;
}

p_dictionary map_dictionary(
    const p_dictionary dict, dictionary_iteration_callback_map callback) {
    if (!dict)
        return NULL;

    p_dictionary new_dict = create_dictionary();
    p_record current = dict->head;
    int index = 0;
    while (current) {
        p_record next = current->next;
        p_record new_record = callback(current, index, dict);
        if (new_record)
            add_record_to_dictionary(new_dict, new_record->key, new_record->value);
        current = next;
        index++;
    }
    return new_dict;
}

p_dictionary map_dictionary_with_args(
    const p_dictionary dict,
    dictionary_iteration_callback_map_with_args callback, void *args) {
    if (!dict)
        return NULL;

    p_dictionary new_dict = create_dictionary();
    p_record current = dict->head;
    int index = 0;
    while (current) {
        p_record next = current->next;
        p_record new_record = callback(current, index, dict, args);
        if (new_record)
            add_record_to_dictionary(new_dict, new_record->key, new_record->value);
        current = next;
        index++;
    }
    return new_dict;
}

p_dictionary filter_dictionary(
    const p_dictionary dict, dictionary_iteration_callback_filter callback) {
    if (!dict)
        return NULL;

    p_dictionary new_dict = create_dictionary();
    p_record current = dict->head;
    int index = 0;
    while (current) {
        p_record next = current->next;
        if (callback(current, index, dict))
            add_record_to_dictionary(new_dict, current->key, current->value);
        current = next;
        index++;
    }
    return new_dict;
}

p_dictionary filter_dictionary_with_args(
    const p_dictionary dict,
    dictionary_iteration_callback_filter_with_args callback, void *args) {
    if (!dict)
        return NULL;

    p_dictionary new_dict = create_dictionary();
    p_record current = dict->head;
    int index = 0;
    while (current) {
        p_record next = current->next;
        if (callback(current, index, dict, args))
            add_record_to_dictionary(new_dict, current->key, current->value);
        current = next;
        index++;
    }
    return new_dict;
}

void *reduce_dictionary(
    const p_dictionary dict,
    dictionary_iteration_callback_reduce callback, void *acc) {
    if (!dict)
        return NULL;

    p_record current = dict->head;
    int index = 0;
    while (current) {
        p_record next = current->next;
        callback(acc, current, index, dict);
        current = next;
        index++;
    }
    return acc;
}

void *reduce_dictionary_with_args(
    const p_dictionary dict,
    dictionary_iteration_callback_reduce_with_args callback, void *acc, void *args) {
    if (!dict)
        return NULL;

    p_record current = dict->head;
    int index = 0;
    while (current) {
        p_record next = current->next;
        callback(acc, current, index, dict, args);
        current = next;
        index++;
    }
    return acc;
}

p_dictionary sort_dictionary(
    const p_dictionary dict, dictionary_iteration_callback_sort callback) {
    if (!dict)
        return NULL;

    p_dictionary new_dict = create_dictionary();
    p_record current = dict->head;
    while (current) {
        add_record_to_dictionary(new_dict, current->key, current->value);
        current = current->next;
    }
    p_record current1 = new_dict->head;
    while (current1) {
        p_record current2 = current1->next;
        while (current2) {
            if (callback(current1, current2)) {
                char *key = current1->key;
                void *value = current1->value;
                current1->key = current2->key;
                current1->value = current2->value;
                current2->key = key;
                current2->value = value;
            }
            current2 = current2->next;
        }
        current1 = current1->next;
    }
    return new_dict;
}

p_dictionary sort_dictionary_with_args(
    const p_dictionary dict, dictionary_iteration_callback_sort_with_args callback, void *args) {
    if (!dict)
        return NULL;

    p_dictionary new_dict = create_dictionary();
    p_record current = dict->head;
    while (current) {
        add_record_to_dictionary(new_dict, current->key, current->value);
        current = current->next;
    }
    p_record current1 = new_dict->head;
    while (current1) {
        p_record current2 = current1->next;
        while (current2) {
            if (callback(current1, current2, args)) {
                char *key = current1->key;
                void *value = current1->value;
                current1->key = current2->key;
                current1->value = current2->value;
                current2->key = key;
                current2->value = value;
            }
            current2 = current2->next;
        }
        current1 = current1->next;
    }
    return new_dict;
}
