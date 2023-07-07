set(sources
  "module_a.cpp"
)

set(modules)

add_source_files(TARGETS protestOut
                 SOURCES ${sources}
                 DEPENDENCIES ${modules}
                 INCLUDES "${CMAKE_CURRENT_LIST_DIR}")

add_library(module_a STATIC "${protestOut}")
target_include_directories(module_a PUBLIC ".")