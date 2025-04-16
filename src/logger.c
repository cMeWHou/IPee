/*********************************************************************************************
 * INCLUDES DECLARATIONS
 ********************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>

#include <logger.h>
#include <hashmap.h>
#include <macro.h>

#define MESSAGE_BUFFER_SIZE 1024

#ifndef EE_LOG_OFF

/***********************************************************************************************
 * STRUCTS DECLARATIONS
 **********************************************************************************************/

typedef struct ee_metadata_s {
    p_hashmap api;
    const char *const *const api_names;
    const unsigned *const api_values;
    p_hashmap log_level;
    const char *const *const log_level_names;
    const unsigned *const log_level_values;
    unsigned api_log_level[IPEE_API_MAX];
} ee_metadata_t, *p_ee_metadata;

typedef struct log_entry_s {
    char *message;
    struct log_entry_s *next;
} log_entry_t, *p_log_entry;

typedef struct log_file_s {
    FILE *log_file;
    char *log_file_name;
} log_file_t, *p_log_file;

/*********************************************************************************************
 * STATIC VARIABLES
 ********************************************************************************************/

static log_level_t global_log_level = LEVEL_DEBUG;
static p_log_entry log_queue_head = NULL;
static p_log_entry log_queue_tail = NULL;
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t queue_cond = PTHREAD_COND_INITIALIZER;
static pthread_t log_thread;
static int logging_active = 0;
static char message_buffer[MESSAGE_BUFFER_SIZE];

static const char *const ee_metadata_ipee_api_e_api_names[] = {
    "UNSPECIFIED",
    "BITMAP",
    "CONTAINER",
    "DICTIONARY",
    "EVENT",
    "LOGGER",
    "THREADPOOL",
    NULL
};

static const unsigned ee_metadata_ipee_api_e_api_values[] = {
    IPEE_API_UNSPECIFIED,
    IPEE_API_BITMAP,
    IPEE_API_CONTAINER,
    IPEE_API_DICTIONARY ,
    IPEE_API_EVENT,
    IPEE_API_LOGGER,
    IPEE_API_THREADPOOL,
    IPEE_API_MAX
};

static const char *const ee_metadata_ipee_api_e_log_level_names[] = {
    "DEBUG",
    "INFO",
    "NOTICE",
    "WARNING",
    "ERROR",
    "CRITICAL",
    NULL
};

static const unsigned ee_metadata_ipee_api_e_log_level_values[] = {
    LEVEL_DEBUG,
    LEVEL_INFO,
    LEVEL_NOTICE,
    LEVEL_WARNING,
    LEVEL_ERROR,
    LEVEL_CRITICAL,
    LEVEL_MAX
};

static ee_metadata_t ee_metadata_ipee_api_e = {
    .api_names = ee_metadata_ipee_api_e_api_names,
    .api_values = ee_metadata_ipee_api_e_api_values,
    .log_level_names = ee_metadata_ipee_api_e_log_level_names,
    .log_level_values = ee_metadata_ipee_api_e_log_level_values,
};

static log_file_t logger_file = {
    .log_file = NULL,
    .log_file_name = NULL,
};

/***********************************************************************************************
 * FUNCTIONS DEFINITIONS
 **********************************************************************************************/

void* logger_worker(void* arg) {
    while (1) {
        if (pthread_mutex_lock(&queue_mutex) != 0) {
            fprintf(stderr, "Error: Failed to acquire queue mutex\n");
            break;
        }

        while (!log_queue_head && logging_active) {
            if (pthread_cond_wait(&queue_cond, &queue_mutex) != 0) {
                fprintf(stderr, "Error: Failed to waiting condition signal\n");
                break;
            }
        }

        if (!logging_active && !log_queue_head) {
            if (pthread_mutex_unlock(&queue_mutex) != 0) {
                fprintf(stderr, "Error: Failed to release queue mutex\n");
            }
            break;
        }

        p_log_entry entry = log_queue_head;
        log_queue_head = entry->next;
        if (!log_queue_head) {
            log_queue_tail = NULL;
        }

        if (pthread_mutex_unlock(&queue_mutex) != 0) {
            fprintf(stderr, "Error: Failed to release queue mutex\n");
            break;
        }

        // Log the message
        if (pthread_mutex_lock(&log_mutex) != 0) {
            fprintf(stderr, "Error: Failed to acquire log mutex\n");
            break;
        }

        if (logger_file.log_file) {
            fprintf(logger_file.log_file, "%s", entry->message);
            if (fflush(logger_file.log_file) != 0) {
                fprintf(stderr, "Error: Failed to write data from log file\n");
                break;
            }
        }

        if (pthread_mutex_unlock(&log_mutex) != 0) {
            fprintf(stderr, "Error: Failed to release log mutex\n");
            break;
        }

        free(entry->message);
        free(entry);
    }

    return NULL;
}

