cmake_minimum_required(VERSION 3.19)

project(demo-14 C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (${DEVELOPMENT_BUILD})
  add_subdirectory(../../ protest-test)
  include(../../cmake/ProtestCompilerDevelopment.cmake)
else()
  find_package(Protest 0.1.0 REQUIRED)
  include(ProtestCompiler)
endif()

set(protestSrc
  "main.pt.cpp"
)

set (modules
  protest::mock
)

add_protest_files(SOURCES ${protestSrc}
                  TARGETS protestOut
                  DEPENDENCIES ${modules}
                  INCLUDES "${CMAKE_CURRENT_LIST_DIR}")

add_executable(run_test ${protestOut} "my_class.cpp")
target_include_directories(run_test PUBLIC ".")
target_link_libraries(run_test
  ${modules}
  protest::core
  protest::doc
  protest::rtos_protest)