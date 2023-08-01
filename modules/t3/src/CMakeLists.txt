set(
  sources
  "protest/t3/member_access.cpp"
)

if (PROTEST_INCLUDE_UNIT_TESTS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  --coverage")
endif()

add_library(t3 STATIC ${sources})

target_link_libraries(
  t3
  core
)

target_include_directories(
  t3 PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

install(TARGETS t3 EXPORT Protest)
install(DIRECTORY . DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} FILES_MATCHING PATTERN *.h)