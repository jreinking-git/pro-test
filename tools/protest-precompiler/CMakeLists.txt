cmake_minimum_required(VERSION 3.19)

install(FILES
  ${CMAKE_CURRENT_LIST_DIR}/ProtestPreCompiler.py
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Protest/modules
)

install(FILES
  ${CMAKE_CURRENT_LIST_DIR}/ProtestSutPreCompiler.py
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Protest/modules
)