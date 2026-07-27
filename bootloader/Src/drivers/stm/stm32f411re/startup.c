#include <stdint.h>
#include <stm32f411xe.h>
#include "isr_routine.h"
#include "log.h"
#include "drivers/flash.h"
#include "load_fw.h"

#define DMA_LOG_UART_TX_IRQ DMA2_Stream7_IRQn

extern uint32_t _estack; 
extern uint32_t _begin_app; // Symbol defined in the linker script, representing the start of the application code

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

    LOG_init();

    init_gpio_a_clock();
    init_boot_status_pin();
    LOG_send("Bootloader started. Checking boot status pin...\r\n");
    if(is_flash_mode()){
        Load_FW_Flash_Start();
    } else {
        LOG_send("Jumping to main application.\r\n");
        SCB->VTOR = _begin_app;
        __set_MSP(_estack);
        void (*app_reset_handler)(void) = (void (*)(void))(*((uint32_t*)(&_begin_app + 4)));
        app_reset_handler();
    }
    while(1){}
}

extern void DMA2_Stream7_IRQHandler(void);
extern void USART1_IRQHandler(void);

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
    [16 + DMA_LOG_UART_TX_IRQ] = DMA2_Stream7_IRQHandler, // DMA1 Stream6 interrupt handler
    [16 + USART1_IRQn] = USART1_IRQHandler, // USART2 interrupt handler
    [16 + FLASH_IRQn] = Flash_Interrupt_Handler, // Flash interrupt handler
};