#include <container.h>

#include <pthread.h>
#include <stdlib.h>

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
 * @param container Application container.
 */
static void release_service(
    char *key, container_callback_function callback, p_application_container container);

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

int set_max_service_count(int count) {
    if (containers)
        return IPEE_ERROR_CODE__CONTAINER__SERVICE_ALREADY_INITIALIZED;

    MAX_CONTAINER_SIZE = count;
    return 0;
}

int set_max_transient_service_count(int count) {
    if (containers)
        return IPEE_ERROR_CODE__CONTAINER__SERVICE_ALREADY_INITIALIZED;

    MAX_TRANSIENT_REFS = count;
    return 0;
}

p_dictionary get_all_containers(void) {
    return containers;
}

p_container get_container(const char *name) {
    if (!containers)
        return NULL;

    if (!contains_key_in_dictionary(containers, name))
        return NULL;

    return (p_container)get_value_from_dictionary(containers, name);
}

p_container init_container(const char *name) {
    if (!containers)
        containers = create_dictionary();
    else if (contains_key_in_dictionary(containers, name))
        return get_container(name);

    p_application_container app_container =
        (p_application_container)malloc(sizeof(application_container_t));
    if (!app_container)
        return NULL;

    app_container->name = name;
    app_container->services = create_dictionary();
    app_container->elements_types = create_dictionary();
    app_container->elements_initial_callback = create_dictionary();
    app_container->elements_release_callback = create_dictionary();
    app_container->elements_args = create_dictionary();
    app_container->elements_refs = create_dictionary();

    pthread_mutexattr_t mutex_attr;
    pthread_mutexattr_init(&mutex_attr);
    pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&app_container->mutex, &mutex_attr);
    pthread_mutexattr_destroy(&mutex_attr);

    add_record_to_dictionary(containers, name, app_container);

    return (p_container)app_container;
}

int release_container(p_container container) {
    if (!containers)
        return IPEE_ERROR_CODE__CONTAINER__SERVICE_UNINITIALIZED;
    if (!container)
        return IPEE_ERROR_CODE__CONTAINER__NOT_EXISTS;

    if (!contains_key_in_dictionary(containers, container->name))
        return 0;

    p_application_container app_container = (p_application_container)container;
    pthread_mutex_lock(&app_container->mutex);
    iterate_over_dictionary_with_args(
        app_container->elements_release_callback,
        (dictionary_iteration_callback_with_args)release_service, app_container);
    delete_dictionary(app_container->services);
    app_container->services = NULL;
    delete_dictionary(app_container->elements_types);
    app_container->elements_types = NULL;
    delete_dictionary(app_container->elements_initial_callback);
    app_container->elements_initial_callback = NULL;
    delete_dictionary(app_container->elements_release_callback);
    app_container->elements_release_callback = NULL;
    delete_dictionary(app_container->elements_args);
    app_container->elements_args = NULL;
    delete_dictionary(app_container->elements_refs);
    app_container->elements_refs = NULL;
    pthread_mutex_unlock(&app_container->mutex);
    pthread_mutex_destroy(&app_container->mutex);
    remove_record_from_dictionary(containers, container->name);

    free(container);
    if (containers->size == 0) {
        delete_dictionary(containers);
        containers = NULL;
    }
    return 0;
}

int release_container_by_name(const char *name) {
    if (!containers)
        return IPEE_ERROR_CODE__CONTAINER__SERVICE_UNINITIALIZED;

    if (!contains_key_in_dictionary(containers, name))
        return 0;

    p_container container = (p_container)get_value_from_dictionary(containers, name);
    return release_container(container);
}

int release_all_containers(void) {
    if (!containers)
        return IPEE_ERROR_CODE__CONTAINER__SERVICE_UNINITIALIZED;

    iterate_over_dictionary_values(
        containers, (dictionary_iteration_values_callback)release_container);
    return 0;
}

int add_glblvalue_to_global_container(
    char *key, void *value, container_callback_function release_callback) {
    return add_service_to_global_container(
        SERVICE_TYPE_GLBLVALUE, key, NULL, release_callback, value);
}

