# set(CMAKE_CXX_CLANG_TIDY clang-tidy)

cmake_minimum_required(VERSION 3.19)

project(Protest LANGUAGES C CXX VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

find_package(nlohmann_json REQUIRED)

add_subdirectory(./modules/core/src)
add_subdirectory(./modules/coro/src)
add_subdirectory(./modules/coro/arch/posix/src)
add_subdirectory(./modules/matcher/src)
add_subdirectory(./modules/doc/src)
add_subdirectory(./modules/json/src)
add_subdirectory(./modules/log/src)
add_subdirectory(./modules/meta/src)
add_subdirectory(./modules/mock/src)
add_subdirectory(./modules/utils/src)
add_subdirectory(./modules/time/src)
add_subdirectory(./modules/t3/src)
add_subdirectory(./modules/rtos/src)
add_subdirectory(./modules/rtos/arch/protest/src)
add_subdirectory(./modules/rtos/arch/posix/src)
add_subdirectory(./tools)

configure_package_config_file(
  cmake/ProtestConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/ProtestConfig.cmake
  PATH_VARS CMAKE_INSTALL_INCLUDEDIR
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Protest
)

write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/ProtestConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY ExactVersion
)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/ProtestConfig.cmake ${CMAKE_CURRENT_BINARY_DIR}/ProtestConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Protest
)

install(FILES
  ${CMAKE_CURRENT_LIST_DIR}/cmake/ProtestCompiler.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Protest/modules
)

install(EXPORT Protest
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Protest
  NAMESPACE protest::
)