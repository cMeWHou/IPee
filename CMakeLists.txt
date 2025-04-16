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

# Bitmap collection
set(BITMAP_SRC "${CMAKE_SOURCE_DIR}/src/bitmap.c")
set(BITMAP_LIB ${PROJECT}Bitmap)
add_library(${BITMAP_LIB} ${BITMAP_SRC})
target_include_directories(${BITMAP_LIB} PUBLIC ${INCLUDE_PATH})

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
target_link_libraries(${THREADPOOL_LIB} ${DICTIONARY_LIB} ${EVENT_LIB} ${BITMAP_LIB})

# Logger
set(LOGGER_SRC "${CMAKE_SOURCE_DIR}/src/logger.c")
set(LOGGER_LIB ${PROJECT}Logger)
add_library(${LOGGER_LIB} ${LOGGER_SRC})
target_include_directories(${LOGGER_LIB} PUBLIC ${INCLUDE_PATH})

# All
set(PROJECT_SRC ${DICTIONARY_SRC} ${CONTAINER_SRC} ${EVENT_SRC} ${HASHMAP_SRC} ${BITMAP_SRC} ${THREADPOOL_SRC} ${LOGGER_SRC})
set(PROJECT_LIB ${PROJECT})
add_library(${PROJECT_LIB} ${PROJECT_SRC})
target_include_directories(${PROJECT_LIB} PUBLIC ${INCLUDE_PATH})

# This definition is used to turn off logging
# target_compile_definitions(${PROJECT_LIB} PUBLIC EE_LOG_OFF)

# Need for clangd
target_compile_definitions(${PROJECT_LIB} PUBLIC CMAKE_EXPORT_COMPILE_COMMANDS=1)

# Copy logger configuration file to build directory
file(COPY_FILE "${CMAKE_SOURCE_DIR}/cfg/config_logger" "${CMAKE_BINARY_DIR}/tests/config_logger" ONLY_IF_DIFFERENT)

# Testing
enable_testing()
include(CTest)
add_subdirectory(tests)