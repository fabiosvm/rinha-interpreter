
cmake_minimum_required(VERSION 3.15)

project(rinha LANGUAGES C VERSION 1.0.0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

if(MSVC)
  add_compile_options(/W4 /WX /02)
else()
  add_compile_options(-Wall -Wextra -Wpedantic -Werror -O3)
  add_compile_options("$<$<CONFIG:Debug>:-fsanitize=address>")
  add_link_options("$<$<CONFIG:Debug>:-fsanitize=address>")
endif()

if(NOT APPLE)
  add_link_options("$<$<CONFIG:Release>:-static>")
endif()

add_executable(${PROJECT_NAME}
  src/array.c
  src/callstack.c
  src/chunk.c
  src/closure.c
  src/compiler.c
  src/dump.c
  src/function.c
  src/main.c
  src/memory.c
  src/result.c
  src/scanner.c
  src/stack.c
  src/str.c
  src/tuple.c
  src/value.c
  src/vm.c)
