/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <stdint.h>
#include "stm32f1xx.h"
#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

void Interrupt_Config (void);
void GPIO_Config (void);

uint8_t flag_EXTI1 = 0;
int main(void)
{
	SystemInit();

	Interrupt_Config ();
	GPIO_Config();

    /* Loop forever */
	for(;;)
	{
		if (flag_EXTI1)
		{
			flag_EXTI1 = 0;

			// From HAL_GPIO_TogglePin
			uint16_t LED_PIN = (1 << 12);
			uint32_t LED0_mask = (GPIOB->ODR & LED_PIN);
			uint16_t LED_set_mask = ~LED0_mask & LED_PIN;
			uint16_t LED_reset_mask = LED0_mask & LED_PIN;

			GPIOB->BSRR |= (LED_set_mask | (LED_reset_mask << 16));
		}
	}
}

void Interrupt_Config (void)
{
	// Bit 0:0 AFIOEN: Alternate function IO clock enable
	RCC->APB2ENR |= (1 << 0); // 1: Alternate Function IO clock enabled

	// Bits 7:4 EXTI1[3:0]: EXTI 1
	AFIO->EXTICR[0] &= 	~(0xF << 4);
	AFIO->EXTICR[0] |= 	(1 << 4); // 0001: PB1 pin

	// Bits 1:1 MRx: Interrupt Mask on line 1
	EXTI->IMR |= (1 << 1); // Interrupt request from Line 1 is not masked

	//
	// Bits 1:1 TRx: Rising trigger event configuration bit of line 1
	EXTI->RTSR &= ~(1 << 1); // Rising trigger disabled (for Event and Interrupt) for input line

	// Bits 1:1 TRx: Falling trigger event configuration bit of line 1
	EXTI->FTSR |= (1 << 1); // Falling trigger enabled (for Event and Interrupt) for input line.

	//
	NVIC_SetPriority(EXTI1_IRQn, 1);
	NVIC_EnableIRQ(EXTI1_IRQn);
}

void GPIO_Config (void)
{
	// Set IOPB EN
	RCC->APB2ENR |= (1 << 3); // Enable GPIOB clock

	// Set MODE12[1:0] = 01: : Output mode, max speed 10 MHz
	GPIOB->CRH |= (1 << 16); // set bit 16
	GPIOB->CRH &= ~(1 << 17); // clear bit 17

	// Clear  CNF12[1:0]: General purpose output push-pull
	GPIOB->CRH &= ~(0b11 << 18);
}

void EXTI1_IRQHandler(void)
{

	// PRx: Pending bit
	if(EXTI->PR & (1 << 1)) // 1: selected trigger request occurred
	{
		flag_EXTI1 = 1;
		EXTI->PR |= (1 << 1); // "This bit is cleared by writing a ‘1’ into the bit. "
	}
}


