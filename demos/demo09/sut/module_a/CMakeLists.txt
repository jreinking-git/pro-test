set(sources
  "module_a.cpp"
)

add_library(module_a STATIC ${sources})
target_include_directories(module_a PUBLIC .)
target_link_libraries(module_a protest::core)
