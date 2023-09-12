
cmake_minimum_required(VERSION 3.15)

project(rinha)

set(CMAKE_C_STANDARD 11)

if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -Wpedantic -Werror)
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
