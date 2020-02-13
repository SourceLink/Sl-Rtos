# CMake minimum version
cmake_minimum_required (VERSION 3.5)

if(NOT DEFINED BOARD_FAMILY)
    message(FATAL_ERROR "Please specify BOARD_FAMILY, eg cmake -DBOARD_FAMILY=NUCLEO_STM32F746ZG")
endif()

# 选择编译配置
if(${BOARD_FAMILY} MATCHES NUCLEO_STM32F746ZG)
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/gcc_stm32f7.cmake")
endif()

# Project Infomation
project(Slos)

# enable asm
enable_language(ASM)


# arch 需要选择
if(${BOARD_FAMILY} MATCHES NUCLEO_STM32F746ZG)
    set(ARCH_PATH "arch/arm/arm-v7m")
    set(ARCH_INC "arch/arm/arm-v7m/corterxm-common/gcc")
endif()

# board 需要选择 链接文件, 启动文件
if(${BOARD_FAMILY} MATCHES NUCLEO_STM32F746ZG)
    set(BOARD_LDSCRIPT "/board/${BOARD_FAMILY}/ldscripts/STM32F746ZGTx_FLASH.ld")
    set(STARTUP_SRC "/platform/st/CMSIS/Device/ST/STM32F7xx/Source/Templates/gcc/startup_stm32f746xx.s")
endif()


add_subdirectory(${ARCH_PATH})
add_subdirectory(board/${BOARD_FAMILY})
add_subdirectory(kernel)
add_subdirectory(demo/sched_lock)