include_guard(GLOBAL)

###############################
#########Langs Toggle##########
###############################
option(ENABLE_ASM    "Enable language Assembly" OFF)
option(ENABLE_CUDA   "Enable language CUDA"     OFF)
option(ENABLE_PYTHON "Enable language Python"   OFF)
option(ENABLE_CYTHON "Enable Cython"            OFF)
option(ENABLE_NVML   "Enable nvidia-ml"         OFF)
option(ENABLE_PYTHON "Enable language Python"   OFF)

###############################
##########ASM Configs##########
###############################
if (ENABLE_ASM)
    ENABLE_LANGUAGE(ASM)
endif()

###############################
#########CUDA Configs##########
###############################
if (ENABLE_CUDA)
    if (ENABLE_NVML)
        # 1. Only find nvml
        # find_package(PkgConfig REQUIRED)
        # pkg_check_modules(NVML REQUIRED nvidia-ml)
        # 2. Find CUDA::nvml from CUDAToolkit
        find_package(CUDAToolkit REQUIRED COMPONENTS nvml)
        if (TARGET CUDA::nvml)
            set(NVML_FOUND TRUE CACHE INTERNAL "NVML found status")
            pretty_message(STATUS "NVML found successfully")
        else()
            set(NVML_FOUND FALSE CACHE INTERNAL "NVMl found status")
            pretty_message(FATAL_ERROR "ENABLE_NVML is toggled, but NVML not found")
        endif()
    endif()
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cuda")
    include(CUDAConfig)
    ENABLE_LANGUAGE(CUDA)
    include(CUDACompileOptions)
    cuda_config_module_verbose_info()
endif()

###############################
########Python Configs#########
###############################
if (ENABLE_PYTHON)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/python")
    include(PythonConfig)
    if (ENABLE_CYTHON)
        include(CythonConfig)
    endif()
endif()

###############################
######Enabled Langs Info#######
###############################
macro(enabled_langs_info)
    pretty_message(VINFO_BANNER "Enabled Language(s)" "=" ${BANNER_WIDTH})
    get_property(GLOBAL_ENABLED_LANGS GLOBAL PROPERTY ENABLED_LANGUAGES)
    string(REPLACE ";" ", " GLOBAL_ENABLED_LANGS_PRINT "${GLOBAL_ENABLED_LANGS}")
    pretty_message(STATUS "${GLOBAL_ENABLED_LANGS_PRINT}")
    pretty_message(VINFO_LINE "=" ${BANNER_WIDTH})
    pretty_message(STATUS "")
endmacro()

enabled_langs_info()
