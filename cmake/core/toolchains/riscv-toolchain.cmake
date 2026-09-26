######################################
######### RISC-V TOOLCHAIN  ##########
######################################

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR riscv64)

### -DRISCV_ARCH=rv32gc if needed ###
if (NOT RISCV_ARCH)
    set(RISCV_ARCH "rv64gc")
endif()
### -DRISCV_ABI=ilp32d if needed  ###
if (NOT RISCV_ABI)
    set(RISCV_ABI "lp64d")
endif()

if (RISCV_ARCH MATCHES "rv32")
    set(TOOLCHAIN_PREFIX riscv32-linux-gnu)
else()
    set(TOOLCHAIN_PREFIX riscv64-linux-gnu)
endif()

##########################################
#### Assign cross-compilers for riscv ####
##########################################
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}-gcc)

set(CMAKE_C_FLAGS_INIT   "-march=${RISCV_ARCH} -mabi=${RISCV_ABI}" )
set(CMAKE_CXX_FLAGS_INIT "-march=${RISCV_ARCH} -mabi=${RISCV_ABI}")
set(CMAKE_ASM_FLAGS_INIT "-march=${RISCV_ARCH} -mabi=${RISCV_ABI}")

execute_process(
    COMMAND ${CMAKE_C_COMPILER} -print-sysroot
    OUTPUT_VARIABLE TOOLCHAIN_SYSROOT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)

if (TOOLCHAIN_SYSROOT AND EXISTS ${TOOLCHAIN_SYSROOT})
    set(CMAKE_SYSROOT ${TOOLCHAIN_SYSROOT})
    message(STATUS "RISC-V sysroots: ${CMAKE_SYSROOT}")
else()
    ### Use a default risc-v toolchain path ###
    set(DEFAULT_SYSROOT "/usr/${TOOLCHAIN_PREFIX}")
    if (EXISTS ${DEFAULT_SYSROOT})
        SET(CMAKE_SYSROOT ${DEFAULT_SYSROOT})
        message(STATUS "RISC-V sysroot (default): ${CMAKE_SYSROOT}")
    else()
        message(WARNING "RISC-V sysroot not found at ${DEFAULT_SYSROOT}")
    endif()
endif()

###########################################
##### Assign other binutils for riscv #####
###########################################
set(CMAKE_AR        ${TOOLCHAIN_PREFIX}-ar)
set(CMAKE_LINKER    ${TOOLCHAIN_PREFIX}-ld)
set(CMAKE_NM        ${TOOLCHAIN_PREFIX}-nm)
set(CMAKE_OBJCOPY   ${TOOLCHAIN_PREFIX}-objcopy)
set(CMAKE_OBJDUMP   ${TOOLCHAIN_PREFIX}-objdump)
set(CMAKE_RANLIB    ${TOOLCHAIN_PREFIX}-ranlib)
set(CMAKE_SIZE      ${TOOLCHAIN_PREFIX}-size)
set(CMAKE_STRIP     ${TOOLCHAIN_PREFIX}-strip)

### Static Library ###
set(CMAKE_C_ARCHIVE_CREATE "<CMAKE_AR> qc  <TARGET> <LINK_FLAGS> <OBJECTS>")
set(CMAKE_C_ARCHIVE_APPEND "<CMAKE_AR> q   <TARGET> <LINK_FLAGS> <OBJECTS>")
set(CMAKE_C_ARCHIVE_FINISH "<CMAKE_RANLIB> <TARGET>")

set(CMAKE_CXX_ARCHIVE_CREATE "<CMAKE_AR> qc  <TARGET> <LINK_FLAGS> <OBJECTS>")
set(CMAKE_CXX_ARCHIVE_APPEND "<CMAKE_AR> q   <TARGET> <LINK_FLAGS> <OBJECTS>")
set(CMAKE_CXX_ARCHIVE_FINISH "<CMAKE_RANLIB> <TARGET>")

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
if (RISCV_ARCH MATCHES "rv32")
    find_program(QEMU_RISCV qemu-riscv32)
elseif(RISCV_ARCH MATCHES "rv64")
    find_program(QEMU_RISCV qemu-riscv64)
endif()

if (QEMU_RISCV AND CMAKE_SYSROOT)
    set(CMAKE_CROSSCOMPILING_EMULATOR ${QEMU_RISCV} -L ${CMAKE_SYSROOT})
    message(STATUS "Using QEMU Emulator for ${RISCV_ARCH}: ${CMAKE_CROSSCOMPILING_EMULATOR}")
endif()
