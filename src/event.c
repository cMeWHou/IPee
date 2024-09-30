#include <event.h>

#include <stdlib.h>

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

void global_subscribe(const char *event_name, observable_callback callback) {
    subscribe(name_of(global), event_name, callback);
}

void subscribe(const char *context, const char *event_name, observable_callback callback) {
    subscribe_with_args(context, event_name, callback, NULL);
}

void global_subscribe_with_args(const char *event_name, observable_callback_with_args callback, void *args) {
    subscribe_with_args(name_of(global), event_name, callback, args);
}

void subscribe_with_args(const char *context, const char *event_name,
                         observable_callback_with_args callback, void *args) {
    if (!events)
        events = create_dictionary();

    p_dictionary event_context = (p_dictionary)get_value_from_dictionary(events, context);
    if (!event_context)
        event_context = init_event_context(context);

    p_dictionary event = (p_dictionary)get_value_from_dictionary(event_context, event_name);
    if (!event)
        event = init_event(event_context, event_name);

    add_record_to_dictionary_with_metadata(event, event->size, callback, args);
}

void global_unsubscribe(const char *event_name, observable_callback callback) {
    unsubscribe(name_of(global), event_name, callback);
}

void unsubscribe(const char *context, const char *event_name, observable_callback callback) {
    p_dictionary event_context = (p_dictionary)get_value_from_dictionary(events, context);
    if (!event_context)
        return;

    p_dictionary event = (p_dictionary)get_value_from_dictionary(event_context, event_name);
    if (!event)
        return;

    p_record record = (p_record)get_record_from_dictionary_by_value(event, callback);
    if (!record)
        return;
    
    remove_record_from_dictionary_by_index(event, record->key);
    if (!event->size) {
        delete_dictionary(event);
        remove_record_from_dictionary(event_context, event_name);
    }

    if (!event_context->size) {
        delete_dictionary(event_context);
        remove_record_from_dictionary(events, context);
    }

    if (!events->size)
        delete_dictionary(events);
}

void global_notify(const char *event, void *args) {
    notify(name_of(global), event, args);
}

void notify(const char *context_name, const char *event_name, void *args) {
    p_dictionary context = (p_dictionary)get_value_from_dictionary(events, context_name);
    p_dictionary event = (p_dictionary)get_value_from_dictionary(context, event_name);

    iterate_over_dictionary_records_with_args(event, callback_invoker, args);
}

p_dictionary get_subscriptions(void) {
    return events;
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
