cmake_minimum_required(VERSION 3.19)

project(demo-05 C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Protest 0.1.0 REQUIRED)
include(ProtestCompiler)

set(protestSrc
  "main.pt.cpp"
)

set (modules
)

add_protest_file(SOURCES ${protestSrc}
                 TARGETS protestOut
                 DEPENDENCIES ${modules})

add_executable(run_test ${protestOut})
target_link_libraries(run_test
  ${modules}
  protest::core
  protest::doc)