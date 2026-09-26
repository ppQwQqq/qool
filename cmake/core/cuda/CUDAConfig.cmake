include_guard(GLOBAL)

set(CMAKE_CUDA_STANDARD 20)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_SEPARABLE_COMPILATION ON)
set(AUTO_RESET_CUDA_ARCH
    ON CACHE BOOL
    "Automatically reset CUDA architectures"
)

set(_NEED_RESET_ARCH FALSE)
if (NOT DEFINED CMAKE_CUDA_ARCHITECTURES
    OR CMAKE_CUDA_ARCHITECTURES STREQUAL "")
    set(_NEED_RESET_ARCH TRUE)
else()
    list(GET CMAKE_CUDA_ARCHITECTURES 0 _FIRST_ARCH)
    if (_FIRST_ARCH MATCHES "^[0-9]+$" AND _FIRST_ARCH LESS 70)
        set(_NEED_RESET_ARCH TRUE)
    endif()
endif()

if (_NEED_RESET_ARCH)
    pretty_message(OPTIONAL "Current CMAKE_CUDA_ARCHITECTURES is less then 70: ${CMAKE_CUDA_ARCHITECTURES}")
    pretty_message(OPTIONAL "Now try to reset CMAKE_CUDA_ARCHITECTURES to native")
    pretty_message(TIPS     "Use -DAUTO_RESET_CUDA_ARCH=OFF to disable automatically reset")
    if (AUTO_RESET_CUDA_ARCH)
        if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.24)
            set(CMAKE_CUDA_ARCHITECTURES native)
        else()
            find_program(NVCC_DEVICE_QUERY "__nvcc_device_query")
            if (NVCC_DEVICE_QUERY)
                execute_process(
                    COMMAND ${NVCC_DEVICE_QUERY}
                    RESULT_VARIABLE NVCC_DEVICE_QUERY_RESULT
                    OUTPUT_VARIABLE DETECTED_ARCHITECTURES
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                    ERROR_QUIET
                )
                if (NVCC_DEVICE_QUERY_RESULT EQUAL 0 AND DETECTED_ARCHITECTURES)
                    pretty_message(SUCCESS "Found valid __nvcc_device_query to query CUDA architectures: ${DETECTED_ARCHITECTURES}")
                    set(CMAKE_CUDA_ARCHITECTURES ${DETECTED_ARCHITECTURES})
                else()
                    pretty_message(ERROR "Failed to query valid CUDA architecture(use default architectures)")
                    set(CMAKE_CUDA_ARCHITECTURES "75;80;86")
                endif()
            else()
                pretty_message(ERROR "Failed to find valid __nvcc_device_query to query CUDA architecture(use default architectures)")
                set(CMAKE_CUDA_ARCHITECTURES "75;80;86")
            endif()
        endif()
    endif()
endif()

function(cuda_config_module_verbose_info)
    pretty_message(DEBUG "CUDAConfig.cmake module loaded.")
    pretty_message(VINFO_BANNER "CUDA Tool Kit" "=" ${BANNER_WIDTH})
    pretty_message_kv(VINFO "CUDA Standard"              "${CMAKE_CUDA_STANDARD}")
    pretty_message_kv(VINFO "CUDA Separable Compilation" "${CMAKE_CUDA_SEPARABLE_COMPILATION}")
    pretty_message_kv(VINFO "CUDA Architectures"         "${CMAKE_CUDA_ARCHITECTURES}")
    pretty_message_kv(VINFO "CUDA NVML"                  "${ENABLE_NVML}")
    pretty_message(VINFO_LINE "=" ${BANNER_WIDTH})
    pretty_message(STATUS "")
endfunction()
