# CMake minimum version
cmake_minimum_required (VERSION 3.1)

# Project Infomation
project( slos_app )

set(MY_TARGET slos_app)

# enable asm
enable_language(ASM)

# Reset output path
set (EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set (LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

# STDLIB
set (CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)

# add_subdirectory(Drivers)
add_subdirectory(kernel)


# Set include path
include_directories(${PROJECT_SOURCE_DIR}/platform/st/CMSIS/Device/ST/STM32F7xx/Include)
include_directories(${PROJECT_SOURCE_DIR}/platform/st/CMSIS/Include)
# include_directories(${PROJECT_SOURCE_DIR}/platform/st/STM32F7xx_HAL_Driver/Inc)
include_directories(${PROJECT_SOURCE_DIR}/kernel/include)
include_directories(${PROJECT_SOURCE_DIR}/arch/arm/arm-v7m/corterx-m7/gcc)
include_directories(${PROJECT_SOURCE_DIR}/arch/arm/arm-v7m/common/include)
include_directories(${PROJECT_SOURCE_DIR}/board/NUCLEO_STM32F746ZG/include)

# The need build source path and build all files
# aux_source_directory (${PROJECT_SOURCE_DIR}/platform/st/CMSIS/Device/ST/STM32F7xx/Source/Templates STM32F7XX_SRC)
aux_source_directory (${PROJECT_SOURCE_DIR}/board/NUCLEO_STM32F746ZG/src BOARD_SRC)


# CC AR LD AS
set (CMAKE_C_COMPILER "arm-none-eabi-gcc")
set (CMAKE_C_AR "arm-none-eabi-ar")
set (CMAKE_ASM_COMPILER "arm-none-eabi-gcc")
set (CMAKE_SIZE "arm-none-eabi-size" )
set (CMAKE_OBJCOPY "arm-none-eabi-objcopy")


# Set FLAGS
set(CPU "-mcpu=cortex-m7")
set(FPU "-mfpu=fpv5-sp-d16")
set(ABI "-mfloat-abi=softfp")
set(MCU "-mthumb -fno-builtin ${CPU} ${FPU} ${ABI}")

# CFLAGS
set (CMAKE_C_FLAGS "-g ${MCU} -Wall -Werror -std=gnu99 -ffunction-sections -fdata-sections -fomit-frame-pointer -mabi=aapcs -fno-unroll-loops -ffast-math -ftree-vectorize" CACHE INTERNAL "c compiler flags")

# set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D USE_HAL_DRIVER -D STM32F746xx")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D STM32F746xx")

# CXXFLAGS
set (CMAKE_CXX_FLAGS "-g ${MCU} -Wall -Werror -std=c++11 -ffunction-sections -fdata-sections -fomit-frame-pointer -mabi=aapcs -fno-unroll-loops -ffast-math -ftree-vectorize" CACHE INTERNAL "cxx compiler flags")

# ASMFLAGS
set (CMAKE_ASM_FLAGS "-g ${MCU} -Wa,-mimplicit-it=thumb -x assembler-with-cpp" CACHE INTERNAL "asm compiler flags")

# LDFLAGS
set (LDSCRIPT "${PROJECT_SOURCE_DIR}/board/NUCLEO_STM32F746ZG/ldscripts/STM32F746ZGTx_FLASH.ld")
set (CMAKE_EXE_LINKER_FLAGS "-specs=nano.specs -specs=nosys.specs -T${LDSCRIPT} -Wl,-Map=${PROJECT_BINARY_DIR}/${MY_TARGET}.map,--cref -Wl,--gc-sections")

# Generate the target
add_executable (${CMAKE_PROJECT_NAME}.elf ${STM32F7XX_SRC} ${BOARD_SRC} ${PROJECT_SOURCE_DIR}/platform/st/CMSIS/Device/ST/STM32F7xx/Source/Templates/gcc/startup_stm32f746xx.s)

# Link the library to the target
target_link_libraries (${CMAKE_PROJECT_NAME}.elf slos_kernel)

# Generate the binary file
add_custom_target (${CMAKE_PROJECT_NAME}.bin ALL ${CMAKE_OBJCOPY} -Obinary "${EXECUTABLE_OUTPUT_PATH}/${CMAKE_PROJECT_NAME}.elf" "${EXECUTABLE_OUTPUT_PATH}/${CMAKE_PROJECT_NAME}.bin" DEPENDS ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_PROJECT_NAME}.elf)

# Generate the hex file
add_custom_target (${CMAKE_PROJECT_NAME}.hex ALL ${CMAKE_OBJCOPY} -Oihex "${EXECUTABLE_OUTPUT_PATH}/${CMAKE_PROJECT_NAME}.elf" "${EXECUTABLE_OUTPUT_PATH}/${CMAKE_PROJECT_NAME}.hex" DEPENDS ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_PROJECT_NAME}.elf)

# Echo the size Infomation
add_custom_target (size ALL ${CMAKE_SIZE} "${EXECUTABLE_OUTPUT_PATH}/${CMAKE_PROJECT_NAME}.elf" DEPENDS ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_PROJECT_NAME}.elf)

# Make flash to the board by st-link
add_custom_target (flash COMMAND st-flash write ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_PROJECT_NAME}.bin 0x8000000)

# Make clean-all
add_custom_target (clean-all COMMAND rm -rf ${CMAKE_BINARY_DIR}/*)