#include "features/backlight.h"


void backlight_init(void) {
	// GPIO - PA6
	GPIOA->MODER &= ~(0b11 << 12);
	GPIOA->MODER |= 0b10 << 12;
	GPIOA->OSPEEDR |= 0b10 << 12;
	GPIOA->AFR[0] |= 0b0101 << 24;

	RCC->APBENR2 |= RCC_APBENR2_TIM16EN;

	TIM16->PSC = 20-1;  // 200 - 800Hz, 40 - 4kHz, 20 - 8kHz, 10 - 16kHz
	TIM16->ARR = 100-1;

	TIM16->CCMR1 = (0b110 << 4) | TIM_CCMR1_OC1PE;
	TIM16->CR1 = TIM_CR1_ARPE;
	TIM16->EGR = TIM_EGR_UG;
	TIM16->CCER = TIM_CCER_CC1NE | TIM_CCER_CC1E;
	TIM16->BDTR = TIM_BDTR_MOE;

	TIM16->CCR1 = 0;

	TIM16->CR1 |= TIM_CR1_CEN;
}

void backlight_set_brightness(uint8_t brightness) {
	if (brightness > 100)
		brightness = 100;

	TIM16->CCR1 = brightness;
}

void backlight_set_freq(uint8_t option) {
	switch (option) {
	case 0: // 1kHz
		TIM16->PSC = 160 - 1;
		break;
	case 1: // 4kHz
		TIM16->PSC = 40 - 1;
		break;
	case 2: // 8kHz
		TIM16->PSC = 20 - 1;
		break;
	case 3:  // 16kHz
		TIM16->PSC = 10 - 1;
		break;
	}
}
