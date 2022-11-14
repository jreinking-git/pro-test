set(sources
  "protest/time/duration_test.cpp"
)

 if (PROTEST_INCLUDE_UNIT_TESTS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  --coverage")
endif()
add_library(time_test OBJECT ${sources})
target_link_libraries(time_test time gtest)
target_include_directories(time_test PUBLIC .)
