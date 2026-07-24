MEMORY {
  FLASH (rx) : ORIGIN = 0x08004000, LENGTH = 496K /* This occupies the first region of flash memory for the bootloader */
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