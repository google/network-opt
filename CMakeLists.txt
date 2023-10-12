cmake_minimum_required(VERSION 3.25.1)
project(NetworkOpt)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_subdirectory(external/abseil-cpp)
add_subdirectory(external/googletest)
add_executable(network_opt
  src/main.cc
  src/network_opt.cc
)
target_link_libraries(network_opt
  absl::flags_parse
  absl::statusor
)

enable_testing()

add_executable(network_opt_tests
  tests/network_opt_tests.cc
  src/network_opt_utils.cc
)
target_link_libraries(network_opt_tests
  GTest::gmock_main
  GTest::gtest_main
  absl::flags
  absl::statusor
)
add_test(NAME network_opt_tests COMMAND network_opt_tests)

include(GoogleTest)
