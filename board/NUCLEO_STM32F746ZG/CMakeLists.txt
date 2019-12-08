# CMake minimum version
cmake_minimum_required (VERSION 3.5)

# Project Infomation
project (board)

# Reset output path
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

# Set include path
include_directories(include)
include_directories(${PROJECT_SOURCE_DIR}/../../platform/st/CMSIS/Device/ST/STM32F7xx/Include)
include_directories(${PROJECT_SOURCE_DIR}/../../platform/st/CMSIS/Include)

# The need build source path and build all files
aux_source_directory(. BOARD_SRC)

# CFLAGS
set(CMAKE_C_FLAGS "-g  ${MCU} -Wall -Werror -std=gnu99 -ffunction-sections -fdata-sections -fomit-frame-pointer -mabi=aapcs -fno-unroll-loops -ffast-math -ftree-vectorize" CACHE INTERNAL "c compiler flags")

# CXXFLAGS
set(CMAKE_CXX_FLAGS "-g ${MCU} -Wall -Werror -std=c++11 -ffunction-sections -fdata-sections -fomit-frame-pointer -mabi=aapcs -fno-unroll-loops -ffast-math -ftree-vectorize" CACHE INTERNAL "cxx compiler flags")

# ASM_FLAGS
set(CMAKE_ASM_FLAGS "-g  ${MCU} -Wa,-mimplicit-it=thumb  -x assembler-with-cpp" CACHE INTERNAL "asm compiler flags")

# Generate the target to the static library
add_library(board ${BOARD_SRC})

