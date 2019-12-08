set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# CC AR
set (CMAKE_C_COMPILER "arm-none-eabi-gcc")
set (CMAKE_C_AR "arm-none-eabi-ar")
set (CMAKE_ASM_COMPILER "arm-none-eabi-gcc")
set (CMAKE_SIZE "arm-none-eabi-size" )
set (CMAKE_OBJCOPY "arm-none-eabi-objcopy")

# Set FLAGS
set(CPU "-mcpu=cortex-m7")
set(FPU "-mfpu=fpv5-sp-d16")
set(ABI "-mfloat-abi=softfp")
set(MCU "-D STM32F746xx -mthumb -fno-builtin ${CPU} ${FPU} ${ABI}")

# STDLIB
set (CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)
