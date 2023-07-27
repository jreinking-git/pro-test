set(sources
  "protest/json/json.cpp"
)

if (PROTEST_INCLUDE_UNIT_TESTS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  --coverage")
endif()

add_library(json STATIC ${sources})

target_include_directories(json PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_link_libraries(json
  nlohmann_json::nlohmann_json)

install(TARGETS json EXPORT Protest)
install(DIRECTORY . DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} FILES_MATCHING PATTERN *.h)