int add_glblvalue_to_container(
    p_container container, char *key, void *value,
    container_callback_function release_callback) {
    return add_service_to_container(
        container, SERVICE_TYPE_GLBLVALUE, key, NULL, release_callback, value);
}

int add_glblvalue_to_container_by_name(
    const char *name, char *key, void *value,
    container_callback_function release_callback) {
    if (!containers)
        return IPEE_ERROR_CODE__CONTAINER__SERVICE_UNINITIALIZED;

    p_container container = (p_container)get_value_from_dictionary(containers, name);
    if (!container)
        return IPEE_ERROR_CODE__CONTAINER__NOT_EXISTS;

    return add_service_to_container(
        container, SERVICE_TYPE_GLBLVALUE, key, NULL, release_callback, value);
}

int add_singleton_to_global_container(
    char *key, container_callback_function initial_callback,
    container_callback_function release_callback) {
    return add_service_to_global_container(
        SERVICE_TYPE_SINGLETON, key, initial_callback, release_callback, NULL);
}

int add_singleton_to_container(
    p_container container, char *key,
    container_callback_function initial_callback,
    container_callback_function release_callback) {
    return add_service_to_container(
        container, SERVICE_TYPE_SINGLETON, key, initial_callback, release_callback, NULL);
}

int add_singleton_to_container_by_name(
    const char *name, char *key,
    container_callback_function initial_callback,
    container_callback_function release_callback) {
    if (!containers)
        return IPEE_ERROR_CODE__CONTAINER__SERVICE_UNINITIALIZED;

    p_container container = (p_container)get_value_from_dictionary(containers, name);
    if (!container)
        return IPEE_ERROR_CODE__CONTAINER__NOT_EXISTS;

    return add_service_to_container(
        container, SERVICE_TYPE_SINGLETON, key, initial_callback, release_callback, NULL);
}

int add_transient_to_global_container(
    char *key, container_callback_function initial_callback,
    container_callback_function release_callback) {
    return add_service_to_global_container(
        SERVICE_TYPE_TRANSIENT, key, initial_callback, release_callback, NULL);
}

int add_transient_to_container(
    p_container container, char *key,
    container_callback_function initial_callback,
    container_callback_function release_callback) {
    return add_service_to_container(
        container, SERVICE_TYPE_TRANSIENT, key, initial_callback, release_callback, NULL);
}

int add_transient_to_container_by_name(
    const char *name, char *key,
    container_callback_function initial_callback,
    container_callback_function release_callback) {
    if (!containers)
        return IPEE_ERROR_CODE__CONTAINER__SERVICE_UNINITIALIZED;

    p_container container = (p_container)get_value_from_dictionary(containers, name);
    if (!container)
        return IPEE_ERROR_CODE__CONTAINER__NOT_EXISTS;

    return add_service_to_container(
        container, SERVICE_TYPE_TRANSIENT, key, initial_callback, release_callback, NULL);
}

int add_service_to_global_container(
    const service_type_t type, char *key,
    container_callback_function initial_callback,
    container_callback_function release_callback, void *args) {
    if (!containers)
        containers = create_dictionary();

    p_container container = (p_container)get_value_from_dictionary(containers, name_of(global));
    if (!container)
        container = init_container(name_of(global));

    return add_service_to_container(container, type, key, initial_callback, release_callback, args);
}

int add_service_to_container(
    p_container container, const service_type_t type, char *key,
    container_callback_function initial_callback,
    container_callback_function release_callback, void *args) {
    if (!containers)
        return IPEE_ERROR_CODE__CONTAINER__SERVICE_UNINITIALIZED;
    if (!container)
        return IPEE_ERROR_CODE__CONTAINER__NOT_EXISTS;

    p_application_container app_container = (p_application_container)container;
    pthread_mutex_lock(&app_container->mutex);

    int is_replacement = contains_key_in_dictionary(app_container->elements_types, key);
    if (!is_replacement && container->services->size >= MAX_CONTAINER_SIZE) {
        pthread_mutex_unlock(&app_container->mutex);
        return 0;
    }

    container_callback_function callback = get_value_from_dictionary(
        app_container->elements_release_callback, key);
    if (callback)
        release_service(key, callback, app_container);

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
        pthread_mutex_unlock(&app_container->mutex);
        return IPEE_ERROR_CODE__CONTAINER__NOT_EXISTS;
    }

    pthread_mutex_unlock(&app_container->mutex);
    return 0;
}

