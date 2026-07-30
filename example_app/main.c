#include "stm32f4xx.h"

uint32_t _estack; // Symbol defined in the linker script, representing the end of the stack

// Simple blocking delay function
static void delay(volatile uint32_t count) {
    while (count--) {
        __NOP();
    }
}

void main(void) {
    /* 1. Enable AHB1 peripheral clock for GPIOA */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    /* 2. Configure PA5 as General Purpose Output Mode
     * MODER5[1:0] bits = 01 (Output)
     */
    GPIOA->MODER &= ~(GPIO_MODER_MODE5_1); // Clear bit 1
    GPIOA->MODER |= GPIO_MODER_MODE5_0;    // Set bit 0

    /* 3. Output type: Push-Pull (default state after reset, bit cleared) */
    GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_5);

    /* 4. Output speed: Low Speed (default state after reset) */
    GPIOA->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED5);

    /* 5. Pull-up / Pull-down: No pull-up, no pull-down */
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD5);

    /* Main Loop */
    while (1) {
        /* Toggle PA5 using ODR XOR */
        GPIOA->ODR ^= GPIO_ODR_OD5;

        /* Delay for roughly ~500ms at default 16MHz HSI clock */
        delay(500000);
    }

}

const __attribute__((section(".irq_vector"), used)) void* vector_table[] = {
    &_estack, // Initial stack pointer
    main, // Reset handler
};

