set(sources
  "runner2.cpp"
  "runner3.cpp"
)

set(includes
  ${CMAKE_CURRENT_LIST_DIR}
)

add_protest_file(SOURCES ${sources}
                 TARGETS protestOut
                 INCLUDES ${includes})

add_library(runner2 STATIC ${protestOut})
target_include_directories(runner2 PUBLIC ${includes})
target_link_libraries(runner2 protest::core)
