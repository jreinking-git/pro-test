set(sources
  "protest/coro/coroutine_test.cpp"
  "protest/coro/logical_clock_test.cpp"
  "protest/coro/scheduler_test.cpp"
)

 if (PROTEST_INCLUDE_UNIT_TESTS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  --coverage")
endif()
add_library(coro_test OBJECT ${sources})
target_link_libraries(coro_test coro gtest)
target_include_directories(coro_test PUBLIC .)
