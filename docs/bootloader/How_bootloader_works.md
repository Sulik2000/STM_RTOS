# How bootloader works
1. Cause it is low-level hardware project, the first of all executes initialization of DATA and BSS memory regions.

2. In start bootloader checks if bootloader status pin(for different microcontrollers it is different, in case STM32F411RE it is PA5) is HIGH
- If it's HIGH then bootloader launchs procedure of updating app code with `Load_FW_Flash_Start()`. This function uses Flash drivers and Log system which logic are platform dependent.

    1. It sends over Log system `0xDD 0xDD`, this gives for Python script signal that microcontroller are ready to get bytes of binary code and write it into the `APP_CODE` region, which are defined in `.ld` platform dependent script

    2. When chunks of code are written, Log system again sends `0xDD 0xDD` for new chunks of code

    3. When code are finished, Python script sends `0xFF 0xFF 0xDD 0xDD` bytes which mean that code ended. Bootloader over Log system sends same bytes to give signal to Python script that everything is OK.

- If it's LOW, then bootloader sets Vector Table Offset to the beginning of `APP_CODE` region, loads address of Main Stack Pointer and performs launching of `APP_CODE` function `Reset_Handler`
