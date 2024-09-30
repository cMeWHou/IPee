/**
 * @file event.h
 * @author chcp (cmewhou@yandex.ru)
 * @brief Common event system.
 * @version 1.0
 * @date 2024-07-23
 *
 * @copyright Copyright (c) 2024
 */

#ifndef IPEE_EVENT_H
#define IPEE_EVENT_H

#include <dictionary.h>

/***********************************************************************************************
 * FUNCTION TYPEDEFS
 **********************************************************************************************/

/**
 * @brief Callback function for initializing or releasing services.
 *
 * @param args Arguments for callback function.
 */
typedef void *(*observable_callback)(void *args);

/**
 * @brief Callback function for initializing or releasing services.
 *
 * @param args Arguments for callback function.
 * @param captured_args Captured arguments for callback function.
 */
typedef void *(*observable_callback_with_args)(void *args, void *captured_args);

/*********************************************************************************************
 * FUNCTIONS DECLARATIONS
 ********************************************************************************************/

/**
 * @brief Subscribe to event globally.
 * 
 * @details
 * Subscribes to event globally.
 * 
 * @param event Event to subscribe to.
 * @param callback Callback function.
 */
extern void global_subscribe(const char *event, observable_callback callback);

/**
 * @brief Subscribe to event.
 * 
 * @details
 * Subscribes to event in the context.
 * 
 * @param context Context to subscribe to.
 * @param event Event to subscribe to.
 * @param callback Callback function.
 */
extern void subscribe(const char *context, const char *event, observable_callback callback);

/**
 * @brief Subscribe to event globally.
 * 
 * @details
 * Subscribes to event globally.
 * 
 * @param event Event to subscribe to.
 * @param callback Callback function.
 * @param args Captured arguments for callback function.
 */
extern void global_subscribe_with_args(const char *event,
                                       observable_callback_with_args callback, void *args);

/**
 * @brief Subscribe to event.
 * 
 * @details
 * Subscribes to event in the context.
 * 
 * @param context Context to subscribe to.
 * @param event Event to subscribe to.
 * @param callback Callback function.
 * @param args Captured arguments for callback function.
 */
extern void subscribe_with_args(const char *context, const char *event_name,
                                observable_callback_with_args callback, void *args);

/**
 * @brief Unsubscribe from event globally.
 * 
 * @details
 * Unsubscribes from event globally.
 * 
 * @param event Event to unsubscribe from.
 */
extern void global_unsubscribe(const char *event, observable_callback callback);

/**
 * @brief Unsubscribe from event.
 * 
 * @details
 * Unsubscribes from event in the context.
 * 
 * @param context Context to unsubscribe from.
 * @param event Event to unsubscribe from.
 */
extern void unsubscribe(const char *context, const char *event, observable_callback callback);

/**
 * @brief Unsubscribe from event globally.
 * 
 * @details
 * Unsubscribes from event globally.
 * 
 * @param event Event to unsubscribe from.
 */
extern void global_notify(const char *event, void *args);

/**
 * @brief Unsubscribe from event.
 * 
 * @details
 * Unsubscribes from event in the context.
 * 
 * @param context Context to unsubscribe from.
 * @param event Event to unsubscribe from.
 */
extern void notify(const char *context, const char *event, void *args);

/**
 * @brief Get the subscriptions.
 * 
 * @details
 * Get subscription list.
 * 
 * @return Subscriptions.
 */
extern p_dictionary get_subscriptions(void);

#endif // IPEE_EVENT_H