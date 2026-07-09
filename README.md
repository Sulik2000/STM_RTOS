# STM_RTOS: Microkernel RTOS & Bootloader Ecosystem

An open-source, highly decoupled microkernel Real-Time Operating System (RTOS) and secure Bootloader ecosystem designed for ARM Cortex-M4 microcontrollers. Developed strictly in C99 and Assembly using **CMSIS** and **STM32 LL (Low-Layer) Drivers**—completely bypassing heavy HAL abstractions. 

This project is actively developed and tested on the **STMicroelectronics Nucleo-F411RE** development board.

---

## 🗺️ Architectural Concept & Philosophy

STM_RTOS breaks away from standard monolithic firmware design by physically and logically cutting the system into three completely isolated binary applications: **Bootloader**, **RTOS Microkernel**, and **User Application**.

```text
+------------------------------------------------------------+
|                        FLASH MEMORY                        |
+--------------------+-------------------+-------------------+
|     Bootloader     |   RTOS Kernel     |  User Application |
|   (0x08000000)     |   (0x08004000)    |   (0x08010000)    |
+--------------------+-------------------+-------------------+
|                    ^                   ^
| Relocates VTOR     |                   |
+--------------------+ Calls Kernels     |
| via SVC           |
+-------------------+
```

### 1. Bootloader & Vector Table Relocation (`VTOR`)
Upon reset, the MCU boots into the Bootloader (`0x08000000`). The bootloader prepares the system, fires up low-level DMA logging, checks image validities, and handles FOTA updates. Before jumping to the OS, it updates the `SCB->VTOR` register to target the RTOS Kernel's dedicated vector table, executing a clean handoff.

### 2. Microkernel & Context Switching
The RTOS Kernel resides in its own flash memory segment. It manages a priority-based scheduler, explicit task stacks, and interrupt mechanics (`SysTick`, `PendSV`). Execution boundaries are strictly enforced.

### 3. Asynchronous Binary Isolation & Late Binding
The User Application is compiled 100% independently from the kernel. The kernel has **zero compiled-in knowledge** of application symbols or memory positions. 
* The application image leaves a fixed structural header at its base address containing its cryptographic signatures and an explicit entry pointer (`app_main`).
* At runtime, the RTOS reads this memory offset dynamically, creates the user process context, and boots it.
* User code *never* invokes kernel functions directly; communication happens strictly via Hardware **SVC (Supervisor Call)** routines.

---

## 📂 Repository Structure

```text
.
├── bootloader/                     # Independent Bootloader Binary Target
│   ├── Inc/
│   │   ├── drivers/
│   │   │   ├── dma.h               # Bare-metal DMA control structures
│   │   │   └── usart2.h            # USART2 Register level interfaces
│   │   ├── isr_routine.h           # Interrupt Vector Table hooks
│   │   ├── log.h                   # Non-blocking DMA logger interface
│   │   ├── main.h
│   │   └── string.h                # Custom, lightweight safe string manipulation
│   ├── Src/
│   │   ├── drivers/                # Low-level hardware drivers 
│   │   │   ├── dma.c
│   │   │   └── usart2.c
│   │   ├── isr_routine.c           # Vector Table ISR implementations
│   │   ├── log.c                   # High-speed UART-DMA logger implementation
│   │   ├── main.c                  # Handoff sequencer logic
│   │   ├── startup.c               # Custom bare-metal Reset_Handler, .data/.bss unpacker
│   │   └── string.c
│   ├── CMakeLists.txt              # Subproject build definition
│   └── STM32F411XX_BOOTLOADER.ld   # Strict Memory Partition Linker Script
├── rtos/                           # Real-Time Operating System Microkernel Target
│   └── CMakeLists.txt
├── app_template/                   # Decoupled Standalone User App (Planned)
├── Drivers/                        # Monorepo Shared Hardware Layer
│   ├── CMSIS/                      # Core ARM Cortex-M4 registers definitions
│   └── STM32F4xx_HAL_Driver/       # Pure STM32 Low-Layer (LL) driver sources
├── cmake/
│   └── gcc-arm-none-eabi.cmake     # Target Toolchain Cross-Compilation Profile
├── CMakeLists.txt                  # Root Master Orchestration Configuration
├── CMakePresets.json               # Modern Build Profiles Configuration
└── LICENSE                         # MIT License
```

## Low-Level Implementation Details

    startup.c (Bootloader): Completely custom hardware entry sequence. Bypasses standard IDE runtime library setups. Initializes physical CPU stacks, maps .data segments from Flash to RAM, zeroes out the .bss section, configures essential clock matrices, and spawns the DMA logging engine before entering main().

    log.c / dma.c: Features zero-overhead debugging. Text streams are piped out through an asynchronous circular queue mapped to USART2 via DMA Stream transfer, completely avoiding CPU blocking bottlenecks during boot execution.

    isr_routine.c: Provides absolute register-level encapsulation for specialized interrupt service routines (isr_routine), ensuring non-overlapping execution paths between components.

## Developer Experience (DX) & Toolchain Setup

This environment utilizes modern DevOps workflows designed for zero local host pollution.
Modern Devcontainers (Docker)

The workspace ships with a fully pre-packaged .devcontainer configuration.

    You do not need to install gcc-arm-none-eabi, cmake, ninja, or specialized packages to your host machine.

    Simply open this repository inside VSCode with 'Dev Containers' extension and click "Reopen in Container". VSCode will pull a verified Linux container containing all production-ready embedded cross-compiling toolchains automatically.

## Automation via VS Code Tasks

The repository comes equipped with automated tasks.json pipelines. You can trigger them directly within VS Code (Ctrl+Shift+B or via Command Palette):

    Building release binary: Configures and compiles the entire codebase using the modern CMake Release preset.

    Building debug binary: Configures and compiles the system with the Debug preset, preserving full symbol tables for debugging.

    Launch bootloader with OpenOCD (ST-Link): Builds the project and automatically flashes the resulting STM_bootloader.elf directly to the Nucleo board over an ST-Link debugger interface using OpenOCD.

## Hardware Debugging

Right now, hardware step-by-step zapping and memory inspection are fully supported via the Cortex-Debug extension in VS Code. Since the project is in its foundational phase, debugging targets the Bootloader workflow to safely observe low-level register setups, clock configurations, and DMA log generation.
🚀 Future Roadmap

Contributions are what make the open-source community amazing. Here is what is actively planned for implementation:

    [ ] Priority Scheduler Consolidation: Transition Kernel scheduling matrices to use explicit priority hierarchies inside task_struct.

    [ ] Hardware MPU Sandboxing: Activate the ARM Cortex-M4 Memory Protection Unit (MPU) to prevent Application threads from modifying RTOS kernel RAM regions.

    [ ] Unified System Call API: Standardize the raw assembly SVC system routing backend mapping application requests to Kernel services.

    [ ] UART Host Programming Scripts (/tools): Finalize production python scripting utilities to cleanly push separate application binaries down UART lines by triggering hardware pin configurations.

    [ ] FOTA (Firmware Over-The-Air): Implement a safe, fault-tolerant dual-bank flash swapping mechanism into the Bootloader routine.

## 📄 License

This ecosystem is distributed entirely under the MIT License. Check out the LICENSE file for full permission boundaries. Feel free to copy, tweak, or deploy any portion of this low-level architecture for your own projects!