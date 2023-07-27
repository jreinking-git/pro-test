cmake_minimum_required(VERSION 3.19)

project(demo-04 C CXX)

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
)

add_protest_files(SOURCES ${protestSrc}
                 TARGETS protestOut
                 DEPENDENCIES ${modules})

add_protest_executable(TARGET run_test SOURCES ${protestOut})
target_link_libraries(run_test
  ${modules}
  protest::core
  protest::doc)