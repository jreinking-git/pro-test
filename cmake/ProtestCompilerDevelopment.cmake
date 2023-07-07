# This file is the equivalent for 'ProtestCompiler.cmake' for development.
# Using this file CMake can build a demo project (using the protest API)
# and pro-test (compiler and libraries) itself in the same project. This
# makes it easier to test changes which can be tested without
# reinstalling the whole project.

include(CMakeParseArguments)

# when installing the libraries a namespace protest:: is added.
# In developing mode use aliases with the prefix protest:: instead.
# So if you add a module please add it to the list here

add_library(protest::core ALIAS core)
add_library(protest::doc ALIAS doc)
add_library(protest::matcher ALIAS matcher)
add_library(protest::coro ALIAS coro)
add_library(protest::rtos ALIAS rtos)
add_library(protest::mock ALIAS mock)
add_library(protest::utils ALIAS utils)
add_library(protest::time ALIAS time)
add_library(protest::rtos_protest ALIAS rtos_protest)
add_library(protest::rtos_posix ALIAS rtos_posix)

# when installing protest all headers are copied into the installation
# folder. Since this is a development build which works without
# installing the path to the modules must passed explicit to the protest
# precompiler. If you add a new module. Make sure to add the the include
# path here.

set(PROTEST_ROOT_PATH "${CMAKE_CURRENT_LIST_DIR}/..")

set(IMPLICIT_INCLUDES
  "${PROTEST_ROOT_PATH}/modules/matcher/src"
  "${PROTEST_ROOT_PATH}/modules/doc/src"
  "${PROTEST_ROOT_PATH}/modules/core/src"
  "${PROTEST_ROOT_PATH}/modules/coro/src"
  "${PROTEST_ROOT_PATH}/modules/coro/arch/posix/src"
  "${PROTEST_ROOT_PATH}/modules/log/src"
  "${PROTEST_ROOT_PATH}/modules/time/src"
  "${PROTEST_ROOT_PATH}/modules/meta/src"
  "${PROTEST_ROOT_PATH}/modules/mock/src"
  "${PROTEST_ROOT_PATH}/modules/utils/src"
)

