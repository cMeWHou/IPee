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

    /** Container. */
    IPEE_ERROR_CODE__CONTAINER__SERVICE_UNINITIALIZED,
    IPEE_ERROR_CODE__CONTAINER__NOT_EXISTS,

    /** Event. */
    IPEE_ERROR_CODE__EVENT__SERVICE_UNINITIALIZED,
    IPEE_ERROR_CODE__EVENT__CONTEXT_NOT_EXISTS,
    IPEE_ERROR_CODE__EVENT__NOT_EXISTS,
    IPEE_ERROR_CODE__EVENT__INVALID_CALLBACK,
    IPEE_ERROR_CODE__EVENT__NAME_CREATION_ERROR,

    /** ThreadPool. */
    IPEE_ERROR_CODE__THREADPOOL__SERVICE_UNINITIALIZED,
    IPEE_ERROR_CODE__THREADPOOL__SERVICE_ALREADY_INITIALIZED,
    IPEE_ERROR_CODE__THREADPOOL__THREAD_CREATION_ERROR,
    IPEE_ERROR_CODE__THREADPOOL__INVALID_THREAD,
    IPEE_ERROR_CODE__THREADPOOL__INVALID_TASK,
} ipee_error_code_t, *p_ipee_error_code;

#endif // IPEE_MACRO_H