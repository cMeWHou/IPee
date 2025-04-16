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
#include <macro.h>

#undef  __MODULE__
#define __MODULE__ IPEE_API_LOGGER

/***********************************************************************************************
 * STRUCTS DECLARATIONS
 **********************************************************************************************/

typedef struct ee_metadata_s {
    const char* const* const api_names;
    const unsigned long api_str_hash[IPEE_API_MAX];
    const unsigned long log_level_str_hash[LEVEL_MAX];
    log_level_t log_level[IPEE_API_MAX];
} ee_metadata_t, *p_ee_metadata;

/***********************************************************************************************
 * STATIC FUNCTIONS DECLARATIONS
 **********************************************************************************************/

static unsigned long hash(const char *str);

/*********************************************************************************************
 * STATIC VARIABLES
 ********************************************************************************************/

static FILE* log_file = NULL;
static log_level_t current_log_level = LEVEL_DEBUG;
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
static const char* level_strings[] = { "DEBUG", "INFO", "NOTICE", "WARNING", "ERROR", "CRITICAL" };

const char* const ee_metadata_ipee_api_e_names[] = {
    "UNSPECIFIED",
    "BITMAP",
    "CONTAINER",
    "DICTIONARY",
    "EVENT",
    "LOGGER",
    "THREADPOOL",
    NULL
};

static ee_metadata_t ee_metadata_ipee_api_e = {
    .api_names = ee_metadata_ipee_api_e_names,
};

/***********************************************************************************************
 * FUNCTIONS DEFINITIONS
 **********************************************************************************************/

// void load_config(const char* config_file) {
//     if (config_file != NULL) {
//         FILE* file = fopen(config_file, "r");
//         if (!file) {
//             fprintf(stderr, "Failed to open config file: %s\n", config_file);
//             return;
//         }

//         char line[128];
//         while (fgets(line, sizeof(line), file)) {
//             if (strncmp(line, "log_level=", 10) == 0) {
//                 char* level = line + 10;
//                 level[strcspn(level, "\n")] = '\0'; // Remove newline
//                 if (strcmp(level, "DEBUG") == 0) set_log_level(DEBUG);
//                 else if (strcmp(level, "INFO") == 0) set_log_level(INFO);
//                 else if (strcmp(level, "WARNING") == 0) set_log_level(WARNING);
//                 else if (strcmp(level, "ERROR") == 0) set_log_level(ERROR);
//             } else if (strncmp(line, "log_file=", 9) == 0) {
//                 char* filename = line + 9;
//                 filename[strcspn(filename, "\n")] = '\0'; // Remove newline
//                 init_logging(filename);
//             }
//         }
//         fclose(file);
//     } else {

//     }
// }

void init_logging(const char* config_filehame, const char* log_filename) {
    if (pthread_mutex_lock(&log_mutex) != 0) {
        fprintf(stderr, "Error: Failed to acquire log mutex\n");
        exit(EXIT_FAILURE);
    }

    if (config_filehame) {
        FILE* config_file = fopen(config_filehame, "r");

        if (!config_file) {
            fprintf(stderr, "Failed to open config file: %s\n", config_filehame);
            exit(EXIT_FAILURE);
        }

        {
            unsigned i = 0;
            while(i++ < IPEE_API_MAX)
                memset(&ee_metadata_ipee_api_e.api_str_hash[i], hash(ee_metadata_ipee_api_e.api_names[i]), sizeof(unsigned long));

            i = 0;
            while(i++ < LEVEL_MAX)
                memset(&ee_metadata_ipee_api_e.log_level_str_hash[i], hash(level_strings[i]), sizeof(unsigned long));
        }

        char line[128];
        while (fgets(line, sizeof(line), config_file)) {
            
        }
    } else {
        
    }

    if (log_filename) {
        log_file = fopen(log_filename, "a");
        if (!log_file) {
            fprintf(stderr, "Error: Failed to open log file '%s'\n", log_filename);

            if (pthread_mutex_unlock(&log_mutex) != 0) {
                fprintf(stderr, "Error: Failed to release log mutex\n");
            }

            exit(EXIT_FAILURE);
        }
    } else {
        log_file = stdout;
    }

    if (pthread_mutex_unlock(&log_mutex) != 0) {
        fprintf(stderr, "Error: Failed to release log mutex\n");
    }
}

void close_logging() {
    if (log_file && log_file != stdout) {
        fclose(log_file);
        log_file = NULL;
    }
}

void set_log_level(log_level_t level) {
    current_log_level = level;
}

void internal_log(log_level_t level, const char *f, unsigned l, const char *format, ...) {
    if (level < current_log_level) {
        return;
    }

    if (pthread_mutex_lock(&log_mutex) != 0) {
        fprintf(stderr, "Error: Failed to acquire log mutex\n");
        return;
    }

    if (!log_file) {
        fprintf(stderr, "Error: Logging not initialized\n");

        if (pthread_mutex_unlock(&log_mutex) != 0) {
            fprintf(stderr, "Error: Failed to release log mutex\n");
        }

        return;
    }

    va_list args;

    va_start(args, format);
    int n = vsnprintf(NULL, 0, format, args);
    va_end(args);

    if (n <= 0) return;
    ++n;

    char *p = malloc(n);
    if (p == NULL) return;

    va_start(args, format);
    n = vsnprintf(p, n, format, args);
    va_end(args);

    if (n > 0) {
        struct timeval tp;
        gettimeofday(&tp, 0);
        time_t now = tp.tv_sec;
        struct tm* local_time = localtime(&now);

        fprintf(log_file, "%04d-%02d-%02d %02d:%02d:%02d.%d [%s] %s:%d %s\n",
                local_time->tm_year + 1900, local_time->tm_mon + 1,
                local_time->tm_mday, local_time->tm_hour, local_time->tm_min,
                local_time->tm_sec, tp.tv_usec, level_strings[level], f, l, p);

        fflush(log_file);
    }

    free(p);

    if (pthread_mutex_unlock(&log_mutex) != 0) {
        fprintf(stderr, "Error: Failed to release log mutex\n");
    }
}

/***********************************************************************************************
 * STATIC FUNCTIONS DEFINITIONS
 **********************************************************************************************/

unsigned long hash(const char *str) {
    unsigned long hash = 5381;  
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash;
}
