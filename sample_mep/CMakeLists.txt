cmake_minimum_required(VERSION 2.8)

set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{MEPSDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{MEPSDK}/share/mep.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define MEPSDK to point to your SDK path!")
  endif()
endif()

project(ime_sample ASM)
project(ime_sample C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11 -flto -fno-toplevel-reorder -mivc2 -mcop -mall-opts -mrepeat -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -flto -fno-toplevel-reorder -mivc2 -mcop -mall-opts -mrepeat -O3")

add_executable(${PROJECT_NAME}
  src/boot.c
  src/main.c
)

target_link_libraries(${PROJECT_NAME}
  gcc
)

set_target_properties(ime_sample
  PROPERTIES LINK_FLAGS "-nostdlib"
)
