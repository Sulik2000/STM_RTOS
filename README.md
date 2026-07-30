# Custom Bare-Metal STM32F411RE Bootloader

![Platform](https://img.shields.io/badge/Platform-STM32F411RE-blue.svg)
![Architecture](https://img.shields.io/badge/Architecture-ARM%20Cortex--M4-orange.svg)
![Toolchain](https://img.shields.io/badge/Toolchain-Arm%20GNU%20Toolchain%20%2F%20CMSIS--Build-green.svg)
![License](https://img.shields.io/badge/License-MIT-lightgrey.svg)

A bare-metal, custom bootloader written completely from scratch for the **STM32F411RE** microcontroller (NUCLEO-F411RE board). 

This project was built without relying on high-level HAL/LL libraries, pre-packaged vendor linker scripts, or pre-configured NVIC vector tables. Everything—from the memory layout and startup code to peripheral drivers (UART, DMA, Internal Flash) and the host-side UART Python loading script—was implemented manually to demonstrate deep low-level firmware architecture skills.

---

## 🌟 Highlights & Key Features

- **100% Bare-Metal & Custom Linker Script**: Custom-written `.ld` script (`STM32F411XX_BOOTLOADER.ld`) establishing precise memory mapping for the bootloader and main application sections.
- **Custom Peripheral Drivers**:
  - **UART Driver**: Register-level driver for serial communication and status reporting.
  - **DMA Driver**: High-throughput automated memory/peripheral transfer for incoming binary payloads.
  - **Internal Flash Driver**: Sector erasure, unlock/lock routines, and page programming routines.
- **Custom Interrupts & Vector Table**: Custom startup sequence, exception handlers (`isr_routine.c`), and manual interrupt controller management.
- **Host-Side Python Flasher**: Includes `Load_RTOS.py`, a utility to stream RTOS/Application binary images directly into microcontroller Flash over serial UART.
- **VS Code Build Automation**: Automated build workflows using VS Code tasks integrated with `bootloader.cproject.yml` (CMSIS-CSolution format).
- **Verified on Hardware**: Fully validated on physical **STM32 NUCLEO-F411RE** hardware.

---

## 📁 Directory Structure

```text
bootloader/
├── bootloader.cproject.yml                  # CMSIS-CSolution project configuration
├── Inc/                                     # Header files
│   ├── drivers/
│   │   ├── dma.h                            # DMA driver interface
│   │   ├── flash.h                          # Internal Flash memory control interface
│   │   └── uart.h                           # UART driver interface
│   ├── isr_routine.h                        # Interrupt service routine handlers
│   ├── load_fw.h                            # Firmware loader state machine & protocol
│   ├── log.h                                # Lightweight serial logging utility
│   └── string.h                             # Minimal standalone string utility functions
├── linker/
│   └── stm/
│       └── stm32f411re/
│           └── STM32F411XX_BOOTLOADER.ld    # Custom linker script for bootloader memory layout
├── RTE/                                     # Run-Time Environment & CMSIS Device startup files
│   ├── _Debug_Nucleo-F411RE/
│   │   └── RTE_Components.h
│   ├── Device/
│   │   └── STM32F411RETx/
│   │       ├── startup_stm32f411xe.S          # Vector table & Reset_Handler assembly
│   │       ├── startup_stm32f411xe.S.base@2.6.8
│   │       ├── system_stm32f4xx.c           # System initialization (Clocks, SystemInit)
│   │       └── system_stm32f4xx.c.base@2.6.8
│   └── _Release_Nucleo-F411RE/
│       └── RTE_Components.h
└── Src/                                     # Source implementation
    ├── drivers/
    │   └── stm/
    │       └── stm32f411re/
    │           ├── dma.c                    # DMA register setup & handler logic
    │           ├── flash.c                  # Sector unlock, erase, write sequence
    │           ├── isr_routine.c            # ISR routines & NVIC handling
    │           ├── startup.c                # Low-level reset routines
    │           └── uart.c                   # UART configuration & transmit/receive
    ├── load_fw.c                            # Application download & Flash writing logic
    ├── log.c                                # Serial logging implementation
    └── string.c                             # Lightweight string utility implementation
```

---

## 🗺️ Memory Map & Architecture

The STM32F411RE features **512 KB Flash** and **128 KB SRAM**. The memory map is partitioned between the bootloader and the target application (e.g., FreeRTOS / Bare-Metal App):

| Memory Region | Address Range | Size | Purpose |
| :--- | :--- | :--- | :--- |
| **Bootloader Flash** | `0x0800 0000 - 0x0800 3FFF` | 16 KB (Sector 0) | Bootloader execution, initialization, & update logic |
| **Application Flash** | `0x0800 4000 - 0x0807 FFFF` | ~496 KB (Sectors 1–7) | User Application / RTOS image |
| **SRAM** | `0x2000 0000 - 0x2002 0000` | 128 KB | Shared RAM / Stack / Heap |

### Boot Flow Sequence

1. **Power-On / Reset**: Core fetches MSP (Main Stack Pointer) and Reset Handler address from Sector 0 (`0x0800 0000`).
2. **System Setup**: Clocks and low-level peripherals (UART, DMA, Flash controller) are initialized.
3. **Firmware Update Check**:
   - The bootloader listens for incoming update commands over UART.
   - If a binary stream is detected via `Load_RTOS.py`, Flash sectors starting at `0x0800 4000` are erased, and the incoming image is programmed via DMA/Flash drivers.
4. **Jump to Application**:
   - Once validated, the vector table is relocated to `0x0800 4000` via `SCB->VTOR`.
   - The MSP is set to `*(uint32_t*)0x08004000`.
   - Bootloader jumps to the application Reset Handler at `*(uint32_t*)0x08004004`.

---

## 🛠️ Build & Flash Instructions

### Prerequisites

- **Toolchain**: `arm-none-eabi-gcc` (Arm GNU Toolchain)
- **Build System**: CMSIS-CSolution / `cbuild` (Open-CMSIS-Pack) or VS Code Arm environment
- **Python**: Python 3.x with `pyserial` installed (`pip install pyserial argparse`)
- **Hardware**: NUCLEO-F411RE board with USB-UART connection.

### 1. Build using VS Code Tasks

The project is configured with automated **VS Code Tasks** matching `.cproject.yml`:

1. Open the repository in VS Code.
2. Press `Ctrl+Shift+B` (or `Cmd+Shift+B` on macOS) to execute the default build task.
3. Select between `Debug` and `Release` configurations.

Alternatively, build via command line using `cbuild`:

```bash
cbuild bootloader.cproject.yml --configuration Release
```

### 2. Loading Target Firmware via Python Host Utility

To flash an application binary (e.g., FreeRTOS image) through the bootloader:

```bash
python Load_RTOS.py --port /dev/ttyUSB0 --baud 115200 --file /home/stanislav/Projects/STM_bootloader/out/rtos/Nucleo-F411RE/Release/rtos.bin
```

#### CLI Parameters:
- `--port`: Serial port assigned to the NUCLEO board (e.g., `/dev/ttyUSB0` or `COM3`).
- `--baud`: UART baud rate (default: `115200`).
- `--file`: Absolute or relative path to target application `.bin` file.

---

## 🔬 Key Low-Level Implementation Highlights

- **Custom Linker Script (`STM32F411XX_BOOTLOADER.ld`)**:
  Defines custom section memory mappings (`.text`, `.rodata`, `.data`, `.bss`), stack positioning, and explicit boundary symbols used by `startup.c` to clear `.bss` and copy initialized data from Flash to SRAM.

- **Direct Register Manipulation**:
  Driver implementations interact directly with STM32 peripheral control registers (RCC, USART, DMA, FLASH) through base addresses and bitwise operations, bypassing generic HAL abstraction layers for minimal binary footprint and high performance.

- **Reliable Flash In-Application Programming (IAP)**:
  Handles sector unlocking key sequences (`0x45670123`, `0xCDEF89AB`), status register polling (`FLASH_SR_BSY`), and half-word/word programming sequences safely.

---

## 🧪 Hardware Verification

- **Board**: STMicroelectronics NUCLEO-F411RE
- **MCU**: STM32F411RET6 (Cortex-M4 @ 100MHz, 512KB Flash, 128KB SRAM)
- **Test Status**: Fully tested and verified. Application firmware successfully transferred, flashed, and executed via bootloader jump mechanism.

---

## 👤 Author & Portfolio Notice

This project was conceived, engineered, and tested independently as a personal embedded software engineering portfolio project. It serves as a demonstration of bare-metal ARM Cortex-M architecture understanding, low-level firmware design, memory management, and hardware protocol integration.