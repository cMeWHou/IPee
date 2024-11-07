#include <container.h>

#include <stdlib.h>
#include <pthread.h>

#include <dictionary.h>
#include <macro.h>

/***********************************************************************************************
 * STRUCTS DECLARATIONS
 **********************************************************************************************/

/**
 * @brief Application container.
 */
typedef struct application_container_s {
    const char *name;                       // Container name.
    p_dictionary services;                  // Services names.
    p_dictionary elements_types;            // Elements types.
    p_dictionary elements_initial_callback; // Elements initial callback.
    p_dictionary elements_release_callback; // Elements release callback.
    p_dictionary elements_args;             // Elements args.
    p_dictionary elements_refs;             // Counts of refs to elements.
    pthread_mutex_t mutex;                  // Mutex.
} application_container_t, *p_application_container;

/***********************************************************************************************
 * STATIC FUNCTIONS DECLARATIONS
 **********************************************************************************************/

/**
 * @brief Release service.
 * 
 * @param key Service key identifier.
 * @param callback Release callback function. 
 */
static void release_service(char *key, container_callback_function callback, p_application_container container);

/*********************************************************************************************
 * STATIC VARIABLES
 ********************************************************************************************/

/**
 * @brief Application container instance.
 * 
 * @details
 * Do not for directly access.
 */
static p_dictionary containers = NULL;

/**
 * @brief Maximum transient references.
 * 
 * @details
 * Maximum number of transient references.
 */
static int MAX_TRANSIENT_REFS = 3;

/**
 * @brief Maximum container size.
 * 
 * @details
 * Maximum number of services in container.
 */
static int MAX_CONTAINER_SIZE = 255;

/*********************************************************************************************
 * FUNCTIONS DEFINITIONS
 ********************************************************************************************/

void set_max_service_count(int count) {
    MAX_CONTAINER_SIZE = count;
}

void set_max_transient_service_count(int count) {
    MAX_TRANSIENT_REFS = count;
}

p_dictionary get_all_containers(void) {
    return containers;
}

p_container get_container(const char *name) {
    if (!containers || !contains_key_in_dictionary(containers, name))
        return NULL;

    return (p_container)get_value_from_dictionary(containers, name);
}

p_container init_container(const char *name) {
    if (!containers)
        containers = create_dictionary();
    else if (contains_key_in_dictionary(containers, name))
        return get_container(name);

    p_application_container app_container = (p_application_container)malloc(sizeof(application_container_t));
    if (!app_container) return NULL;

    app_container->name = name;
    app_container->services = create_dictionary();
    app_container->elements_types = create_dictionary();
    app_container->elements_initial_callback = create_dictionary();
    app_container->elements_release_callback = create_dictionary();
    app_container->elements_args = create_dictionary();
    app_container->elements_refs = create_dictionary();
    pthread_mutex_init(&app_container->mutex, NULL);
    add_record_to_dictionary(containers, name, app_container);

    return (p_container)app_container;
}

void release_container(p_container container) {
    if (!container || !contains_key_in_dictionary(containers, container->name))
        return;

    p_application_container app_container = (p_application_container)container;
    pthread_mutex_lock(&app_container->mutex);
    if (app_container) {
        iterate_over_dictionary_with_args(app_container->elements_release_callback, (void *)release_service, app_container);
        delete_dictionary(app_container->services);
        delete_dictionary(app_container->elements_types);
        delete_dictionary(app_container->elements_initial_callback);
        delete_dictionary(app_container->elements_release_callback);
        delete_dictionary(app_container->elements_args);
        delete_dictionary(app_container->elements_refs);
    }
    pthread_mutex_unlock(&app_container->mutex);
    pthread_mutex_destroy(&app_container->mutex);
    remove_record_from_dictionary(containers, container->name);
    
    free(container);
    if (containers->size == 0)
        delete_dictionary(containers);
}

void release_container_by_name(const char *name) {
    if (!contains_key_in_dictionary(containers, name))
        return;

    p_container container = (p_container)get_value_from_dictionary(containers, name);
    release_container(container);
}

void release_all_containers(void) {
    iterate_over_dictionary_values(containers, (void *)release_container);
}

void add_glblvalue_to_global_container(char *key, void *value, container_callback_function release_callback) {
    p_container container = (p_container)get_value_from_dictionary(containers, name_of(global));
    if (!container)
        container = init_container(name_of(global));
    add_service_to_container(container, SERVICE_TYPE_GLBLVALUE, key, NULL, release_callback, value);
}

