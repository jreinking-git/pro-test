configure_file(./protest/core/version.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/version.cpp)

set(sources
  "protest/core/condition.cpp"
  "protest/core/context.cpp"
  "protest/core/invariant.cpp"
  "protest/core/job.cpp"
  "protest/core/runner_raw.cpp"
  "protest/core/stopwatch.cpp"
  "protest/core/api.cpp"
  "protest/core/signal_raw.cpp"
  "${CMAKE_CURRENT_BINARY_DIR}/version.cpp"
)

if (PROTEST_INCLUDE_UNIT_TESTS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  --coverage")
endif()

add_library(core STATIC ${sources})

target_include_directories(core PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_link_libraries(core
  meta
  log
  coro
  meta
  doc)

install(TARGETS core EXPORT Protest)
install(DIRECTORY . DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} FILES_MATCHING PATTERN *.h)