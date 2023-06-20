cmake_minimum_required(VERSION 3.19)

project(Protest-Unittest CXX C ASM)

# not needed but makes build easier
include(GNUInstallDirs)

set(PROTEST_INCLUDE_UNIT_TESTS true)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

# build.Unittest && lcov -c -d . -o name.info
# build.Unittest && genhtml name.info

add_subdirectory(../../modules/core/src ./modules/core/src)
add_subdirectory(../../modules/coro/src ./modules/coro/src)
add_subdirectory(../../modules/coro/test ./modules/coro/test)
add_subdirectory(../../modules/coro/arch/posix/src ./modules/coro/arch/posix/src)
add_subdirectory(../../modules/doc/src ./modules/doc/src)
add_subdirectory(../../modules/log/src ./modules/log/src)
add_subdirectory(../../modules/matcher/src ./modules/matcher/src)
add_subdirectory(../../modules/meta/src ./modules/meta/src)
add_subdirectory(../../modules/mock/src ./modules/mock/src)
add_subdirectory(../../modules/mock/test ./modules/mock/test)
add_subdirectory(../../modules/utils/src ./modules/utils/src)
add_subdirectory(../../modules/utils/test ./modules/utils/test)
add_subdirectory(../../modules/time/src ./modules/time/src)
add_subdirectory(../../modules/time/test ./modules/time/test)
add_subdirectory(../../modules/rtos/src ./modules/rtos/src)
add_subdirectory(../../modules/rtos/test ./modules/rtos/test)
add_subdirectory(../../modules/rtos/arch/protest/src ./modules/rtos/arch/protest/src)
add_subdirectory(../../modules/rtos/arch/posix/src ./modules/rtos/arch/posix/src)
add_subdirectory(../../ext/googletest ./ext/googletest)

set(sources
  "main.cpp"
)

add_executable(demo ${sources})
target_link_libraries(demo
  pthread
  m
  utils
  utils_test
  time
  time_test
  coro
  coro_test
  rtos
  rtos_test
  mock
  mock_test
  gtest
)

add_custom_target(cov.html ALL
    COMMAND ./demo && lcov -c -d . -o name.info && genhtml name.info
    DEPENDS demo
    VERBATIM
)