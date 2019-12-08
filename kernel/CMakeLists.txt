# CMake minimum version
cmake_minimum_required (VERSION 3.5)

# Project Infomation
project (kernel)

# Reset output path
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

# Set include path
include_directories(${PROJECT_SOURCE_DIR}/core/include)
include_directories(${PROJECT_SOURCE_DIR}/cpu/include)
include_directories(${CMAKE_SOURCE_DIR}/${ARCH_INC})

# The need build source path and build all files
aux_source_directory(${PROJECT_SOURCE_DIR}/core CORE_SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/cpu PORT_SRC)

# CFLAGS
set (CMAKE_C_FLAGS "-g ${MCU} -Wall -Werror -std=gnu99 -ffunction-sections -fdata-sections -fomit-frame-pointer -mabi=aapcs -fno-unroll-loops -ffast-math -ftree-vectorize" CACHE INTERNAL "c compiler flags")

# ASM_FLAGS
set(CMAKE_ASM_FLAGS "-g ${MCU} -Wa,-mimplicit-it=thumb -x assembler-with-cpp" CACHE INTERNAL "asm compiler flags")

# Generate the target to the static library
add_library(slos_kernel ${CORE_SRC} ${PORT_SRC})
