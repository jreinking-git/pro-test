set(sources
  "protest/mock/member_function_call_test.cpp"
)

if (PROTEST_INCLUDE_UNIT_TESTS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  --coverage")
endif()

add_library(mock_test OBJECT ${sources})
target_link_libraries(mock_test mock matcher gtest)
target_include_directories(mock_test PUBLIC .)
