set(sources
  "protest/rtos/log.cpp"
  "protest/rtos/mutex.cpp"
  "protest/rtos/thread.cpp"
  "protest/rtos/semaphore.cpp"
  "protest/rtos/system_clock.cpp"
)

if (PROTEST_INCLUDE_UNIT_TESTS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  --coverage")
endif()

add_library(rtos_protest STATIC ${sources})
target_include_directories(rtos_protest PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(rtos_protest rtos time utils core)

install(TARGETS rtos_protest EXPORT Protest)
install(DIRECTORY . DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} FILES_MATCHING PATTERN *.h)