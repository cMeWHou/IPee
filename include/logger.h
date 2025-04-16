/*********************************************************************************************
 * @file logger.h
 * @author Bulat Ramazanov (ramzesbulman@gmail.com)
 * @brief Logging functionality
 * @version 1.0
 * @date 2025-04-12
 * 
 * @copyright Copyright (c) 2025
 ********************************************************************************************/

#ifndef IPEE_LOGGER_H
#define IPEE_LOGGER_H

/*********************************************************************************************
 * STRUCTS DECLARATIONS
 ********************************************************************************************/

/**
 * @brief Defines log level
 */
typedef enum log_level_e {
    /** Log Level Debug */
    LEVEL_DEBUG             = 0,

    /** Log Level Info */
    LEVEL_INFO              = 1,

    /** Log Level Notice */
    LEVEL_NOTICE            = 1,

    /** Log Level Warning */
    LEVEL_WARNING           = 2,

    /** Log Level Error */
    LEVEL_ERROR             = 3,

    /** Log Level Critical*/
    LEVEL_CRITICAL          = 4,

    /** Total number of log level */
    LEVEL_MAX
} log_level_t, *p_log_level;

/*********************************************************************************************
 * FUNCTIONS DECLARATIONS
 ********************************************************************************************/

extern void init_logging(const char* config_file, const char* filename);
extern void close_logging(void);
extern void set_log_level(log_level_t level);
extern void internal_log(log_level_t level, const char *f, unsigned l, const char *format, ...);

/*********************************************************************************************
 * MACROS DECLARATIONS
 ********************************************************************************************/

#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)

#ifndef EE_LOG_OFF

#define INT_LOG(level, __fmt, ...)                                          \
    internal_log(level, __FUNCTION__, __LINE__, __fmt __VA_OPT__(,) __VA_ARGS__);

#else // EE_LOG_OFF
#define INT_LOG(...)
#define LOG(...)
#endif

#define LOG(level, __fmt, __args...)                                        \
    INT_LOG(level, __fmt, ##__args)

#define EE_ENTER()                                                          \
    INT_LOG(LEVEL_INFO, " ENTER")

#define EE_EXIT()                                                           \
    INT_LOG(LEVEL_INFO, " EXIT")

#define EE_LOG_RV_ERROR(prevend, rv) INT_LOG(LEVEL_ERROR, "%s, failed with error %d.\n", prevend, rv)

#define EE_RV_CHK(prevend, rv)                                              \
    if (unlikely(0 != rv)) {                                                \
        EE_LOG_RV_ERROR(prevend, rv);                                       \
        return rv;                                                          \
    }

#define EE_RV_LVL_CHK(level, prevend, rv)                                   \
    if (unlikely(0 != rv)) {                                                \
        INT_LOG(level, "%s, failed with error %d.\n", prevend, rv);         \
        return rv;                                                          \
    }

#endif // IPEE_LOGGER_H