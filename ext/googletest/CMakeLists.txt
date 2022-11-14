set(sources
  "googletest-1.8.1-fused/gmock-gtest-all.cc"
)

add_library(gtest STATIC ${sources})
target_include_directories(gtest PUBLIC ./googletest-1.8.1-fused)
