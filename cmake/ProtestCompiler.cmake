include(CMakeParseArguments)


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
      COMMAND python3 ${PROTEST_MODULE_PATH}/ProtestSutPreCompiler.py
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

  if (NOT DEFINED PROTEST_PROJECT_ROOT)
    set(PROTEST_PROJECT_ROOT ${CMAKE_SOURCE_DIR})
  endif()

  foreach (definition ${PROTEST_DEFINITIONS})
    set(DEFINITIONS "-D${definition};${DEFINITIONS}")
  endforeach()

  foreach (module ${PROTEST_DEPENDENCIES})
    set(MODULE_INCLUDE "$<TARGET_PROPERTY:${module},INCLUDE_DIRECTORIES>")
    set(MODULE_DEFINITION "$<TARGET_PROPERTY:${module},COMPILE_DEFINITIONS>")
    set(DEFINITIONS_COMPILER "$<$<BOOL:${MODULE_DEFINITION}>:-D$<JOIN:${MODULE_DEFINITION},;-D>>;${DEFINITIONS_COMPILER}")
    set(INCLUDES_PRECOMPILER "$<$<BOOL:${MODULE_INCLUDE}>:$<JOIN:${MODULE_INCLUDE},;>>;${INCLUDES_PRECOMPILER}")
  endforeach()

  foreach(pt ${PROTEST_SOURCES})
    get_filename_component(name ${pt} NAME_WE)
    get_filename_component(ext ${pt} EXT)

    add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${name}.pretest${ext}"
        COMMAND python3 ${PROTEST_MODULE_PATH}/ProtestPreCompiler.py
        --include-path "${Protest_INCLUDE_DIR}" "${INCLUDES_PRECOMPILER}" "${PROTEST_INCLUDES}"
        --src-file "${CMAKE_CURRENT_SOURCE_DIR}/${pt}" --output-file "${CMAKE_CURRENT_BINARY_DIR}/${name}.pretest${ext}"
        COMMAND_EXPAND_LISTS VERBATIM
        IMPLICIT_DEPENDS CXX "${CMAKE_CURRENT_SOURCE_DIR}/${pt}"
        # DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${pt}"
        )
        
      set(OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/${name}.protest${ext}")
      set(${PROTEST_TARGETS} ${${PROTEST_TARGETS}} "${OUTPUT_FILE}")
      set(${PROTEST_TARGETS} ${${PROTEST_TARGETS}} "${OUTPUT_FILE}" PARENT_SCOPE)
        
      add_custom_command(
          OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${name}.protest${ext}"
          COMMAND
            ${PROTEST_MODULE_PATH}/../../../../bin/protest-create-mocks
              "${CMAKE_CURRENT_BINARY_DIR}/${name}.pretest${ext}"
              -r ${PROTEST_PROJECT_ROOT}
              -o ${name}_mocks.hpp
               -- 
              -ferror-limit=1000
              --std=c++${CMAKE_CXX_STANDARD}
              -DPROTEST_COMPILE_STAGE
              ${DEFINITIONS}
              "${DEFINITIONS_COMPILER}" 2> /dev/null &&
            ${PROTEST_MODULE_PATH}/../../../../bin/protest-compiler
              "${CMAKE_CURRENT_BINARY_DIR}/${name}.pretest${ext}"
              -r ${PROTEST_PROJECT_ROOT}
              -o ${OUTPUT_FILE}
               --
              --std=c++${CMAKE_CXX_STANDARD} -DPROTEST_COMPILE_STAGE ${DEFINITIONS} "${DEFINITIONS_COMPILER}"
          COMMAND_EXPAND_LISTS VERBATIM
          DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${name}.pretest${ext}")
  endforeach()
endfunction()
