set(sources
  "protest/utils/can_invoke_test.cpp"
  "protest/utils/heap_test.cpp"
  "protest/utils/list_test.cpp"
  "protest/utils/memory_pool_test.cpp"
  "protest/utils/queue_test.cpp"
  "protest/utils/ring_buffer_test.cpp"
)

 if (PROTEST_INCLUDE_UNIT_TESTS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  --coverage")
endif()
add_library(utils_test OBJECT ${sources})
target_link_libraries(utils_test utils gtest)
target_include_directories(utils_test PUBLIC .)
