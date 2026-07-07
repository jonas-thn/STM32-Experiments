cmake_minimum_required(VERSION 3.15)
project(Custom_Setup C ASM)

set(MCU_FLAGS "-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16")

set(CMAKE_C_FLAGS "${MCU_FLAGS} -Wall -Wextra -g -O0" CACHE INTERNAL "C Flags")
set(CMAKE_EXE_LINKER_FLAGS "--specs=nosys.specs" CACHE INTERNAL "Linker Flags")
set(CMAKE_ASM_FLAGS "${MCU_FLAGS} -g" CACHE INTERNAL "ASM Flags")

set(SOURCES
    src/main.c
)

add_executable(${PROJECT_NAME}.elf ${SOURCES})