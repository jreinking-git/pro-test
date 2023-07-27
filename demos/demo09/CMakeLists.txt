cmake_minimum_required(VERSION 3.19)

project(demo-09 C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (${DEVELOPMENT_BUILD})
  add_subdirectory(../../ protest-test)
  include(../../cmake/ProtestCompilerDevelopment.cmake)
else()
  find_package(Protest 0.1.0 REQUIRED)
  include(ProtestCompiler)
endif()

add_subdirectory(sut/module_a)
add_subdirectory(sut/module_b)
add_subdirectory(tests/runner1)
add_subdirectory(tests/runner2)

set(modules
  runner1
  runner2
)

add_protest_files(SOURCES "main.pt.cpp"
                 TARGETS protestOut
                 DEPENDENCIES ${modules}
                 INCLUDES ${CMAKE_CURRENT_LIST_DIR}
                 DEFINITIONS FLAG_A FLAG_B)

add_protest_executable(TARGET run_test SOURCES ${protestOut})

target_compile_definitions(run_test PUBLIC FLAG_A FLAG_B)

target_link_libraries(run_test
  runner1
  runner2
  protest::doc)