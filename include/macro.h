#ifndef IPEE_MACRO_H
#define IPEE_MACRO_H

/**
 * @brief Returns stringified parameter.
 *
 * @details
 * Converts parameter to string.
 */
#define stringify(param) #param

/**
 * @brief Return stringified name.
 */
#define name_of(param) stringify(param)

/**
 * @brief IPee error codes.
 */
typedef enum ipee_error_code_e {
    /** Dictionary. */
    IPEE_ERROR_CODE__DICTIONARY__NOT_EXISTS,
    IPEE_ERROR_CODE__DICTIONARY__INDEX_OUT_OF_RANGE,
    IPEE_ERROR_CODE__DICTIONARY__RECORD_CREATION_ERROR,

    /** Bitmap. */
    IPEE_ERROR_CODE__BITMAP__NOT_EXISTS,
    IPEE_ERROR_CODE__BITMAP__INDEX_OUT_OF_RANGE,

    /** Container. */
    IPEE_ERROR_CODE__CONTAINER__SERVICE_UNINITIALIZED,
    IPEE_ERROR_CODE__CONTAINER__SERVICE_ALREADY_INITIALIZED,
    IPEE_ERROR_CODE__CONTAINER__NOT_EXISTS,

    /** Event. */
    IPEE_ERROR_CODE__EVENT__SERVICE_UNINITIALIZED,
    IPEE_ERROR_CODE__EVENT__CONTEXT_NOT_EXISTS,
    IPEE_ERROR_CODE__EVENT__NOT_EXISTS,
    IPEE_ERROR_CODE__EVENT__INVALID_CALLBACK,
    IPEE_ERROR_CODE__EVENT__NAME_CREATION_ERROR,

    /** Hashmap. */
    IPEE_ERROR_CODE__HASHMAP__NOT_EXISTS,
    IPEE_ERROR_CODE__HASHMAP__ALLOCATION_ERROR,

    /** ThreadPool. */
    IPEE_ERROR_CODE__THREADPOOL__SERVICE_UNINITIALIZED,
    IPEE_ERROR_CODE__THREADPOOL__SERVICE_ALREADY_INITIALIZED,
    IPEE_ERROR_CODE__THREADPOOL__THREAD_CREATION_ERROR,
    IPEE_ERROR_CODE__THREADPOOL__INVALID_THREAD,
    IPEE_ERROR_CODE__THREADPOOL__INVALID_TASK,
} ipee_error_code_t, *p_ipee_error_code;

/**
 * @brief Defines API sets have assigned IDs.
 */
typedef enum ipee_api_e {
    /** Unspecified API */
    IPEE_API_UNSPECIFIED    = 0,

    /** Bitmap API */
    IPEE_API_BITMAP         = 1,

    /** Container API */
    IPEE_API_CONTAINER      = 2,

    /** Dictionary API */
    IPEE_API_DICTIONARY     = 3,

    /** Event API */
    IPEE_API_EVENT          = 4,

    /** Logger API */
    IPEE_API_LOGGER         = 5,

    /** Threadpool API */
    IPEE_API_THREADPOOL     = 6,

    /** Total number of APIs */
    IPEE_API_MAX
} ipee_api_t, *p_ipee_api;

#endif // IPEE_MACRO_H