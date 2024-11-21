/*********************************************************************************************
 * INCLUDES DECLARATIONS
 ********************************************************************************************/

#include <hashmap.h>

#include <stdlib.h>
#include <string.h>

#include <macro.h>

/*********************************************************************************************
 * MACROS DECLARATIONS
 ********************************************************************************************/

#define HASHMAP_DEFAULT_CAPACITY 20
#define HASHMAP_MAX_LOAD 0.75f
#define HASHMAP_RESIZE_FACTOR 2

#define HASHMAP_HASH_INIT 2166136261u

/*********************************************************************************************
 * STRUCTS DECLARATIONS
 ********************************************************************************************/

typedef struct bucket_s {
    struct bucket_s *next;  // Next backet reference.
    p_key key;              // Bucket entry key.
    size_t ksize;           // Bucket entry key size.
    uint32_t hash;          // Bucket entry hash.
    void *value;            // Value in bucket entry.
} bucket_t, *p_bucket;

typedef struct hashmap_s {
    p_bucket buckets;                           // Pointer to linked list of buckets.
    int capacity;                               // Allocated size for the count of entries.
    int count;                                  // Count of set entries in the hash map.
    int tombstone_count;                        // Tombstones are empty buckets after items have been removed.
    p_bucket first;                             // Pointer to linked list of all valid entries arranged in order.
    p_bucket last;                              // Necessary to know where to add next entry.
} hashmap_t, *p_hashmap;

/***********************************************************************************************
 * STATIC FUNCTIONS DECLARATIONS
 **********************************************************************************************/

/**
 * @brief Put an old bucket into a resized hashmap.
 * 
 * @param map       Pointer to hashmap.
 * @param old_entry Old bucket entry.
 * 
 * @return Pointer to bucket entry.
 */
static p_bucket resize_entry(p_hashmap map, p_bucket old_entry);

/**
 * @brief Rezise hashmap.
 * 
 * @param map       Pointer to hashmap.
 * 
 * @return Error code.
 */
static int hashmap_resize(p_hashmap map);

/**
 * @brief Jenkins one-at-a-time hash function.
 * 
 * @param data      Pointer to key for bucket entry.
 * @param size      Key size for bucket entry.
 * 
 * @return Hash value.
 */
static inline uint32_t hash_data(const unsigned char *data, size_t size);

/**
 * @brief Searching for an entry in hashmap.
 * 
 * @param map       Pointer to hashmap.
 * @param key       Pointer to key for bucket entry.
 * @param ksize     Key size for bucket entry.
 * @param hash      Hash value.
 * 
 * @return Pointer to bucket entry.
 */
static p_bucket find_entry(p_hashmap map, p_key key, size_t ksize, uint32_t hash);

/***********************************************************************************************
 * FUNCTIONS DEFINITIONS
 **********************************************************************************************/

p_hashmap hashmap_create(void) {
    p_hashmap map = malloc(sizeof(hashmap_t));

    if (!map) {
        return NULL;
    }

    map->capacity = HASHMAP_DEFAULT_CAPACITY;
    map->count = 0;
    map->tombstone_count = 0;

    map->buckets = calloc(HASHMAP_DEFAULT_CAPACITY, sizeof(bucket_t));

    if (!map->buckets) {
        free(map);
        return NULL;
    }

    map->first = NULL;
    map->last = (p_bucket)&map->first;

    return map;
}

void hashmap_set_entry(p_hashmap map, p_key key, void *value) {
    if (!map) exit(IPEE_ERROR_CODE__HASHMAP__NOT_EXISTS);

    if (map->count + 1 > HASHMAP_MAX_LOAD * map->capacity) {
        if (hashmap_resize(map) == -1)
            exit(IPEE_ERROR_CODE__HASHMAP__INDEX_OUT_OF_MEMORY);
    }

    size_t ksize = strlen(key);

    uint32_t hash = hash_data(key, ksize);
    p_bucket entry = find_entry(map, key, ksize, hash);
    if (!entry->key) {
        map->last->next = entry;
        map->last = entry;
        entry->next = NULL;

        ++map->count;

        entry->key = key;
        entry->ksize = ksize;
        entry->hash = hash;
    }

    entry->value = value;
}

void *hashmap_get_entry(p_hashmap map, p_key key) {
    if (!map) return NULL;

    size_t ksize = strlen(key);

    uint32_t hash = hash_data(key, ksize);
    p_bucket entry = find_entry(map, key, ksize, hash);

    return entry ? entry->value : NULL;
}

void hashmap_remove_entry(p_hashmap map, p_key key) {
    if (!map) exit(IPEE_ERROR_CODE__HASHMAP__NOT_EXISTS);

    size_t ksize = strlen(key);

    uint32_t hash = hash_data(key, ksize);
    p_bucket entry = find_entry(map, key, ksize, hash);

    if (entry->key) {
        entry->key = NULL;
        ++map->tombstone_count;
    }
}