void logger_init(const char *config_filehame, const char *log_filename) {
    if (pthread_mutex_lock(&queue_mutex) != 0) {
        fprintf(stderr, "Error: Failed to acquire queue mutex\n");
        return;
    }

    if (logging_active == 1) {
        fprintf(stderr, "Warning: Logger is already initialized\n");

        if (pthread_mutex_unlock(&queue_mutex) != 0) {
            fprintf(stderr, "Error: Failed to release queue mutex\n");
        }

        return;
    }

    ee_metadata_ipee_api_e.api = hashmap_create();
    if (!ee_metadata_ipee_api_e.api) {
        fprintf(stderr, "Error: Out of memory\n");

        if (pthread_mutex_unlock(&queue_mutex) != 0) {
            fprintf(stderr, "Error: Failed to release queue mutex\n");
        }

        exit(EXIT_FAILURE);
    }
    HASHMAP_INPUT_ITEMS(ee_metadata_ipee_api_e.api, ee_metadata_ipee_api_e.api_names, ee_metadata_ipee_api_e.api_values, IPEE_API_MAX);

    ee_metadata_ipee_api_e.log_level = hashmap_create();
    if (!ee_metadata_ipee_api_e.log_level) {
        fprintf(stderr, "Error: Out of memory\n");

        if (pthread_mutex_unlock(&queue_mutex) != 0) {
            fprintf(stderr, "Error: Failed to release queue mutex\n");
        }

        exit(EXIT_FAILURE);
    }
    HASHMAP_INPUT_ITEMS(ee_metadata_ipee_api_e.log_level, ee_metadata_ipee_api_e.log_level_names, ee_metadata_ipee_api_e.log_level_values, LEVEL_MAX);

    if (config_filehame) {
        FILE *config_file = fopen(config_filehame, "r");

        if (!config_file) {
            fprintf(stderr, "Failed to open config file: %s\n", config_filehame);

            if (pthread_mutex_unlock(&queue_mutex) != 0) {
                fprintf(stderr, "Error: Failed to release queue mutex\n");
            }

            exit(EXIT_FAILURE);
        }

        char line[32];
        memset(line, 0x0, 32);
        while (fgets(line, sizeof(line), config_file)) {
            unsigned *api_val = NULL;
            unsigned *level_val = NULL;
            char *api_str = strtok(line, "=");
            char *level_str = strtok(NULL, "=");
            level_str[strcspn(level_str, "\n")] = '\0'; // Remove newline

            if (((api_val = hashmap_get_entry(ee_metadata_ipee_api_e.api, api_str)) != NULL) &&
                ((level_val = hashmap_get_entry(ee_metadata_ipee_api_e.log_level, level_str)) != NULL)) {
                ee_metadata_ipee_api_e.api_log_level[*api_val] = *level_val;
            } else {
                fprintf(stderr, "Error: Incorrect argument from configuration file was received\n");

                if (pthread_mutex_unlock(&queue_mutex) != 0) {
                    fprintf(stderr, "Error: Failed to release queue mutex\n");
                }

                exit(EXIT_FAILURE);
            }
        }

        fclose(config_file);
    } else {
        for (unsigned api = IPEE_API_UNSPECIFIED; api < IPEE_API_MAX; api++) {
            ee_metadata_ipee_api_e.api_log_level[api] = global_log_level;
        }
    }

    if (pthread_mutex_lock(&log_mutex) != 0) {
        fprintf(stderr, "Error: Failed to acquire log mutex\n");
        exit(EXIT_FAILURE);
    }

    FILE *log_file;

    if (log_filename) {
        log_file = fopen(log_filename, "a");
        if (!log_file) {
            fprintf(stderr, "Error: Failed to open log file '%s'\n", log_filename);

            if (pthread_mutex_unlock(&queue_mutex) != 0) {
                fprintf(stderr, "Error: Failed to release queue mutex\n");
            }

            if (pthread_mutex_unlock(&log_mutex) != 0) {
                fprintf(stderr, "Error: Failed to release log mutex\n");
            }

            exit(EXIT_FAILURE);
        }
    } else {
        log_file = stdout;
    }

    logger_file.log_file = log_file;
    if (log_file != stdout) {
        logger_file.log_file_name = strdup(log_filename);
    }

    if (pthread_mutex_unlock(&log_mutex) != 0) {
        fprintf(stderr, "Error: Failed to release log mutex\n");
        exit(EXIT_FAILURE);
    }

    logging_active = 1;

    if (pthread_mutex_unlock(&queue_mutex) != 0) {
        fprintf(stderr, "Error: Failed to release queue mutex\n");
        exit(EXIT_FAILURE);
    }

    // Start the logging thread
    if (pthread_create(&log_thread, NULL, logger_worker, NULL) != 0) {
        fprintf(stderr, "Error: Failed to create log worker thread\n");
        exit(EXIT_FAILURE);
    }
}

