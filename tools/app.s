
/home/stanislav/Projects/STM_bootloader/out/rtos/Nucleo-F411RE/Debug/rtos.elf:     file format elf32-littlearm


Disassembly of section .text:

08004000 <vector_table>:
 8004000:	00 00 00 20 21 40 00 08                             ... !@..

08004008 <delay>:
#include "stm32f4xx.h"

uint32_t _estack; // Symbol defined in the linker script, representing the end of the stack

// Simple blocking delay function
static void delay(volatile uint32_t count) {
 8004008:	b082      	sub	sp, #8
 800400a:	9001      	str	r0, [sp, #4]
    while (count--) {
 800400c:	e000      	b.n	8004010 <delay+0x8>
        __NOP();
 800400e:	bf00      	nop
    while (count--) {
 8004010:	9b01      	ldr	r3, [sp, #4]
 8004012:	1e5a      	subs	r2, r3, #1
 8004014:	9201      	str	r2, [sp, #4]
 8004016:	2b00      	cmp	r3, #0
 8004018:	d1f9      	bne.n	800400e <delay+0x6>
    }
}
 800401a:	b002      	add	sp, #8
 800401c:	4770      	bx	lr
	...

08004020 <main>:

void main(void) {
 8004020:	b508      	push	{r3, lr}
    /* 1. Enable AHB1 peripheral clock for GPIOA */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
 8004022:	4a11      	ldr	r2, [pc, #68]	@ (8004068 <main+0x48>)
 8004024:	6b13      	ldr	r3, [r2, #48]	@ 0x30
 8004026:	f043 0301 	orr.w	r3, r3, #1
 800402a:	6313      	str	r3, [r2, #48]	@ 0x30

    /* 2. Configure PA5 as General Purpose Output Mode
     * MODER5[1:0] bits = 01 (Output)
     */
    GPIOA->MODER &= ~(GPIO_MODER_MODE5_1); // Clear bit 1
 800402c:	4b0f      	ldr	r3, [pc, #60]	@ (800406c <main+0x4c>)
 800402e:	681a      	ldr	r2, [r3, #0]
 8004030:	f422 6200 	bic.w	r2, r2, #2048	@ 0x800
 8004034:	601a      	str	r2, [r3, #0]
    GPIOA->MODER |= GPIO_MODER_MODE5_0;    // Set bit 0
 8004036:	681a      	ldr	r2, [r3, #0]
 8004038:	f442 6280 	orr.w	r2, r2, #1024	@ 0x400
 800403c:	601a      	str	r2, [r3, #0]

    /* 3. Output type: Push-Pull (default state after reset, bit cleared) */
    GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_5);
 800403e:	685a      	ldr	r2, [r3, #4]
 8004040:	f022 0220 	bic.w	r2, r2, #32
 8004044:	605a      	str	r2, [r3, #4]

    /* 4. Output speed: Low Speed (default state after reset) */
    GPIOA->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED5);
 8004046:	689a      	ldr	r2, [r3, #8]
 8004048:	f422 6240 	bic.w	r2, r2, #3072	@ 0xc00
 800404c:	609a      	str	r2, [r3, #8]

    /* 5. Pull-up / Pull-down: No pull-up, no pull-down */
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD5);
 800404e:	68da      	ldr	r2, [r3, #12]
 8004050:	f422 6240 	bic.w	r2, r2, #3072	@ 0xc00
 8004054:	60da      	str	r2, [r3, #12]

    /* Main Loop */
    while (1) {
        /* Toggle PA5 using ODR XOR */
        GPIOA->ODR ^= GPIO_ODR_OD5;
 8004056:	4a05      	ldr	r2, [pc, #20]	@ (800406c <main+0x4c>)
 8004058:	6953      	ldr	r3, [r2, #20]
 800405a:	f083 0320 	eor.w	r3, r3, #32
 800405e:	6153      	str	r3, [r2, #20]

        /* Delay for roughly ~500ms at default 16MHz HSI clock */
        delay(500000);
 8004060:	4803      	ldr	r0, [pc, #12]	@ (8004070 <main+0x50>)
 8004062:	f7ff ffd1 	bl	8004008 <delay>
    while (1) {
 8004066:	e7f6      	b.n	8004056 <main+0x36>
 8004068:	40023800 	.word	0x40023800
 800406c:	40020000 	.word	0x40020000
 8004070:	0007a120 	.word	0x0007a120

Disassembly of section .init:

08004074 <_init>:
 8004074:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
 8004076:	bf00      	nop

Disassembly of section .fini:

08004078 <_fini>:
 8004078:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
 800407a:	bf00      	nop
