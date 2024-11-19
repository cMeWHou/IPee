/**
 * @file container.h
 * @author chcp (cmewhou@yandex.ru)
 * @brief Common container for global access to application services.
 * @version 1.5
 * @date 2024-11-18
 *
 * @copyright Copyright (c) 2024
 */

#ifndef IPEE_CONTAINER_H
#define IPEE_CONTAINER_H

#include <dictionary.h>

/*********************************************************************************************
 * STRUCTS DECLARATIONS
 ********************************************************************************************/

/**
 * @brief Element lifetime type.
 *
 * @details
 * Defines the element lifetime.
 * Singleton - one instance of service in the application.
 * Transient - new instance of service on each new request.
 * Glblvalue - global value.
 */
typedef enum service_type_e {
    SERVICE_TYPE_SINGLETON = 0, // One instance of service in the application.
    SERVICE_TYPE_TRANSIENT = 1, // New instance of service on each new request.
    SERVICE_TYPE_GLBLVALUE = 2, // Global value.
} service_type_t, *p_service_type;

/**
 * @brief Container.
 */
typedef struct container_s {
    const char *name;
    const p_dictionary services;
} container_t, *p_container;

/***********************************************************************************************
 * FUNCTION TYPEDEFS
 **********************************************************************************************/

/**
 * @brief Callback function for initializing or releasing services.
 *
 * @param args Arguments for callback function.
 */
typedef void *(*container_callback_function)(void *args);

/*********************************************************************************************
 * FUNCTIONS DECLARATIONS
 ********************************************************************************************/

/**
 * @brief Set max transient service count.
 * 
 * @param count Max transient service count.
 */

extern void set_max_service_count(int count);

/**
 * @brief Set max transient service count.
 * 
 * @param count Max transient service count.
 */

extern void set_max_transient_service_count(int count);

/**
 * @brief Get the all containers object
 * 
 * @return All registered containers.
 */
extern p_dictionary get_all_containers(void);

/**
 * @brief Get container by name.
 * 
 * @param name Container name.
 * @return Container instance.
 */
extern p_container get_container(const char *name);

/**
 * @brief Initialize container.
 *
 * @details
 * Create application container.
 * To add services to it use "add_service_to_container" function.
 */
extern p_container init_container(const char *name);

/**
 * @brief Release container by reference.
 * 
 * @details
 * Release container and all services.
 * Invokes callback function to release resources of each service.
 * 
 * @param container Container reference.
 */
extern void release_container(p_container container);

/**
 * @brief Release container by name.
 * 
 * @details
 * Release container and all services.
 * Invokes callback function to release resources of each service.
 * 
 * @param name Container name.
 */
extern void release_container_by_name(const char *name);

/**
 * @brief Release all containers.
 * 
 * @details
 * Release all containers and all services.
 * Invokes callback function to release resources of each service.
 */
extern void release_all_containers(void);

/**
 * @brief Add new global value to global container.
 * 
 * @param key Global value identifier.
 * @param value Global value.
 * @param release_callback Release callback function.
 * 
 * @details
 * Add new global value to application container.
 * If global value with specified key exists it will be replaced.
 */
extern void add_glblvalue_to_global_container(char *key, void *value,
                                              container_callback_function release_callback);

/**
 * @brief Add new global value to container.
 * 
 * @param container Container reference.
 * @param key Global value identifier.
 * @param value Global value.
 * @param release_callback Release callback function.
 * 
 * @details
 * Add new global value to application container.
 * If global value with specified key exists it will be replaced.
 */
extern void add_glblvalue_to_container(p_container container, char *key, void *value,
                                       container_callback_function release_callback);

/**
 * @brief Add new global value to container by name.
 * 
 * @param name Container name.
 * @param key Global value identifier.
 * @param value Global value.
 * @param release_callback Release callback function.
 * 
 * @details
 * Add new global value to application container.
 * If global value with specified key exists it will be replaced.
 */
extern void add_glblvalue_to_container_by_name(const char *name, char *key, void *value,
                                               container_callback_function release_callback);

/**
 * @brief Add new singleton to global container.
 * 
 * @details
 * Add new singleton to application container.
 * If singleton with specified key exists it will be replaced.
 * 
 * @param key Singleton identifier.
 * @param initial_callback Initial callback function.
 * @param release_callback Release callback function.
 */
extern void add_singleton_to_global_container(char *key, container_callback_function initial_callback,
                                              container_callback_function release_callback);

/**
 * @brief Add new singleton to container.
 *
 * @details
 * Add new singleton to application container.
 * If singleton with specified key exists it will be replaced.
 * 
 * @param container Container reference.
 * @param key Singleton identifier.
 * @param initial_callback Initial callback function.
 * @param release_callback Release callback function.
 */
