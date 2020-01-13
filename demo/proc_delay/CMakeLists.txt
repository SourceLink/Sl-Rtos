# CMake minimum version
cmake_minimum_required (VERSION 3.5)

# Project Infomation
project( demo_proc_delay )

# Reset output path
set (EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set (LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)


# Set include path
include_directories(${CMAKE_SOURCE_DIR}/arch/arm/arm-v7m/corterxm-common/gcc)
include_directories(${CMAKE_SOURCE_DIR}/board/${BOARD_FAMILY}/include)
include_directories(${CMAKE_SOURCE_DIR}/kernel/core/include)
include_directories(${CMAKE_SOURCE_DIR}/kernel/cpu/include)
include_directories(${CMAKE_SOURCE_DIR}/platform/st/CMSIS/Device/ST/STM32F7xx/Include)
include_directories(${CMAKE_SOURCE_DIR}/platform/st/CMSIS/Include)
# include_directories(${PROJECT_SOURCE_DIR}/platform/st/STM32F7xx_HAL_Driver/Inc)


# The need build source path and build all files
# aux_source_directory (${PROJECT_SOURCE_DIR}/platform/st/CMSIS/Device/ST/STM32F7xx/Source/Templates STM32F7XX_SRC)
aux_source_directory (. SOURCE_SRC)


# CFLAGS
set (CMAKE_C_FLAGS "-g ${MCU} -Wall -Werror -std=gnu99 -ffunction-sections -fdata-sections -fomit-frame-pointer -mabi=aapcs -fno-unroll-loops -ffast-math -ftree-vectorize" CACHE INTERNAL "c compiler flags")

# CXXFLAGS
set (CMAKE_CXX_FLAGS "-g ${MCU} -Wall -Werror -std=c++11 -ffunction-sections -fdata-sections -fomit-frame-pointer -mabi=aapcs -fno-unroll-loops -ffast-math -ftree-vectorize" CACHE INTERNAL "cxx compiler flags")

# ASMFLAGS
set (CMAKE_ASM_FLAGS "-g ${MCU} -Wa,-mimplicit-it=thumb -x assembler-with-cpp" CACHE INTERNAL "asm compiler flags")

# LDFLAGS
set (LDSCRIPT "${CMAKE_SOURCE_DIR}/${BOARD_LDSCRIPT}")
set (CMAKE_EXE_LINKER_FLAGS "-specs=nano.specs -specs=nosys.specs -T${LDSCRIPT} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map,--cref -Wl,--gc-sections")

# Generate the target
add_executable (${PROJECT_NAME}.elf ${CMAKE_SOURCE_DIR}/${STARTUP_SRC} ${STM32F7XX_SRC} ${SOURCE_SRC} )

# Link the library to the target
target_link_libraries (${PROJECT_NAME}.elf slos_kernel board arch)


################################################################################################################
# Generate the binary file
add_custom_target (${PROJECT_NAME}.bin ALL ${CMAKE_OBJCOPY} -Obinary "${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.elf" "${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.bin" DEPENDS ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.elf)

# Generate the hex file
add_custom_target (${PROJECT_NAME}.hex ALL ${CMAKE_OBJCOPY} -Oihex "${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.elf" "${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.hex" DEPENDS ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.elf)

# Echo the size Infomation
add_custom_target (size ALL ${CMAKE_SIZE} "${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.elf" DEPENDS ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.elf)

# Make flash to the board by st-link
add_custom_target (flash COMMAND st-flash write ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.bin 0x8000000)

# Make clean-all
add_custom_target (clean-all COMMAND rm -rf ${CMAKE_BINARY_DIR}/*)
