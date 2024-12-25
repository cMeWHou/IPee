/**
 * @file helper.c
 * @author chcp (cmewhou@yandex.ru)
 * @brief Helper functions.
 * @version 1.0
 * @date 2024-11-13
 *
 * @copyright Copyright (c) 2024
 */

#ifndef IPEE_TEST_HELPER_H
#define IPEE_TEST_HELPER_H

#define ORDER_RESULT(result, test_order_num) (!result) << test_order_num

/**
 * @brief Compare two strings.
 *
 * @param a First string.
 * @param b Second string.
 * @return 1 if equal, 0 else.
 */
extern int is_equal(const char *a, const char *b);

#endif // IPEE_TEST_HELPER_H