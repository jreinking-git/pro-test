set(sources
  "module_b.cpp"
)

add_library(module_b STATIC ${sources})
target_include_directories(module_b PUBLIC .)
target_link_libraries(module_b protest::core)
target_compile_definitions(module_b PUBLIC COMPILED_WITH_MODULE_B)
