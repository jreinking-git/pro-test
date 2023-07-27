cmake_minimum_required(VERSION 3.19)

# https://github.com/llvm/llvm-project.git 6ffb3ad631c5071ce82c8b6c73dd1c88e0452944
# find_package(LLVM  REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  "${LLVM_CMAKE_DIR}"
)

include(AddLLVM)

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})

set(CLANG_VERSION "${LLVM_VERSION_MAJOR}.${LLVM_VERSION_MINOR}.${LLVM_VERSION_PATCH}")

add_definitions(${LLVM_DEFINITIONS})
add_definitions(${LLVM_INCLUDE_DIRS})

add_llvm_executable(protest-compiler "protest_compiler.cpp" "member_func_visitor.cpp" "member_attr_visitor.cpp" "static_attr_visitor.cpp" "static_var_visitor.cpp" "utils.cpp")
set_property(TARGET protest-compiler PROPERTY CXX_STANDARD 17)
target_link_libraries(protest-compiler PRIVATE clangTooling)
target_compile_definitions(protest-compiler PUBLIC PATH_TO_STD_CLANG_INCLUDE="${LLVM_LIBRARY_DIR}/clang/${CLANG_VERSION}/include")

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/protest-compiler
  DESTINATION bin
  PERMISSIONS GROUP_READ GROUP_EXECUTE OWNER_EXECUTE OWNER_READ WORLD_EXECUTE WORLD_READ
)