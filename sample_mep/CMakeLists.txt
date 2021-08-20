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

project(mep_sample ASM)
project(mep_sample C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(${PROJECT_NAME}
  src/main.c
)

target_link_libraries(${PROJECT_NAME}
)

set_target_properties(mep_sample
  PROPERTIES LINK_FLAGS "-nostdlib"
)
