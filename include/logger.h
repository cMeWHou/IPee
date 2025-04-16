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
    LEVEL_NOTICE            = 2,

    /** Log Level Warning */
    LEVEL_WARNING           = 3,

    /** Log Level Error */
    LEVEL_ERROR             = 4,

    /** Log Level Critical*/
    LEVEL_CRITICAL          = 5,

    /** Total number of log level */
    LEVEL_MAX
} log_level_t, *p_log_level;

/*********************************************************************************************
 * FUNCTIONS DECLARATIONS
 ********************************************************************************************/

extern void logger_init(const char *config_file, const char *filename);
extern void logger_close(void);
extern void logger_clear_data_from_log_file(void);
extern void logger_set_global_log_level(log_level_t level);
extern void logger_update_current_log_level(unsigned api, log_level_t level);
extern void logger_update_all_log_level(log_level_t level);
extern void internal_log(unsigned api, log_level_t level, const char *f, unsigned l, const char *format, ...);

/*********************************************************************************************
 * MACROS DECLARATIONS
 ********************************************************************************************/

#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)

#ifndef EE_LOG_OFF

#define INT_LOG(api, level, __fmt, ...)                                                 \
    internal_log(api, level, __FUNCTION__, __LINE__, __fmt __VA_OPT__(,) __VA_ARGS__);

#define LOG(level, __fmt, ...)                                                          \
    INT_LOG(__MODULE__, level, __fmt, __VA_ARGS__)

#define EE_ENTER()                                                                      \
    INT_LOG(__MODULE__, LEVEL_INFO, "ENTER")

#define EE_EXIT()                                                                       \
    INT_LOG(__MODULE__, LEVEL_INFO, "EXIT")

#define EE_RV_CHK(prevend, rv)                                                          \
    if (unlikely(0 != rv)) {                                                            \
        INT_LOG(__MODULE__, LEVEL_ERROR, "%s, failed with error %d.\n", prevend, rv);   \
        return rv;                                                                      \
    }

#define EE_RV_LVL_CHK(level, prevend, rv)                                               \
    if (unlikely(0 != rv)) {                                                            \
        INT_LOG(__MODULE__, level, "%s, failed with error %d.\n", prevend, rv);         \
        return rv;                                                                      \
    }

#else // EE_LOG_OFF
#define INT_LOG(...)
#define LOG(...)
#define EE_ENTER()
#define EE_EXIT()
#define EE_RV_CHK(...)
#define EE_RV_LVL_CHK(...)
#endif

#endif // IPEE_LOGGER_H