void logger_close() {
    // Signal the logging thread to exit
    if (pthread_mutex_lock(&queue_mutex) != 0) {
        fprintf(stderr, "Error: Failed to acquire queue mutex\n");
        exit(EXIT_FAILURE);
    }

    if (logging_active == 0) {
        fprintf(stderr, "Warning: Logger is already closed\n");
        return;
    }

    logging_active = 0;

    // Remove hashmaps
    hashmap_remove(ee_metadata_ipee_api_e.api);
    hashmap_remove(ee_metadata_ipee_api_e.log_level);

    if (pthread_cond_signal(&queue_cond) != 0) {
        fprintf(stderr, "Error: Failed to send condition signal\n");
        exit(EXIT_FAILURE);
    }

    if (pthread_mutex_unlock(&queue_mutex) != 0) {
        fprintf(stderr, "Error: Failed to release queue mutex\n");
        exit(EXIT_FAILURE);
    }

    if (pthread_join(log_thread, NULL) != 0) {
        fprintf(stderr, "Error: Failed to join log worker thread\n");
        exit(EXIT_FAILURE);
    }

    if (pthread_mutex_lock(&log_mutex) != 0) {
        fprintf(stderr, "Error: Failed to acquire log mutex\n");
        exit(EXIT_FAILURE);
    }

    // Close log file
    if (logger_file.log_file) {
        if (logger_file.log_file != stdout) {
            if (fclose(logger_file.log_file) != 0) {
                fprintf(stderr, "Error: Failed to close log file\n");

                if (pthread_mutex_unlock(&log_mutex) != 0) {
                    fprintf(stderr, "Error: Failed to release log mutex\n");
                }

                exit(EXIT_FAILURE);
            }

            logger_file.log_file = NULL;

            free(logger_file.log_file_name);
            logger_file.log_file_name = NULL;
        } else {
            logger_file.log_file = NULL;
        }
    }

    if (pthread_mutex_unlock(&log_mutex) != 0) {
        fprintf(stderr, "Error: Failed to release log mutex\n");
        exit(EXIT_FAILURE);
    }
}

void logger_clear_data_from_log_file(void) {
    if (pthread_mutex_lock(&log_mutex) != 0) {
        fprintf(stderr, "Error: Failed to acquire log mutex\n");
        exit(EXIT_FAILURE);
    }

    if (logger_file.log_file && logger_file.log_file != stdout) {
        logger_file.log_file = freopen(logger_file.log_file_name, "w", logger_file.log_file);
    }

    if (pthread_mutex_unlock(&log_mutex) != 0) {
        fprintf(stderr, "Error: Failed to release log mutex\n");
        exit(EXIT_FAILURE);
    }
}

void logger_set_global_log_level(log_level_t level) {
    if (pthread_mutex_lock(&queue_mutex) != 0) {
        fprintf(stderr, "Error: Failed to acquire queue mutex\n");
        return;
    }

    global_log_level = level;

    if (pthread_mutex_unlock(&queue_mutex) != 0) {
        fprintf(stderr, "Error: Failed to release queue mutex\n");
    }
}

