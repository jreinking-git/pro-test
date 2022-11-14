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

add_library(rtos_posix STATIC ${sources})
target_include_directories(rtos_posix PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(rtos_posix rtos time pthread)

install(TARGETS rtos_posix EXPORT Protest)
install(DIRECTORY . DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} FILES_MATCHING PATTERN *.h)