int add_service_to_container_by_name(
    const char *name, const service_type_t type, char *key,
    container_callback_function initial_callback,
    container_callback_function release_callback, void *args) {
    if (!containers)
        return IPEE_ERROR_CODE__CONTAINER__SERVICE_UNINITIALIZED;

    p_container container = (p_container)get_value_from_dictionary(containers, name);
    if (!container)
        return IPEE_ERROR_CODE__CONTAINER__NOT_EXISTS;

    return add_service_to_container(container, type, key, initial_callback, release_callback, args);
}

void *get_service_from_global_container(char *key) {
    if (!containers)
        return NULL;

    p_container container = (p_container)get_value_from_dictionary(containers, name_of(global));
    if (!container)
        return NULL;

    return get_service_from_container(container, key);
}

void *get_service_from_container(p_container container, char *key) {
    return get_service_from_container_with_args(container, key, NULL);
}

void *get_service_from_container_with_args(
    p_container container, char *key, void *tmp_args) {
    if (!containers || !container)
        return NULL;

    p_application_container app_container = (p_application_container)container;

    service_type_t type = (service_type_t)get_value_from_dictionary(
        app_container->elements_types, key);
    switch (type) {
    case SERVICE_TYPE_SINGLETON:
        pthread_mutex_lock(&app_container->mutex);
        void *instance = get_value_from_dictionary(app_container->elements_refs, key);
        if (!instance) {
            container_callback_function initial_callback = get_value_from_dictionary(
                app_container->elements_initial_callback, key);
            void *args = get_value_from_dictionary(app_container->elements_args, key);
            if (tmp_args)
                args = tmp_args;

            instance = initial_callback(args);
            update_record_in_dictionary(app_container->elements_refs, key, instance);
        }
        pthread_mutex_unlock(&app_container->mutex);
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
                container_callback_function release_callback = get_value_from_dictionary(
                    app_container->elements_release_callback, key);
                release_callback(head_value);
                remove_record_from_dictionary_by_index(refs, 0);
            }
            pthread_mutex_unlock(&app_container->mutex);
        }

        container_callback_function initial_callback = get_value_from_dictionary(
            app_container->elements_initial_callback, key);
        add_record_to_dictionary(refs, key, initial_callback(args));
        return get_tail_value_from_dictionary(refs);

    case SERVICE_TYPE_GLBLVALUE:
        return get_value_from_dictionary(app_container->elements_args, key);

    default:
        break;
    }

    return NULL;
}

void *get_service_from_container_by_name(const char *name, char *key) {
    if (!containers)
        return NULL;

    p_container container = (p_container)get_value_from_dictionary(containers, name);
    if (!container)
        return NULL;

    return get_service_from_container(container, key);
}

/***********************************************************************************************
 * STATIC FUNCTIONS DEFINITIONS
 **********************************************************************************************/

static void release_service(
    char *key, container_callback_function callback, p_application_container container) {
    if (!container)
        return;

    service_type_t type = (service_type_t)get_value_from_dictionary(
        container->elements_types, key);
    switch (type) {
    case SERVICE_TYPE_SINGLETON:
        void *instance = get_value_from_dictionary(container->elements_refs, key);
        if (instance && callback)
            callback(instance);
        break;

    case SERVICE_TYPE_TRANSIENT:
        p_dictionary refs = get_value_from_dictionary(container->elements_refs, key);
        if (refs && callback)
            iterate_over_dictionary_values(refs, (dictionary_iteration_values_callback)callback);
        delete_dictionary(refs);
        refs = NULL;
        break;

    case SERVICE_TYPE_GLBLVALUE:
        void *args = get_value_from_dictionary(container->elements_args, key);
        if (callback)
            callback(args);
        break;

    default:
        break;
    }
}
