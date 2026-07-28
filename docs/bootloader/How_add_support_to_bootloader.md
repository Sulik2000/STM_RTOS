# How add support of your microcontroller to bootloader

1. You need to write `.ld` script for your microcontroller in `linker/stm/stm32f411re` foler, which defines such variables as:
- `_estack` - this is top address of Stack memory
- `_begin_app` - this is origin address of `APP_CODE` region
- `_sidata` - it is `LOADADDR(.data)`, the address of memory from where default values of global values are loaded into RAM
- `_sbss` - it is beginning of `.bss` region in `.ld` script
- `_ebss` - it is the end of `.bss` region in `.ld` script

2. You need to write code which implements functions from `bootloader/Inc/drivers/*` headers for your concrete microcontroller and implement `startup.c` file(as example you can check `drivers/stm/stm32f411re/startup.c`), place this code in `bootloader/Src/drivers/{Vendor}/{Model}` folder.

3. In `bootloader/Src/load_fw.c` in function `void Load_FW_Flash_Start()` you need to change regions which are cleared, and in `bootloader/Inc/drivers/flash.h` you need add your version of `enum Flash_Region`

4. In fact, log system can be done not using DMA driver implementation, but then you need implement `log.c` as well in your driver, and I think good practice is to implement it