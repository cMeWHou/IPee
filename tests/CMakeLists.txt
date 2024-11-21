set(PROJECT_TESTS ${PROJECT}Tests)
set(AVAILABLE_TESTS
  "dictionary_test.c"
  "container_test.c"
  "event_test.c"
  "hashmap_test.c"
  "threadpool_test.c"
)
create_test_sourcelist(TESTS_SOURCES IpeeTests.c ${AVAILABLE_TESTS})

add_executable(${PROJECT_TESTS} ${TESTS_SOURCES} "utils/helper.c")
target_include_directories(${PROJECT_TESTS} PUBLIC ${HEADER_PATH} "utils/")
target_link_libraries(${PROJECT_TESTS} ${PROJECT_LIB})

foreach (TEST ${AVAILABLE_TESTS})
  get_filename_component(TEST_NAME ${TEST} NAME_WE)
  add_test(NAME ${TEST_NAME} COMMAND ${PROJECT_TESTS} ${TEST_NAME})
endforeach ()