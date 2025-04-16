/**
 * @file logger.test.c
 * @author Bulat Ramazanov (ramzesbulman@gmail.com)
 * @brief Logger tests
 * @version 1.0
 * @date 2025-04-12
 *
 * @copyright Copyright (c) 2025
 */

#include "utils/helper.h"

#include <logger.h>
#include <stddef.h>

/*********************************************************************************************
 * FUNCTIONS DECLARATIONS
 ********************************************************************************************/

int logger_getValidValue_OK(void);

/*********************************************************************************************
 * FUNCTIONS DEFINITIONS
 ********************************************************************************************/

int logger_test(int argc, char *argv[]) {
    int exit_result = 0;

    exit_result |= logger_getValidValue_OK();

    return exit_result;
}

int logger_getValidValue_OK(void) {
    init_logging(NULL, "app.log");

    EE_ENTER();

    int i = 0;

    LOG(LEVEL_INFO, "Application started %d", ++i);
    LOG(LEVEL_WARNING, "Warning message %d", ++i);
    LOG(LEVEL_ERROR, "Error message %d", ++i);
    LOG(LEVEL_CRITICAL, "Critical message %d", ++i);
    LOG(LEVEL_DEBUG, "Debug message %d", ++i);

    EE_EXIT();

    close_logging();

    const int result = 1;

    return ORDER_RESULT(result, 0);
}