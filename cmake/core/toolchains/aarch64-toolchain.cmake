######################################
######### RISC-V TOOLCHAIN  ##########
######################################

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)
set(TOOLCHAIN_PREFIX aarch64-linux-gnu)

##########################################
### Assign cross-compilers for aarch64 ###
##########################################
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}-gcc)

execute_process(
    COMMAND ${CMAKE_C_COMPILER} -print-sysroot
    OUTPUT_VARIABLE TOOLCHAIN_SYSROOT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)

if (TOOLCHAIN_SYSROOT AND EXISTS ${TOOLCHAIN_SYSROOT})
    set(CMAKE_SYSROOT ${TOOLCHAIN_SYSROOT})
    message(STATUS "AArch64 sysroots: ${CMAKE_SYSROOT}")
else()
    ### Use a default aarch64 toolchain path ###
    set(DEFAULT_SYSROOT "/usr/${TOOLCHAIN_PREFIX}")
    if (EXISTS ${DEFAULT_SYSROOT})
        SET(CMAKE_SYSROOT ${DEFAULT_SYSROOT})
        message(STATUS "AArch64 sysroot (default): ${CMAKE_SYSROOT}")
    else()
        message(WARNING "AArch64 sysroot not found at ${DEFAULT_SYSROOT}")
    endif()
endif()

###########################################
#### Assign other binutils for aarch64 ####
###########################################
set(CMAKE_AR        ${TOOLCHAIN_PREFIX}-ar)
set(CMAKE_LINKER    ${TOOLCHAIN_PREFIX}-ld)
set(CMAKE_NM        ${TOOLCHAIN_PREFIX}-nm)
set(CMAKE_OBJCOPY   ${TOOLCHAIN_PREFIX}-objcopy)
set(CMAKE_OBJDUMP   ${TOOLCHAIN_PREFIX}-objdump)
set(CMAKE_RANLIB    ${TOOLCHAIN_PREFIX}-ranlib)
set(CMAKE_SIZE      ${TOOLCHAIN_PREFIX}-size)
set(CMAKE_STRIP     ${TOOLCHAIN_PREFIX}-strip)

######################################
###### Set find-path in sysroot ######
######################################
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
list(APPEND CMAKE_FIND_ROOT_PATH ${CMAKE_SYSROOT})

#######################################
######### QEMU EMULATOR SETUP #########
#######################################
find_program(QEMU_AARCH64 qemu-aarch64)

if (QEMU_AARCH64 AND CMAKE_SYSROOT)
    set(CMAKE_CROSSCOMPILING_EMULATOR ${QEMU_AARCH64} -L ${CMAKE_SYSROOT})
    message(STATUS "Using QEMU Emulator for AArch64: ${CMAKE_CROSSCOMPILING_EMULATOR}")
endif()
