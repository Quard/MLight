#include "features/fan.h"


void fan_init(void) {
	// GPIO - PA4
	GPIOA->MODER &= ~(0b11 << 8);
	GPIOA->MODER |= 0b10 << 8;
	GPIOA->OSPEEDR |= 0b10 << 8;
	GPIOA->AFR[0] |= 0b0100 << 16;

	RCC->APBENR2 |= RCC_APBENR2_TIM14EN;

	TIM14->PSC = 32 - 1;
	TIM14->ARR = 20 - 1;

	TIM14->CCMR1 = (0b110 << 4) | TIM_CCMR1_OC1PE;
	TIM14->CR1 = TIM_CR1_ARPE;
	TIM14->EGR = TIM_EGR_UG;
	TIM14->CCER = TIM_CCER_CC1E;

	TIM14->CCR1 = 0;

	TIM14->CR1 |= TIM_CR1_CEN;
}

void fan_set_speed(uint8_t speed) {
	if (speed > 100)
		speed = 100;

	TIM14->CCR1 = (uint8_t)(speed / 100.f * 20.f);
}
