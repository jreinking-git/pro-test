cmake_minimum_required(VERSION 3.19)

project(reliable-broadcast C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Protest 0.1.0 REQUIRED)
include(ProtestCompiler)

add_subdirectory(sut)

set(protestSrc
  "main.pt.cpp"
)

set (modules
  sut
)

add_protest_file(SOURCES ${protestSrc}
                 TARGETS protestOut
                 DEPENDENCIES ${modules})

add_executable(run_test ${protestOut})
target_link_libraries(run_test
  ${modules}
  protest::core
  protest::doc
  protest::rtos_protest)

add_executable(test "main.cpp")
target_link_libraries(test
  ${modules}
  protest::rtos_posix)