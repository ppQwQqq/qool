include_guard(GLOBAL)

set(CMAKE_SYSTEM_NAME Linux)

find_program(CMAKE_C_COMPILER clang)
find_program(CMAKE_CXX_COMPILER clang++)

find_program(CMAKE_CUDA_COMPILER clang++)

if (NOT CMAKE_C_COMPILER OR NOT CMAKE_CXX_COMPILER)
    pretty_message(FATAL "Could not find Clang compiler (clang/clang++).")
endif()

set(CMAKE_CUDA_FLAGS_INIT "--cuda-path=/opt/cuda --cuda-gpu-arch=sm_75")

# find_program(CMAKE_AR llvm-ar REQUIRED)
# find_program(CMAKE_RANLIB llvm-ranlib REQUIRED)
#
# # Install lld if not installed
# set(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS}    -fuse-ld=lld")
# set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=lld")
# set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -fuse-ld=ldd")
#
# # Install libc++ and libc++abi if not installed
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")

# Enable compiler-rt as sanitizers
# option(USE_COMPILER_RT "Use compiler-rt instead of libgcc" ON)
# if (USE_COMPILER_RT)
#     set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -rtlib=compiler-rt")
# endif()
#
# option(USE_LLVM_UNWIND "Use LLVM libunwind" OFF)
# if (USE_LLVM_UNWIND)
#     set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lunwind")
# endif()
