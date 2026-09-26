######################################
###        RISC-V TOOLCHAIN        ###
###      Bare-metal/Embedded       ###
######################################

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv)

### -DRISCV_ARCH=rv32gc if needed ###
if (NOT RISCV_ARCH)
    set(RISCV_ARCH "rv64gc")
endif()
### -DRISCV_ABI=ilp32d if needed  ###
if (NOT RISCV_ABI)
    if (RISCV_ARCH MATCHES "rv32")
        set(RISCV_ABI "ilp32d")
    else()
        set(RISCV_ABI "lp64")
    endif()
endif()

#################################
# Common bare-metal toolchains: #
#  1. riscv64-unknown-elf       #
#  2. riscv64-elf               #
#  3. riscv64-none-elf          #
#################################
###      Auto Detection       ###
#################################
if (NOT TOOLCHAIN_PREFIX)
    find_program(RISCV_ELF_GCC riscv64-unknown-elf-gcc)
    if (RISCV_ELF_GCC)       ## Find *-unknown-* ##
        set(TOOLCHAIN_PREFIX "riscv64-unknown-elf")
    else()
        find_program(RISCV_ELF_GCC riscv64-elf-gcc)
        if (RISCV_ELF_GCC)   ## Find  *- -*      ##
            set(TOOLCHAIN_PREFIX "riscv64-elf")
        else()               ## Find  *-none-*   ##
            find_program(RISCV_ELF_GCC riscv64-none-elf-gcc)
            if (RISCV_ELF_GCC)
                set(RISCV_ELF_GCC "riscv64-none-elf")
            else()           ## Final fallback: riscv64-unknown-elf ##
                set(TOOLCHAIN_PREFIX "riscv64-unknown-elf")
                message(WARNING
                    "RISC-V ELF toolchain not found in PATH, using default: ${TOOLCHAIN_PREFIX}"
                )
            endif()
        endif()
    endif()
endif()

### Compiler ###
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}-gcc)

### Tell CMake not to run link tests ###
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

### Flags ###
set(COMMON_FLAGS "-march=${RISCV_ARCH} -mabi=${RISCV_ABI} -ffunction-sections -fdata-sections -nostdlib -nostartfiles")

set(CMAKE_C_FLAGS_INIT   "${COMMON_FLAGS}" )
set(CMAKE_CXX_FLAGS_INIT "${COMMON_FLAGS} -fno-exceptions -fno-rtti")
set(CMAKE_ASM_FLAGS_INIT "-march=${RISCV_ARCH} -mabi=${RISCV_ABI}")
set(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,-gc-sections -nostdlib -nostartfiles")

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

### Find path ###
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE NEVER)
