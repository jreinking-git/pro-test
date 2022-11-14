set(sources
  "protest/log/logger.cpp"
)

 if (PROTEST_INCLUDE_UNIT_TESTS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  --coverage")
endif()
add_library(log STATIC ${sources})
target_include_directories(log PUBLIC .)
target_link_libraries(log coro)