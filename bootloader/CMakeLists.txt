project(STM_bootloader C CXX ASM)

add_executable(STM_bootloader)

target_link_options(STM_bootloader PRIVATE -T${CMAKE_SOURCE_DIR}/bootloader/STM32F411XX_BOOTLOADER.ld)

target_sources(STM_bootloader PRIVATE
    Src/startup.c
    Src/main.c
    Src/isr_routine.c
    Src/drivers/usart2.c
    Src/log.c
    Src/drivers/dma.c
    Src/string.c
    Src/drivers/flash.c
    Src/load_fw.c
)

target_include_directories(STM_bootloader PRIVATE
    Inc
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Include
)

target_compile_definitions(STM_bootloader PRIVATE
    # Add user defined symbols
    STM32F411xE
)
