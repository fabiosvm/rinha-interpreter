
cmake_minimum_required(VERSION 3.15)

project(rinha LANGUAGES C VERSION 1.0.0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

if(MSVC)
  add_compile_options(/W4 /WX)
  set(CMAKE_C_FLAGS_RELEASE "/O2")
else()
  add_compile_options(-Wall -Wextra -Wpedantic -Werror)
  set(CMAKE_C_FLAGS_RELEASE "-O3")
endif()

if(NOT APPLE)
  set(CMAKE_EXE_LINKER_FLAGS "-static")
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
