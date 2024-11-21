cmake_minimum_required(VERSION 3.5.0)

set(PROJECT IpEe)
project(${PROJECT})
add_compile_options(-Wno-discarded-qualifiers -Wno-incompatible-pointer-types -Wno-int-conversion)
set(INCLUDE_PATH "${CMAKE_SOURCE_DIR}/include/")

# Dictionary collection
set(DICTIONARY_SRC "${CMAKE_SOURCE_DIR}/src/dictionary.c")
set(DICTIONARY_LIB ${PROJECT}Dictionary)
add_library(${DICTIONARY_LIB} ${DICTIONARY_SRC})
target_include_directories(${DICTIONARY_LIB} PUBLIC ${INCLUDE_PATH})

# Container
set(CONTAINER_SRC "${CMAKE_SOURCE_DIR}/src/container.c")
set(CONTAINER_LIB ${PROJECT}Container)
add_library(${CONTAINER_LIB} ${CONTAINER_SRC})
target_include_directories(${CONTAINER_LIB} PUBLIC ${INCLUDE_PATH})
target_link_libraries(${CONTAINER_LIB} ${DICTIONARY_LIB})

# Event
set(EVENT_SRC "${CMAKE_SOURCE_DIR}/src/event.c")
set(EVENT_LIB ${PROJECT}Event)
add_library(${EVENT_LIB} ${EVENT_SRC})
target_include_directories(${EVENT_LIB} PUBLIC ${INCLUDE_PATH})
target_link_libraries(${EVENT_LIB} ${DICTIONARY_LIB})

# Hashmap
set(HASHMAP_SRC "${CMAKE_SOURCE_DIR}/src/hashmap.c")
set(HASHMAP_LIB ${PROJECT}Hashmap)
add_library(${HASHMAP_LIB} ${HASHMAP_SRC})
target_include_directories(${HASHMAP_LIB} PUBLIC ${INCLUDE_PATH})

# Threapool
set(THREADPOOL_SRC "${CMAKE_SOURCE_DIR}/src/threadpool.c")
set(THREADPOOL_LIB ${PROJECT}Threadpool)
add_library(${THREADPOOL_LIB} ${THREADPOOL_SRC})
target_include_directories(${THREADPOOL_LIB} PUBLIC ${INCLUDE_PATH})
target_link_libraries(${THREADPOOL_LIB} ${DICTIONARY_LIB})

# All
set(PROJECT_SRC ${DICTIONARY_SRC} ${CONTAINER_SRC} ${EVENT_SRC} ${HASHMAP_SRC} ${THREADPOOL_SRC})
set(PROJECT_LIB ${PROJECT})
add_library(${PROJECT_LIB} ${PROJECT_SRC})
target_include_directories(${PROJECT_LIB} PUBLIC ${INCLUDE_PATH})

# Testing
enable_testing()
include(CTest)
add_subdirectory(tests)