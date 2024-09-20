cmake_minimum_required(VERSION 3.24)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(szakdoga)
add_executable(
  main
  src/main.cpp
  src/Context.cpp
  src/glad.c
  src/GLGraphics.cpp
  src/Variable.cpp
)
include_directories(src)
target_link_libraries(main glfw)

set_property (TARGET main PROPERTY CXX_STANDARD 20)
