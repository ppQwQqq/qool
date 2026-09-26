##########################
###CUDA Compile Options###
##########################
include_guard(GLOBAL)

add_library(cuda_options INTERFACE)

target_include_directories(cuda_options
    INTERFACE
    # If needed, uncomment it, but suggest only including for specific target #
    # 1. Only find nvml
    # $<IF$<BOOL:${NVML_FOUND}>,${NVML_INCLUDE_DIRS},>
    # 2. Find CUDA::nvml from CUDAToolkit
    # $<IF$<BOOL:${NVML_FOUND}>,${CUDA_TOOLKIT_INCLUDE_DIRECTORIES},>
)

target_link_libraries(cuda_options
    INTERFACE
    # If needed, uncomment it, but suggest only linking for specific target #
    # 1. Only find nvml
    #$<IF$<BOOL:${NVML_FOUND}>,${NVML_LIBRARIES},>
    # 2. Find CUDA::nvml from CUDAToolkit
    #$<IF$<BOOL:${NVML_FOUND}>,CUDA::nvmwl,>
)

target_compile_options(cuda_options
    INTERFACE
    $<$<COMPILE_LANGUAGE:CUDA>:
        --expt-relaxed-constexpr
        --expt-extended-lambda
        -Xcompiler=-Wall
        # -O3
        # -use_fast_math
        # -Xptxas=-v
        # -arch=native
        # --generate-line-info
        # -Wno-deprecated-gpu-targets
    >
)
