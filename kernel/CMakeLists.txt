# CMake minimum version
cmake_minimum_required (VERSION 3.1)

# Project Infomation
project (kernel)

enable_language(ASM)

set(CMAKE_SYSTEM Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Reset output path
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

# Set include path
include_directories(include)
include_directories(${PROJECT_SOURCE_DIR}/../arch/arm/arm-v7m/corterx-m7/gcc)
include_directories(${PROJECT_SOURCE_DIR}/../arch/arm/arm-v7m/common/include)

# The need build source path and build all files
aux_source_directory(. KERNEL_SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/../arch/arm/arm-v7m/corterx-m7/gcc ARCH_PORT_SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/../arch/arm/arm-v7m/common ARCH_COMMON_SRC)

# CC AR
set(CMAKE_C_COMPILER "arm-none-eabi-gcc")
set(CMAKE_C_AR "arm-none-eabi-ar")
set(CMAKE_ASM_COMPILER "arm-none-eabi-gcc")


# Set FLAGS
set(CPU "-mcpu=cortex-m7")
set(FPU "-mfpu=fpv5-sp-d16")
set(ABI "-mfloat-abi=softfp")
set(MCU "-mthumb -fno-builtin ${CPU} ${FPU} ${ABI}")

# CFLAGS
set (CMAKE_C_FLAGS "-g ${MCU} -Wa,-mimplicit-it=thumb -Wall -Werror -std=gnu99 -ffunction-sections -fdata-sections -fomit-frame-pointer -mabi=aapcs -fno-unroll-loops -ffast-math -ftree-vectorize" CACHE INTERNAL "c compiler flags")

# ASM_FLAGS
set(CMAKE_ASM_FLAGS "-g ${MCU} -Wa,-mimplicit-it=thumb -x assembler-with-cpp" CACHE INTERNAL "asm compiler flags")

# Generate the target to the static library
add_library(slos_kernel ${KERNEL_SRC} ${ARCH_PORT_SRC} ${ARCH_COMMON_SRC} ${PROJECT_SOURCE_DIR}/../arch/arm/arm-v7m/corterx-m7/gcc/sl_port_s.s)

# Make clean-all
# add_custom_target (clean-all COMMAND rm -rf ${CMAKE_BINARY_DIR}/*)