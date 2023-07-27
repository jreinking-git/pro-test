set(sources
  "protest/json/json_test.cpp"
)

if (PROTEST_INCLUDE_UNIT_TESTS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  --coverage")
endif()

add_library(json_test OBJECT ${sources})
target_link_libraries(json_test json gtest)
target_include_directories(json_test PUBLIC .)
