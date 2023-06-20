set(sources
  "protest/meta/test_manager.cpp"
  "protest/meta/unit.cpp"
  "protest/meta/call_context.cpp"
)

if (PROTEST_INCLUDE_UNIT_TESTS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  --coverage")
endif()

add_library(meta STATIC ${sources})
target_include_directories(meta PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(meta coro)

install(TARGETS meta EXPORT Protest)
install(DIRECTORY . DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} FILES_MATCHING PATTERN *.h)