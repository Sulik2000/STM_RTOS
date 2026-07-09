#include <stdint.h>
#include <stm32f411xe.h>
#include "isr_routine.h"
#include "drivers/dma.h"
#include "log.h"
#include "main.h"

extern uint32_t _estack; 

void Load_Data(){
    extern uint32_t _sidata;
    extern uint32_t _sdata; 
    extern uint32_t _edata; 

    uint32_t* src = &_sidata;
    uint32_t* dest = &_sdata;

    while(dest < &_edata){
        *dest++ = *src++;
    }
}

void Load_BSS(){
    extern uint32_t _sbss; 
    extern uint32_t _ebss; 

    uint32_t* dest = &_sbss;

    while(dest < &_ebss){
        *dest++ = 0;
    }
}

void Reset_Handler(void){
    Load_Data();

    Load_BSS();
    
    init_log_system();
    log_message("System initialized successfully.\n");
    main();
    while(1){}
}

const __attribute__((section(".isr_vector"), used)) void* vector_table[] = {
    &_estack, // Initial stack pointer
    Reset_Handler, // Reset handler
    NMI_Handler, // NMI handler
    HardFault_Handler, // Hard fault handler
    MemManage_Handler, // Memory management fault handler
    BusFault_Handler, // Bus fault handler
    UsageFault_Handler, // Usage fault handler
    0, 0, 0, 0, // Reserved
    SVC_Handler, // SVCall handler
    DebugMon_Handler, // Debug monitor handler
    0, // Reserved
    PendSV_Handler, // PendSV handler
    SysTick_Handler, // SysTick handler
    [16 + DMA1_Stream6_IRQn] = DMA1_Stream6_IRQHandler, // DMA1 Stream6 interrupt handler
};