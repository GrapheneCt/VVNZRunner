cmake_minimum_required(VERSION 3.19)

set(CMAKE_TOOLCHAIN_FILE "$ENV{SCE_PSP2_SDK_DIR}/host_tools/build/cmake/psp2-snc-toolchain.cmake")

project(VVNZRunner)

include(VitaDevelopmentSuite)
enable_language(EMD)
set(CMAKE_C_STANDARD 99)

add_definitions(-DVVNZRUNNER_BUILD)

add_compile_options(
  -Xdiag=0 -Xquit=2 -O3
)

add_link_options(
  --prx-no-runtime-support
  --strip-unused-data
  --strip-duplicates
  --strip-report=strip-report.txt
)

include_directories(
  ${VDSUITE_KERNEL_INCLUDE_DIRECTORIES}
)

link_directories(
  ${VDSUITE_LIBRARY_DIRECTORIES}
)

VDSuiteCreateStubs(imports_lowio ScePervasiveForDriver.emd
  LIBRARIES
    ScePervasiveForDriver_stub
)

VDSuiteCreateStubs(imports_venezia SceCodecEngineWrapperForDriver.emd
  LIBRARIES
    SceCodecEngineWrapperForDriver_stub
)

VDSuiteCreateStubs(imports_sysroot SceSysrootForDriver.emd
  LIBRARIES
    SceSysrootForDriver_stub
)

VDSuiteCreateStubs(imports_sysmem SceDebugLedForDriver.emd
  LIBRARIES
    SceDebugLedForDriver_stub
)

VDSuiteCreateStubs(vitasdk_stubs lib.emd
  LIBRARIES
    VVNZRunner_vitasdk_stub
  VitasdkStub
)

VDSuiteCreateStubs(stubs lib.emd
  LIBRARIES
    VVNZRunner_stub
)

add_library("${PROJECT_NAME}.elf" MODULE
  main.c
  memory.c
  codec_engine_bridge.c
  lib.emd
)

target_link_libraries("${PROJECT_NAME}.elf"
  SceDebugForDriver_stub
  SceSysmemForDriver_stub
  SceThreadmgrForDriver_stub
  SceSysclibForDriver_stub
  SceProcEventForDriver_stub
  SceCpuForDriver_stub
  SceIofilemgrForDriver_stub
  taihenModuleUtils_stub
  taihenForKernel_stub
  "${CMAKE_CURRENT_BINARY_DIR}/imports_lowio/libScePervasiveForDriver_stub.a"
  "${CMAKE_CURRENT_BINARY_DIR}/imports_sysroot/libSceSysrootForDriver_stub.a"
  "${CMAKE_CURRENT_BINARY_DIR}/imports_venezia/libSceCodecEngineWrapperForDriver_stub.a"
  "${CMAKE_CURRENT_BINARY_DIR}/imports_sysmem/libSceDebugLedForDriver_stub.a"
)

VDSuiteSignElf("${PROJECT_NAME}.skprx" "${PROJECT_NAME}.elf" COMPRESS ON STRIP ON)