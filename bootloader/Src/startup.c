#include <stdint.h>
#include <stm32f411xe.h>
#include "isr_routine.h"
#include "drivers/dma.h"
#include "drivers/usart2.h"
#include "log.h"
#include "main.h"
#include "drivers/flash.h"
#include "load_fw.h"

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

void init_boot_status_pin(void){

    GPIOA->MODER |= (0b00 << 2*5); // Set PA5 as input
    GPIOA->OTYPER |= (0b0 << 5); // Set PA5 as push-pull
    GPIOA->PUPDR |= (0b10 << 2*5); // Set PA5 as pull-down
}

void init_gpio_a_clock(void){
    RCC->AHB1ENR |= (0b1 << 0); // Enable GPIOA clock
}

uint8_t is_flash_mode(void){
    return (GPIOA->IDR & (0b1 << 5)) != 0; // Check if PA5 is high
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
    log_message("System initialized successfully.\r\n");

    init_gpio_a_clock();
    init_boot_status_pin();
    if(is_flash_mode()){
        log_message("Entering flash mode.\r\n");
        Load_FW_Flash_Start();
    } else {
        log_message("Jumping to main application.\r\n");
    }
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
    [16 + DMA_LOG_UART_TX_IRQ] = DMA1_Stream6_IRQHandler, // DMA1 Stream6 interrupt handler
    [16 + USART2_IRQn] = USART2_IRQHandler, // USART2 interrupt handler
    [16 + FLASH_IRQn] = Flash_Interrupt_Handler, // Flash interrupt handler
};