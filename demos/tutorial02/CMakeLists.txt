cmake_minimum_required(VERSION 3.19)

project(tutorial-02 C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Protest 0.1.0 REQUIRED)
include(ProtestCompiler)

set(protestSrc
  "main.pt.cpp"
)

add_protest_file(SOURCES ${protestSrc}
                 TARGETS protestOut)

add_executable(run_test ${protestOut})
target_link_libraries(run_test
  protest::rtos_protest
  protest::core
  protest::doc)