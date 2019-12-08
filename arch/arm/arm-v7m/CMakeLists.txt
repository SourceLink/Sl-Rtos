# CMake minimum version
cmake_minimum_required (VERSION 3.5)

# Project Infomation
project(arch)

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

# Set include path
# include_directories(include)

# The need build source path and build all files
aux_source_directory(${PROJECT_SOURCE_DIR}/corterxm-common/gcc ARCH_SRC)

# CFLAGS
set(CMAKE_C_FLAGS "-g  ${MCU} -Wall -Werror -std=gnu99 -ffunction-sections -fdata-sections -fomit-frame-pointer -mabi=aapcs -fno-unroll-loops -ffast-math -ftree-vectorize" CACHE INTERNAL "c compiler flags")

# ASM_FLAGS
set(CMAKE_ASM_FLAGS "-g  ${MCU} -Wa,-mimplicit-it=thumb  -x assembler-with-cpp" CACHE INTERNAL "asm compiler flags")

# Generate the target to the static library
add_library(arch ${ARCH_SRC} ${PROJECT_SOURCE_DIR}/corterxm-common/gcc/port_s.s)
