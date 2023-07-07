set(sources
  "runner1.cpp"
)

set(includes
  ${CMAKE_CURRENT_LIST_DIR}
)

set(modules
  module_a
  module_b
)

add_protest_files(SOURCES ${sources}
                 TARGETS protestOut
                 DEPENDENCIES ${modules}
                 INCLUDES ${includes})

add_library(runner1 STATIC ${protestOut})
target_include_directories(runner1 PUBLIC ${includes})
target_link_libraries(runner1
  ${modules}
  protest::core
)
