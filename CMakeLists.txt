cmake_minimum_required(VERSION 3.22)

project(RTOS C CXX ASM)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_subdirectory(bootloader)
add_subdirectory(rtos)