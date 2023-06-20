set(sources
  "module_a.cpp"
)

add_custom_command(
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/module_a.sut.cpp"
  COMMAND python3 ../../../../../tools/protest-precompiler/ProtestSutPreCompiler.py
  --include-path "${CMAKE_CURRENT_SOURCE_DIR}"
  --src-file "${CMAKE_CURRENT_SOURCE_DIR}/module_a.cpp" --output-file "${CMAKE_CURRENT_BINARY_DIR}/module_a.sut.cpp"
  COMMAND_EXPAND_LISTS VERBATIM
  IMPLICIT_DEPENDS CXX "${CMAKE_CURRENT_SOURCE_DIR}/module_a.cpp"
  # DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${pt}")
)

add_library(module_a STATIC "${CMAKE_CURRENT_BINARY_DIR}/module_a.sut.cpp")
target_include_directories(module_a PUBLIC .)
