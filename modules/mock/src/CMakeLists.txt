set(sources
  "protest/mock/mock_base.cpp"
  "protest/mock/expectation.cpp"
  "protest/mock/function_mocker.cpp"
  "protest/mock/cardinality.cpp"
  "protest/mock/sequence.cpp"
  "protest/mock/in_sequence.cpp"
)

if (PROTEST_INCLUDE_UNIT_TESTS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  --coverage")
endif()

add_library(mock STATIC ${sources})

target_include_directories(mock PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_link_libraries(mock
  time
  log
  meta)

install(TARGETS mock EXPORT Protest)
install(DIRECTORY . DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} FILES_MATCHING PATTERN *.h)