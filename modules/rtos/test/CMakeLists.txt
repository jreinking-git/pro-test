set(sources
  "protest/rtos/queue_test.cpp"
)

 if (PROTEST_INCLUDE_UNIT_TESTS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  --coverage")
endif()
add_library(rtos_test OBJECT ${sources})
target_link_libraries(rtos_test rtos rtos_posix gtest)
