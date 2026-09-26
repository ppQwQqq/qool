cmake_minimum_required(VERSION 3.16)

list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_LIST_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}/core"
    "${CMAKE_CURRENT_LIST_DIR}/core/toolchains"
    "${CMAKE_CURRENT_LIST_DIR}/deps"
    "${CMAKE_CURRENT_LIST_DIR}/misc"
    "${CMAKE_CURRENT_LIST_DIR}/test"
)

include(PrettyPrint)

# core
include(ProjectVerbose)
include(ProjectInfo)
include(PCH)
include(CompileOptions)

include(BuildConfig)
include(LangConfig)
