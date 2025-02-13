cmake_minimum_required(VERSION 3.24)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(szakdoga)

include_directories(src)

add_library(
  function
  src/Variable.cpp
  src/PowerPermutation.cpp
  src/math_utility.cpp
  # src/Polynomial.hpp
)

add_executable(
  main
  src/main.cpp
)
target_link_libraries(main function)

add_executable(
  basic
  src/basic_implementation.cpp
)


include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)

add_executable(
  function_test
  test/function_test.cpp
)
target_link_libraries(function_test GTest::gtest_main function)

gtest_discover_tests(function_test)