function(add_source_files)
  set(prefix PROTEST)
  set(flags)
  set(singleValues TARGETS PROJECT_ROOT)
  set(multiValues SOURCES DEPENDENCIES INCLUDES DEFINITIONS)

  cmake_parse_arguments(${prefix}
    "${flags}"
    "${singleValues}"
    "${multiValues}"
    ${ARGN})

  if(NOT DEFINED PROTEST_PROJECT_ROOT)
    set(PROTEST_PROJECT_ROOT ${CMAKE_SOURCE_DIR})
  endif()

  foreach(definition ${PROTEST_DEFINITIONS})
    set(DEFINITIONS "-D${definition};${DEFINITIONS}")
  endforeach()

  foreach(module ${PROTEST_DEPENDENCIES})
    set(MODULE_INCLUDE
        "$<TARGET_PROPERTY:${module},INCLUDE_DIRECTORIES>"
    )
    set(MODULE_DEFINITION
      "$<TARGET_PROPERTY:${module},COMPILE_DEFINITIONS>"
    )
    set(DEFINITIONS_COMPILER
      "$<$<BOOL:${MODULE_DEFINITION}>:-D$<JOIN:${MODULE_DEFINITION},;-D>>;${DEFINITIONS_COMPILER}"
    )
    set(INCLUDES_PRECOMPILER
      "$<$<BOOL:${MODULE_INCLUDE}>:$<JOIN:${MODULE_INCLUDE},;>>;${INCLUDES_PRECOMPILER}"
    )
  endforeach()

  foreach(pt ${PROTEST_SOURCES})
    get_filename_component(name ${pt} NAME_WE)
    get_filename_component(ext ${pt} EXT)

    set(OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/${name}.sut${ext}")
    set(${PROTEST_TARGETS} ${${PROTEST_TARGETS}} "${OUTPUT_FILE}")
    set(${PROTEST_TARGETS} ${${PROTEST_TARGETS}} "${OUTPUT_FILE}" PARENT_SCOPE)

    add_custom_command(
      OUTPUT "${OUTPUT_FILE}"
      COMMAND python3 ${PROTEST_ROOT_PATH}/tools/protest-precompiler/ProtestSutPreCompiler.py
      --include-path 
        "${IMPLICIT_INCLUDES}"
        "${INCLUDES_PRECOMPILER}"
        "${PROTEST_INCLUDES}"
      --src-file "${CMAKE_CURRENT_SOURCE_DIR}/${pt}"
      --output-file "${OUTPUT_FILE}"
      COMMAND_EXPAND_LISTS VERBATIM
      IMPLICIT_DEPENDS CXX "${CMAKE_CURRENT_SOURCE_DIR}/${pt}"
      DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${pt}"
    )
  endforeach()
endfunction()

function(add_protest_files)
  set(prefix PROTEST)
  set(flags)
  set(singleValues TARGETS PROJECT_ROOT)
  set(multiValues SOURCES DEPENDENCIES INCLUDES DEFINITIONS)

  cmake_parse_arguments(${prefix}
    "${flags}"
    "${singleValues}"
    "${multiValues}"
    ${ARGN})

  if(NOT DEFINED PROTEST_PROJECT_ROOT)
    set(PROTEST_PROJECT_ROOT ${CMAKE_SOURCE_DIR})
  endif()

  foreach(definition ${PROTEST_DEFINITIONS})
    set(DEFINITIONS "-D${definition};${DEFINITIONS}")
  endforeach()

  foreach(module ${PROTEST_DEPENDENCIES})
    set(MODULE_INCLUDE
      "$<TARGET_PROPERTY:${module},INCLUDE_DIRECTORIES>"
    )
    set(MODULE_DEFINITION
      "$<TARGET_PROPERTY:${module},COMPILE_DEFINITIONS>"
    )
    set(DEFINITIONS_COMPILER
      "$<$<BOOL:${MODULE_DEFINITION}>:-D$<JOIN:${MODULE_DEFINITION},;-D>>;${DEFINITIONS_COMPILER}"
    )
    set(INCLUDES_PRECOMPILER
      "$<$<BOOL:${MODULE_INCLUDE}>:$<JOIN:${MODULE_INCLUDE},;>>;${INCLUDES_PRECOMPILER}"
    )
  endforeach()

  foreach(pt ${PROTEST_SOURCES})
    get_filename_component(name ${pt} NAME_WE)
    get_filename_component(ext ${pt} EXT)
    message("dir: ${CMAKE_CURRENT_SOURCE_DIR}")

    add_custom_command(
      OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${name}.pretest${ext}"
      COMMAND python3 ${PROTEST_ROOT_PATH}/tools/protest-precompiler/ProtestPreCompiler.py
      --include-path
        "${IMPLICIT_INCLUDES}"
        "${INCLUDES_PRECOMPILER}"
        "${PROTEST_INCLUDES}"
      --src-file "${CMAKE_CURRENT_SOURCE_DIR}/${pt}"
      --output-file "${CMAKE_CURRENT_BINARY_DIR}/${name}.pretest${ext}"
      COMMAND_EXPAND_LISTS VERBATIM
      IMPLICIT_DEPENDS CXX "${CMAKE_CURRENT_SOURCE_DIR}/${pt}"
      DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${pt}"
    )

    set(OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/${name}.protest${ext}")
    set(${PROTEST_TARGETS} ${${PROTEST_TARGETS}} "${OUTPUT_FILE}")
    set(${PROTEST_TARGETS} ${${PROTEST_TARGETS}} "${OUTPUT_FILE}" PARENT_SCOPE)

    add_custom_command(
      OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${name}.protest${ext}"
      COMMAND 
        ${CMAKE_BINARY_DIR}/protest-test/tools/protest-create-mocks/protest-create-mocks
          "${CMAKE_CURRENT_BINARY_DIR}/${name}.pretest${ext}"
          -r ${PROTEST_PROJECT_ROOT}
          -o ${name}_mocks.hpp
           -- 
          -ferror-limit=1000
          --std=c++${CMAKE_CXX_STANDARD}
          -DPROTEST_COMPILE_STAGE
          ${DEFINITIONS}
          "${DEFINITIONS_COMPILER}" 2> /dev/null &&
        ${CMAKE_BINARY_DIR}/protest-test/tools/protest-compiler/protest-compiler
          "${CMAKE_CURRENT_BINARY_DIR}/${name}.pretest${ext}"
          -r ${PROTEST_PROJECT_ROOT}
          -o ${OUTPUT_FILE}
           -- 
          --std=c++${CMAKE_CXX_STANDARD}
          -DPROTEST_COMPILE_STAGE ${DEFINITIONS}
          "${DEFINITIONS_COMPILER}"
      COMMAND_EXPAND_LISTS VERBATIM
      # without installing the whole project, this function call depends on the
      # protest-compiler, which will be build in the same build as the test
      # project.
      DEPENDS
        "${CMAKE_CURRENT_BINARY_DIR}/${name}.pretest${ext}"
        protest-compiler
        protest-create-mocks
    )
  endforeach()
endfunction()