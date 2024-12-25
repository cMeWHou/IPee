/**
 * @file event.h
 * @author chcp (cmewhou@yandex.ru)
 * @brief Common event system.
 * @version 1.1
 * @date 2024-11-19
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
 * @brief Get context list.
 *
 * @return Context list.
 */
extern p_dictionary get_context_list(void);

/**
 * @brief Get context events.
 *
 * @param context Context name.
 * @return Context events.
 */
extern p_dictionary get_context_events(const char *context);

/**
 * @brief Get event subscribers.
 *
 * @param context Context name.
 * @param event Event name.
 * @return Context event subscribers.
 */
extern p_dictionary get_context_event_subscribers(const char *context,
                                                  const char *event);

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
extern void subscribe(const char *context, const char *event,
                      observable_callback callback);

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
                                       observable_callback_with_args callback,
                                       void *args);

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
extern void subscribe_with_args(const char *context, const char *event,
                                observable_callback_with_args callback,
                                void *args);

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
extern void unsubscribe(const char *context, const char *event,
                        observable_callback callback);

/**
 * @brief Unsubscribe from event.
 *
 * @details
 * Unsubscribes from event in the context.
 *
 * @param context Context to unsubscribe from.
 * @param event Event to unsubscribe from.
 */
extern void unsubscribe_from_event(const char *context, const char *event);

/**
 * @brief Unsubscribe from context.
 *
 * @details
 * Unsubscribes from context.
 *
 * @param context Context to unsubscribe from.
 */
extern void unsubscribe_from_context(const char *context);

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
 * @brief Prepare event name.
 *
 * @details
 * Prepares event name.
 *
 * @param context Context to prepare event name for.
 * @param event Event to prepare event name for.
 * @param uniq_id Unique id to prepare event name for.
 *
 * @return Event name.
 */
char *prepare_event_name(const char *context, const char *event,
                         const int uniq_id);

#endif // IPEE_EVENT_H