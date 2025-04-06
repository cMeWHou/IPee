#include <event.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dictionary.h>
#include <macro.h>

/***********************************************************************************************
 * STATIC FUNCTIONS DECLARATIONS
 **********************************************************************************************/

/**
 * @brief Initialize event context.
 *
 * @details
 * Create new context. Context is a container for events.
 *
 * @param context Context name.
 * @return Context.
 */
static p_dictionary init_event_context(const char *context);

/**
 * @brief Initialize event.
 *
 * @details
 * Create new event.
 *
 * @param event_context Context name.
 * @param name Event name.
 * @return Event.
 */
static p_dictionary init_event(p_dictionary event_context, const char *name);

/**
 * @brief Callback invoker.
 *
 * @details
 * Invokes callback function.
 *
 * @param record Callback.
 * @param args Arguments.
 */
static void callback_invoker(p_record record, void *args);

/**
 * @brief Get number length.
 *
 * @details
 * Get number length.
 *
 * @param number Number.
 * @return Number length.
 */
static int get_number_length(int number);

/*********************************************************************************************
 * STATIC VARIABLES
 ********************************************************************************************/

/**
 * @brief Events dictionary.
 *
 * @details
 * Events dictionary is a dictionary of events.
 */
static p_dictionary events = NULL;

/*********************************************************************************************
 * FUNCTIONS DEFINITIONS
 ********************************************************************************************/

p_dictionary get_context_list(void) { 
    return events;
}

p_dictionary get_context_events(const char *context_name) {
    if (!events)
        return NULL;

    return get_value_from_dictionary(events, context_name);
}

p_dictionary get_context_event_subscribers(const char *context_name, const char *event_name) {
    if (!events)
        return NULL;
    p_dictionary context =
        (p_dictionary)get_value_from_dictionary(events, context_name);

    if (!context)
        return NULL;
    return get_value_from_dictionary(context, event_name);
}

void global_subscribe(const char *event_name, observable_callback callback) {
    global_subscribe_with_args(event_name, callback, NULL);
}

void global_subscribe_with_args(
    const char *event_name, observable_callback_with_args callback, void *args) {
    subscribe_with_args(name_of(global), event_name, callback, args);
}

void subscribe(const char *context, const char *event_name, observable_callback callback) {
    subscribe_with_args(context, event_name, callback, NULL);
}

void subscribe_with_args(
    const char *context_name, const char *event_name, observable_callback_with_args callback, void *args) {
    if (!events)
        events = create_dictionary();

    p_dictionary context = (p_dictionary)get_value_from_dictionary(events, context_name);
    if (!context)
        context = init_event_context(context_name);

    p_dictionary event = (p_dictionary)get_value_from_dictionary(context, event_name);
    if (!event)
        event = init_event(context, event_name);

    char *key = prepare_event_name(context_name, event_name, event->size);
    add_record_to_dictionary_with_metadata(event, key, callback, args);
}

void global_unsubscribe(const char *event_name, observable_callback callback) {
    unsubscribe(name_of(global), event_name, callback);
}

void unsubscribe(const char *context_name, const char *event_name, observable_callback callback) {
    if (!events)
        exit(IPEE_ERROR_CODE__EVENT__SERVICE_UNINITIALIZED);

    p_dictionary context = (p_dictionary)get_value_from_dictionary(events, context_name);
    if (!context)
        exit(IPEE_ERROR_CODE__EVENT__CONTEXT_NOT_EXISTS);

    p_dictionary event = (p_dictionary)get_value_from_dictionary(context, event_name);
    if (!event)
        exit(IPEE_ERROR_CODE__EVENT__NOT_EXISTS);

    p_record record = (p_record)get_record_from_dictionary_by_value(event, callback);
    if (!record)
        exit(IPEE_ERROR_CODE__EVENT__INVALID_CALLBACK);

    char *key = record->key;
    remove_record_from_dictionary(event, record->key);
    free(key);

    if (!event->size) {
        remove_record_from_dictionary(context, event_name);
        delete_dictionary(event);
    }

    if (!context->size) {
        remove_record_from_dictionary(events, context_name);
        delete_dictionary(context);
    }

    if (!events->size) {
        delete_dictionary(events);
        events = NULL;
    }
}

void unsubscribe_from_event(const char *context, const char *event_name) {
    p_dictionary subscribers = get_context_event_subscribers(context, event_name);
    p_record current = subscribers ? subscribers->head : NULL;
    while (current) {
        p_record next = current->next;
        unsubscribe(context, event_name, current->value);
        current = next;
    }
}

void unsubscribe_from_context(const char *context) {
    p_dictionary context_events = get_context_events(context);
    p_record current = context_events ? context_events->head : NULL;
    while (current) {
        p_record next = current->next;
        unsubscribe_from_event(context, current->key);
        current = next;
    }
}

void global_notify(const char *event, void *args) {
    notify(name_of(global), event, args);
}

void notify(const char *context_name, const char *event_name, void *args) {
    if (!events)
        exit(IPEE_ERROR_CODE__EVENT__SERVICE_UNINITIALIZED);

    p_dictionary context = (p_dictionary)get_value_from_dictionary(events, context_name);
    if (!context)
        exit(IPEE_ERROR_CODE__EVENT__CONTEXT_NOT_EXISTS);

    p_dictionary event = (p_dictionary)get_value_from_dictionary(context, event_name);
    if (!event)
        exit(IPEE_ERROR_CODE__EVENT__NOT_EXISTS);

    iterate_over_dictionary_records_with_args(event, callback_invoker, args);
}

char *prepare_event_name(const char *context, const char *event, const int id) {
    const int id_length = get_number_length(id);

    const char *event_name = malloc(strlen(context) + 1 + strlen(event) + 1 + id_length + 1);
    if (!event_name)
        exit(IPEE_ERROR_CODE__EVENT__NAME_CREATION_ERROR);

    sprintf(event_name, "%s_%s_%d", context, event, id);

    return event_name;
}

/***********************************************************************************************
 * STATIC FUNCTIONS DEFINITIONS
 **********************************************************************************************/

static p_dictionary init_event_context(const char *context) {
    if (!events)
        events = create_dictionary();
    else if (contains_key_in_dictionary(events, context))
        return get_value_from_dictionary(events, context);

    p_dictionary event_context = create_dictionary();
    add_record_to_dictionary(events, context, event_context);

    return event_context;
}

static p_dictionary init_event(p_dictionary event_context, const char *name) {
    if (!event_context)
        return NULL;

    p_dictionary event = create_dictionary();
    add_record_to_dictionary(event_context, name, event);

    return event;
}

static void callback_invoker(p_record record, void *args) {
    const void *key = record->key;
    const void *value = record->value;
    const void *captured_args = record->metadata;

    observable_callback_with_args callback = value;
    callback(args, captured_args);
}

static int get_number_length(int number) {
    int length = 1;
    while (number > 9)
        number /= 10, length++;
    return length;
}