void logger_update_current_log_level(unsigned api, log_level_t level) {
    if (pthread_mutex_lock(&queue_mutex) != 0) {
        fprintf(stderr, "Error: Failed to acquire queue mutex\n");
        return;
    }

    ee_metadata_ipee_api_e.api_log_level[api] = level;

    if (pthread_mutex_unlock(&queue_mutex) != 0) {
        fprintf(stderr, "Error: Failed to release queue mutex\n");
    }
}

void logger_update_all_log_level(log_level_t level) {
    if (pthread_mutex_lock(&queue_mutex) != 0) {
        fprintf(stderr, "Error: Failed to acquire queue mutex\n");
        return;
    }

    for (unsigned api = IPEE_API_UNSPECIFIED; api < IPEE_API_MAX; api++) {
        ee_metadata_ipee_api_e.api_log_level[api] = level;
    }

    if (pthread_mutex_unlock(&queue_mutex) != 0) {
        fprintf(stderr, "Error: Failed to release queue mutex\n");
    }
}

void internal_log(unsigned api, log_level_t level, const char *f, unsigned l, const char *format, ...) {
    if (level < ee_metadata_ipee_api_e.api_log_level[api]) {
        return;
    }

    if (pthread_mutex_lock(&queue_mutex) != 0) {
        fprintf(stderr, "Error: Failed to acquire queue mutex\n");
        return;
    }

    if (!logger_file.log_file) {
        fprintf(stderr, "Error: Logging not initialized\n");

        if (pthread_mutex_unlock(&queue_mutex) != 0) {
            fprintf(stderr, "Error: Failed to release queue mutex\n");
        }

        return;
    }

    va_list args;

    va_start(args, format);
    int n = vsnprintf(NULL, 0, format, args);
    va_end(args);

    if (n < 0) return;
    ++n;

    char *p = malloc(n);
    if (p == NULL) return;

    va_start(args, format);
    n = vsnprintf(p, n, format, args);
    va_end(args);

    if (n >= 0) {
        p_log_entry entry = malloc(sizeof(log_entry_t));

        if (!entry) {
            fprintf(stderr, "Error: Failed to allocate memory for log entry\n");

            if (pthread_mutex_unlock(&queue_mutex) != 0) {
                fprintf(stderr, "Error: Failed to release queue mutex\n");
            }

            return;
        }

        memset(message_buffer, 0x0, MESSAGE_BUFFER_SIZE);

        struct timeval tp;
        gettimeofday(&tp, 0);
        time_t now = tp.tv_sec;
        struct tm* local_time = localtime(&now);

        sprintf(message_buffer, "%04d-%02d-%02d %02d:%02d:%02d.%ld [%s] %s:%s:%d %s\n",
                local_time->tm_year + 1900, local_time->tm_mon + 1,
                local_time->tm_mday, local_time->tm_hour, local_time->tm_min,
                local_time->tm_sec, tp.tv_usec, ee_metadata_ipee_api_e.log_level_names[level],
                ee_metadata_ipee_api_e.api_names[api], f, l, p);

        entry->message = strdup(message_buffer);
        entry->next = NULL;

        if (log_queue_tail) {
            log_queue_tail->next = entry;
        } else {
            log_queue_head = entry;
        }

        log_queue_tail = entry;

        if (pthread_cond_signal(&queue_cond) != 0) {
            fprintf(stderr, "Error: Failed to send condition signal\n");
        }
    }

    free(p);

    if (pthread_mutex_unlock(&queue_mutex) != 0) {
        fprintf(stderr, "Error: Failed to release queue mutex\n");
    }
}

#else // EE_LOG_OFF
[[noreturn]] void logger_init(const char*, const char*) { (void*)0; }
[[noreturn]] void logger_close(void) { { (void*)0; } }
[[noreturn]] void logger_clear_data_from_log_file(void) { (void*)0; }
[[noreturn]] void logger_set_global_log_level(log_level_t) { (void*)0; }
[[noreturn]] void logger_update_current_log_level(unsigned, log_level_t) { (void*)0; }
[[noreturn]] void logger_update_all_log_level(log_level_t) { (void*)0; }
[[noreturn]] void internal_log(unsigned, log_level_t, const char*, unsigned, const char*, ...) { (void*)0; }
#endif
