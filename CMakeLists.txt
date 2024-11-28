cmake_minimum_required(VERSION 3.1.0)

set(PROJECT IpEe)
project(${PROJECT})
add_compile_options(-Wno-discarded-qualifiers -Wno-incompatible-pointer-types -Wno-int-conversion)
set(INCLUDE_PATH "${CMAKE_SOURCE_DIR}/include/")
include_directories(${INCLUDE_PATH})

# Utility functions
set(UTILS_SRS "${CMAKE_SOURCE_DIR}/src/utils.c")
set(UTILS_LIB ${PROJECT}Utils)
add_library(${UTILS_LIB} ${UTILS_SRS})

# Dictionary collection
set(DICTIONARY_SRC "${CMAKE_SOURCE_DIR}/src/dictionary.c")
set(DICTIONARY_LIB ${PROJECT}Dictionary)
add_library(${DICTIONARY_LIB} ${DICTIONARY_SRC})
target_link_libraries(${DICTIONARY_LIB} ${UTILS_LIB})

# Container
set(CONTAINER_SRC "${CMAKE_SOURCE_DIR}/src/container.c")
set(CONTAINER_LIB ${PROJECT}Container)
add_library(${CONTAINER_LIB} ${CONTAINER_SRC})
target_link_libraries(${CONTAINER_LIB} ${DICTIONARY_LIB})

# Event
set(EVENT_SRC "${CMAKE_SOURCE_DIR}/src/event.c")
set(EVENT_LIB ${PROJECT}Event)
add_library(${EVENT_LIB} ${EVENT_SRC})
target_link_libraries(${EVENT_LIB} ${DICTIONARY_LIB})

# Threapool
set(THREADPOOL_SRC "${CMAKE_SOURCE_DIR}/src/threadpool.c")
set(THREADPOOL_LIB ${PROJECT}Threadpool)
add_library(${THREADPOOL_LIB} ${THREADPOOL_SRC})
target_link_libraries(${THREADPOOL_LIB} ${DICTIONARY_LIB})

# All
set(PROJECT_SRC ${UTILS_SRS} ${DICTIONARY_SRC} ${CONTAINER_SRC} ${EVENT_SRC} ${THREADPOOL_SRC})
set(PROJECT_LIB ${PROJECT})
add_library(${PROJECT_LIB} ${PROJECT_SRC})

# Testing
enable_testing()
include(CTest)
add_subdirectory(tests)