void add_glblvalue_to_container(p_container container, char *key, void *value, container_callback_function release_callback) {
    add_service_to_container(container, SERVICE_TYPE_GLBLVALUE, key, NULL, release_callback, value);
}

void add_glblvalue_to_container_by_name(const char *name, char *key, void *value, container_callback_function release_callback) {
    p_container container = (p_container)get_value_from_dictionary(containers, name);
    add_service_to_container(container, SERVICE_TYPE_GLBLVALUE, key, NULL, release_callback, value);
}

void add_singleton_to_global_container(char *key, container_callback_function initial_callback, container_callback_function release_callback) {
    if (!containers)
        containers = create_dictionary();

    p_container container = (p_container)get_value_from_dictionary(containers, name_of(global));
    if (!container)
        container = init_container(name_of(global));
    add_service_to_container(container, SERVICE_TYPE_SINGLETON, key, initial_callback, release_callback, NULL);
}

void add_singleton_to_container(p_container container, char *key, container_callback_function initial_callback, container_callback_function release_callback) {
    add_service_to_container(container, SERVICE_TYPE_SINGLETON, key, initial_callback, release_callback, NULL);
}

void add_singleton_to_container_by_name(const char *name, char *key, container_callback_function initial_callback, container_callback_function release_callback) {
    p_container container = (p_container)get_value_from_dictionary(containers, name);
    add_service_to_container(container, SERVICE_TYPE_SINGLETON, key, initial_callback, release_callback, NULL);
}

void add_transient_to_global_container(char *key, container_callback_function initial_callback, container_callback_function release_callback) {
    if (!containers)
        containers = create_dictionary();
    
    p_container container = (p_container)get_value_from_dictionary(containers, name_of(global));
    if (!container)
        container = init_container(name_of(global));
    add_service_to_container(container, SERVICE_TYPE_TRANSIENT, key, initial_callback, release_callback, NULL);
}

void add_transient_to_container(p_container container, char *key, container_callback_function initial_callback, container_callback_function release_callback) {
    add_service_to_container(container, SERVICE_TYPE_TRANSIENT, key, initial_callback, release_callback, NULL);
}

void add_transient_to_container_by_name(const char *name, char *key, container_callback_function initial_callback, container_callback_function release_callback) {
    if (!containers)
        containers = create_dictionary();
    
    p_container container = (p_container)get_value_from_dictionary(containers, name);
    add_service_to_container(container, SERVICE_TYPE_TRANSIENT, key, initial_callback, release_callback, NULL);
}

void add_service_to_global_container(const service_type_t type, char *key,
                              container_callback_function initial_callback,
                              container_callback_function release_callback,
                              void *args) {
    p_container container = (p_container)get_value_from_dictionary(containers, name_of(global));
    if (!container)
        container = init_container(name_of(global));
    add_service_to_container(container, type, key, initial_callback, release_callback, args);
}

void add_service_to_container(p_container container, const service_type_t type, char *key,
                              container_callback_function initial_callback,
                              container_callback_function release_callback,
                              void *args) {
    if (!container || container->services->size >= MAX_CONTAINER_SIZE)
        return;

    p_application_container app_container = (p_application_container)container;
    pthread_mutex_lock(&app_container->mutex);
    if (app_container) {
        container_callback_function callback = get_value_from_dictionary(app_container->elements_release_callback, key);
        if (callback)
            release_service(key, callback, container);

        switch (type) {
        case SERVICE_TYPE_SINGLETON:
            add_record_to_dictionary(app_container->services, app_container->services->size, key);
            add_record_to_dictionary(app_container->elements_types, key, (void *)SERVICE_TYPE_SINGLETON);
            add_record_to_dictionary(app_container->elements_initial_callback, key, initial_callback);
            add_record_to_dictionary(app_container->elements_release_callback, key, release_callback);
            add_record_to_dictionary(app_container->elements_refs, key, NULL);
            add_record_to_dictionary(app_container->elements_args, key, args);
            break;

        case SERVICE_TYPE_TRANSIENT:
            add_record_to_dictionary(app_container->services, app_container->services->size, key);
            add_record_to_dictionary(app_container->elements_types, key, (void *)SERVICE_TYPE_TRANSIENT);
            add_record_to_dictionary(app_container->elements_initial_callback, key, initial_callback);
            add_record_to_dictionary(app_container->elements_release_callback, key, release_callback);
            add_record_to_dictionary(app_container->elements_refs, key, create_dictionary());
            add_record_to_dictionary(app_container->elements_args, key, args);
            break;

        case SERVICE_TYPE_GLBLVALUE:
            add_record_to_dictionary(app_container->services, app_container->services->size, key);
            add_record_to_dictionary(app_container->elements_types, key, (void *)SERVICE_TYPE_GLBLVALUE);
            add_record_to_dictionary(app_container->elements_initial_callback, key, NULL);
            add_record_to_dictionary(app_container->elements_release_callback, key, release_callback);
            add_record_to_dictionary(app_container->elements_args, key, args);
            break;

        default:
            exit(-1);
            break;
        }
    }
    pthread_mutex_unlock(&app_container->mutex);
}