extern void add_singleton_to_container(p_container container, char *key,
                                       container_callback_function initial_callback,
                                       container_callback_function release_callback);

/**
 * @brief Add new singleton to container by name.
 *
 * @details
 * Add new singleton to application container.
 * If singleton with specified key exists it will be replaced.
 * 
 * @param name Container name.
 * @param key Singleton identifier.
 * @param initial_callback Initial callback function.
 * @param release_callback Release callback function.
 */
extern void add_singleton_to_container_by_name(const char *name, char *key,
                                               container_callback_function initial_callback,
                                               container_callback_function release_callback);

/**
 * @brief Add new transient to global container.
 * 
 * @details
 * Add new transient to application container.
 * If transient with specified key exists it will be replaced
 * 
 * @param key Transient identifier.
 * @param initial_callback Initial callback function.
 * @param release_callback Release callback function.
 */
extern void add_transient_to_global_container(char *key, container_callback_function initial_callback,
                                              container_callback_function release_callback);

/**
 * @brief Add new transient to container.
 *
 * @details
 * Add new transient to application container.
 * If transient with specified key exists it will be replaced.
 * 
 * @param container Container reference.
 * @param key Transient identifier.
 * @param initial_callback Initial callback function.
 * @param release_callback Release callback function.
 */
extern void add_transient_to_container(p_container container, char *key,
                                       container_callback_function initial_callback,
                                       container_callback_function release_callback);

/**
 * @brief Add new transient to container by name.
 *
 * @details
 * Add new transient to application container.
 * If transient with specified key exists it will be replaced.
 * 
 * @param name Container name.
 * @param key Transient identifier.
 * @param initial_callback Initial callback function.
 * @param release_callback Release callback function.
 */
extern void add_transient_to_container_by_name(const char *name, char *key,
                                               container_callback_function initial_callback,
                                               container_callback_function release_callback);

/**
 * @brief Add new service to global container.
 * 
 * @details
 * Add new service to application container.
 * If service with specified key exists it will be replaced.
 * 
 * @param type Lifetime type.
 * @param key Service identifier.
 * @param initial_callback Initial callback function.
 * @param release_callback Release callback function.
 * @param args Arguments.
 */
extern void add_service_to_global_container(const service_type_t type, char *key,
                                            container_callback_function initial_callback,
                                            container_callback_function release_callback,
                                            void *args);

/**
 * @brief Add new service to container.
 *
 * @details
 * Add new service to application container.
 * If service with specified key exists it will be replaced.
 * 
 * @param container Container reference.
 * @param type Lifetime type.
 * @param key Service identifier.
 * @param initial_callback Initial callback function.
 * @param release_callback Release callback function.
 * @param args Arguments.
 */
extern void add_service_to_container(p_container container, const service_type_t type, char *key,
                                     container_callback_function initial_callback,
                                     container_callback_function release_callback,
                                     void *args);

/**
 * @brief Add new service to container by name.
 * 
 * @details
 * Add new service to application container.
 * If service with specified key exists it will be replaced.
 * 
 * @param name Container name.
 * @param type Lifetime type.
 * @param key Service identifier.
 * @param initial_callback Initial callback function.
 * @param release_callback Release callback function.
 * @param args Arguments.
 */
extern void add_service_to_container_by_name(const char *name, const service_type_t type, char *key,
                                             container_callback_function initial_callback,
                                             container_callback_function release_callback,
                                             void *args);

/**
 * @brief Get the service from global container object.
 * 
 * @details
 * Returns specified service instance if it is defined.
 * 
 * @param key Service identifier.
 * @return Service instance.
 */
extern void *get_service_from_global_container(char *key);

/**
 * @brief Get service instance from container.
 *
 * @details
 * Returns specified service instance if it is defined.
 * 
 * @param container Container reference.
 * @param key Service identifier.
 * @return Service instance.
 */
extern void *get_service_from_container(p_container container, char *key);

/**
 * @brief Get service instance from container with arguments.
 *
 * @details
 * Returns specified service instance if it is defined.
 * 
 * @param container Container reference.
 * @param key Service identifier.
 * @param tmp_args Arguments.
 * @return Service instance.
 */
extern void *get_service_from_container_with_args(p_container container, char *key, void *tmp_args);


/**
 * @brief Get service instance from container by name.
 *
 * @details
 * Returns specified service instance if it is defined.
 * 
 * @param name Container name.
 * @param key Service identifier.
 * @return Service instance.
 */
extern void *get_service_from_container_by_name(const char *name, char *key);

#endif // IPEE_CONTAINER_H