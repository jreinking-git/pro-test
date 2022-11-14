set(sources
  "reliable_broadcast.cpp"
)

add_library(sut STATIC ${sources})
target_include_directories(sut PUBLIC .)
target_link_libraries(sut protest::rtos)