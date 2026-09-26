cmake_minimum_required(VERSION 4.3.0)

#############################################
#                                           #
#         Name:    Qool                     #
#         Version: 0.0.1                    #
#                                           #
#         Author:  ppQwQqq                  #
#                                           #
#############################################
project(Qool LANGUAGES C CXX)

set(CMAKE_PROJECT_VERSION 0.0.1)
set(CMAKE_C_STANDARD 26)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_TESTS "Build tests" OFF)

if (BUILD_TESTS AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Set debug build type for more debug info" FORCE)
endif()

include(cmake/Config.cmake)

if (BUILD_TESTS)
    include(CTest)
    enable_testing()
endif()

if (BUILD_TESTS)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
endif()

include(GNUInstallDirs)