void add_service_to_container_by_name(const char *name, const service_type_t type, char *key,
                              container_callback_function initial_callback,
                              container_callback_function release_callback,
                              void *args) {
    p_container container = (p_container)get_value_from_dictionary(containers, name);
    add_service_to_container(container, type, key, initial_callback, release_callback, args);
}

void *get_service_from_global_container(char *key) {
    p_container container = (p_container)get_value_from_dictionary(containers, name_of(global));
    if (!container)
        return NULL;

    return get_service_from_container(container, key);
}

void *get_service_from_container(p_container container, char *key) {
    return get_service_from_container_with_args(container, key, NULL);
}

void *get_service_from_container_with_args(p_container container, char *key, void *tmp_args) {
    p_application_container app_container = (p_application_container)container;

    service_type_t type = (service_type_t)get_value_from_dictionary(app_container->elements_types, key);
    switch (type) {
    case SERVICE_TYPE_SINGLETON:
        void *instance = get_value_from_dictionary(app_container->elements_refs, key);
        if (!instance) {
            container_callback_function initial_callback = get_value_from_dictionary(app_container->elements_initial_callback, key);
            void *args = get_value_from_dictionary(app_container->elements_args, key);
            if (tmp_args)
                args = tmp_args;

            instance = initial_callback(args);
            update_record_in_dictionary(app_container->elements_refs, key, instance);
        }

        return instance;

    case SERVICE_TYPE_TRANSIENT:
        void *args = get_value_from_dictionary(app_container->elements_args, key);
        if (tmp_args)
            args = tmp_args;

        p_dictionary refs = get_value_from_dictionary(app_container->elements_refs, key);
        if (refs->size >= MAX_TRANSIENT_REFS) {
            pthread_mutex_lock(&app_container->mutex);
            if (refs->size) {
                void *head_value = get_head_value_from_dictionary(refs);
                container_callback_function release_callback = get_value_from_dictionary(app_container->elements_release_callback, key);
                release_callback(head_value);
                remove_record_from_dictionary_by_index(refs, 0);
            }
            pthread_mutex_unlock(&app_container->mutex);
        }

        container_callback_function initial_callback = get_value_from_dictionary(app_container->elements_initial_callback, key);
        add_record_to_dictionary(refs, key, initial_callback(args));
        return get_tail_value_from_dictionary(refs);

    case SERVICE_TYPE_GLBLVALUE:
        return get_value_from_dictionary(app_container->elements_args, key);

    default:
        break;
    }
}

void *get_service_from_container_by_name(const char *name, char *key) {
    p_container container = (p_container)get_value_from_dictionary(containers, name);
    return get_service_from_container(container, key);
}

/***********************************************************************************************
 * STATIC FUNCTIONS DEFINITIONS
 **********************************************************************************************/

static void release_service(char *key, container_callback_function callback, p_application_container container) {
    service_type_t type = (service_type_t)get_value_from_dictionary(container->elements_types, key);
    switch (type) {
    case SERVICE_TYPE_SINGLETON:
        void *instance = get_value_from_dictionary(container->elements_refs, key);
        if (instance && callback)
            callback(instance);
        break;

    case SERVICE_TYPE_TRANSIENT:
        p_dictionary refs = get_value_from_dictionary(container->elements_refs, key);
        if (refs && callback) {
            iterate_over_dictionary_values(refs, (void *)callback);
        }
        delete_dictionary(refs);
        break;

    case SERVICE_TYPE_GLBLVALUE:
        const char args = get_value_from_dictionary(container->elements_args, key);
        if (callback) {
            callback(args);
        }
        break;

    default:
        break;
    }
}