void hashmap_remove_all_entries(p_hashmap map) {
    if (!map) exit(IPEE_ERROR_CODE__HASHMAP__NOT_EXISTS);
    
    p_bucket current = map->first;

    while (current) {
        if (current->key) {
            current->key = NULL;
        }

        current = current->next;
    }

    map->capacity = HASHMAP_DEFAULT_CAPACITY;
    map->count = 0;
    map->tombstone_count = 0;

    free(map->buckets);

    map->buckets = calloc(HASHMAP_DEFAULT_CAPACITY, sizeof(bucket_t));

    if (!map->buckets) {
        exit(IPEE_ERROR_CODE__HASHMAP__INDEX_OUT_OF_MEMORY);
    }

    map->first = NULL;
    map->last = (p_bucket)&map->first;
}

void hashmap_remove(p_hashmap *map) {
    if (!map || !(*map)) return;

    p_bucket current = (*map)->first;

    while (current) {
        if (current->key) {
            current->key = NULL;
        }

        current = current->next;
    }

    free((*map)->buckets);
    (*map)->buckets = NULL;

    free(*map);
    (*map) = NULL;
}

int hashmap_get_count(p_hashmap map) {
    if (!map) return -1;

    return map->count - map->tombstone_count;
}

void hashmap_iterate(p_hashmap map, hashmap_iteration_callback callback) {
    if (!map) exit(IPEE_ERROR_CODE__HASHMAP__NOT_EXISTS);;

    p_bucket current = map->first;
    
    while (current) {
        if (current->key) {
            callback(current->key, current->value);
        }

        current = current->next;
    }
}

/***********************************************************************************************
 * STATIC FUNCTIONS DEFINITIONS
 **********************************************************************************************/

static p_bucket resize_entry(p_hashmap map, p_bucket old_entry) {
    uint32_t index = old_entry->hash % map->capacity;
    p_bucket entry = NULL;

    while (1) {
        entry = &map->buckets[index];

        if (!entry->key) {
            *entry = *old_entry;
            return entry;
        }

        index = (index + 1) % map->capacity;
    }
}

static int hashmap_resize(p_hashmap map) {
    int old_capacity = map->capacity;
    p_bucket old_buckets = map->buckets;
    p_bucket current = NULL;

    map->capacity *= HASHMAP_RESIZE_FACTOR;
    map->buckets = calloc(map->capacity, sizeof(bucket_t));

    if (!map->buckets) {
        map->capacity = old_capacity;
        map->buckets = old_buckets;
        return -1;
    }

    map->last = (p_bucket)&map->first;

    map->count -= map->tombstone_count;
    map->tombstone_count = 0;

    do {
        current = map->last->next;
        if (!current->key)
        {
            map->last->next = current->next;
            continue;
        }

        map->last->next = resize_entry(map, map->last->next);
        map->last = map->last->next;
    } while (map->last->next);

    free(old_buckets);
    return 0;
}

static inline size_t hashmap_hash_default(const void *data, size_t size)
{
    const uint8_t *byte = (const uint8_t *)data;
    size_t hash = 0;

    for (size_t i = 0; i < size; ++i) {
        hash += *byte++;
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}

static inline uint32_t hash_data(const unsigned char *data, size_t size)
{
    size_t nblocks = size / 8;
    uint64_t hash = HASHMAP_HASH_INIT;
    for (size_t i = 0; i < nblocks; ++i) {
        hash ^= (uint64_t)data[0] << 0 | (uint64_t)data[1] << 8 |
                (uint64_t)data[2] << 16 | (uint64_t)data[3] << 24 |
                (uint64_t)data[4] << 32 | (uint64_t)data[5] << 40 |
                (uint64_t)data[6] << 48 | (uint64_t)data[7] << 56;
        hash *= 0xbf58476d1ce4e5b9;
        data += 8;
    }

    uint64_t last = size & 0xff;
    switch (size % 8) {
    case 7:
        last |= (uint64_t)data[6] << 56;
    case 6:
        last |= (uint64_t)data[5] << 48;
    case 5:
        last |= (uint64_t)data[4] << 40;
    case 4:
        last |= (uint64_t)data[3] << 32;
    case 3:
        last |= (uint64_t)data[2] << 24;
    case 2:
        last |= (uint64_t)data[1] << 16;
    case 1:
        last |= (uint64_t)data[0] << 8;
        hash ^= last;
        hash *= 0xd6e8feb86659fd93;
    }

    return (uint32_t)(hash ^ hash >> 32);
}

static p_bucket find_entry(p_hashmap map, p_key key, size_t ksize, uint32_t hash) {
    uint32_t index = hash % map->capacity;
    p_bucket entry = NULL;

    while (1) {
        entry = &map->buckets[index];

        if ((!entry->key && !entry->value) ||
            (entry->key             &&
             entry->ksize == ksize  &&
             entry->hash == hash    &&
             memcmp(entry->key, key, ksize) == 0)) {
            return entry;
        }

        index = (index + 1) % map->capacity;
    }
}
