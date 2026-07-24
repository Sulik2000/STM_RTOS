
_estack = ORIGIN(RAM) + LENGTH(RAM); /* end of RAM, start of stack */
_begin_app = ORIGIN(APP_CODE); /* start of application code */

MEMORY {
  FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 16K /* This occupies the first region of flash memory for the bootloader */
  APP_CODE (xrw) : ORIGIN = 0x08004000, LENGTH = 496K
  RAM (xrw) : ORIGIN = 0x20000000, LENGTH = 128K
}

SECTIONS {
  .text : {
    KEEP(*(.isr_vector))
    *(.text*)
    *(.rodata*)
  } > FLASH

  .data : {
    _sdata = .;
    *(.data*)
    _edata = .;
  } > RAM AT > FLASH

  _sidata = LOADADDR(.data);

  . = ALIGN(4);

  .bss : {
    _sbss = .;
    *(.bss*)
    *(COMMON)
    _ebss = .;
  } > RAM
}