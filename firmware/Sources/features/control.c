#include "features/control.h"


void _ctrl_encoder_init(void);
void _ctrl_button_init(void);
void _ctrl_update_value(uint8_t *value, uint8_t step);

extern volatile uint32_t tick;
volatile int8_t enc_value = 0;
volatile uint32_t enc_btn = 0;
volatile uint8_t enc_btn_press = 0;
uint8_t enc_btn_skip_press = 0;

void control_init(void) {
	_ctrl_encoder_init();
	_ctrl_button_init();
}

void _ctrl_encoder_init(void) {
	// TIM3 Encoder mode
	// PA7, PB3
	GPIOA->MODER &= ~(0b11 << 14);
	GPIOA->MODER |= (0b10 << 14);
	GPIOA->OSPEEDR |= 0b10 << 14;
	GPIOA->AFR[0] |= (0b0001 << 28);
	GPIOB->MODER &= ~(0b11 << 8);
	GPIOB->MODER |= (0b10 << 8);
	GPIOB->OSPEEDR |= 0b10 << 8;
	GPIOB->AFR[0] |= (0b0001 << 16);

	RCC->APBENR1 |= RCC_APBENR1_TIM3EN;
	TIM3->CR1 = TIM_CR1_ARPE;
	TIM3->CCMR1 = (0b01 << TIM_CCMR1_CC1S_Pos) | (0b01 << TIM_CCMR1_CC2S_Pos);
	TIM3->SMCR = 0b0011;
	TIM3->CCER = TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC1P;
	TIM3->DIER = TIM_DIER_CC1IE | TIM_DIER_CC2IE;
	TIM3->CR1 |= TIM_CR1_CEN;

	NVIC_EnableIRQ(TIM3_IRQn);
}

void _ctrl_button_init(void) {
	// PB0
	RCC->APBENR2 |= RCC_APBENR2_SYSCFGEN;
	GPIOB->MODER &= ~(0b11 << 0);

	EXTI->RTSR1 |= 1;
	EXTI->FTSR1 |= 1;
	EXTI->EXTICR[0] |= 0x01 << EXTI_EXTICR1_EXTI0_Pos;
	EXTI->IMR1 |= 1;

	NVIC_EnableIRQ(EXTI0_1_IRQn);
}

uint8_t control_update(mlight_t *mlight) {
	if (enc_value) {
		if (enc_btn) {
			enc_btn_skip_press = 1;
			if (enc_value > 0 && mlight->led_freq < LED_FREQ_OPTIONS - 1) {
				mlight->led_freq++;
			} else if (enc_value < 0 && mlight->led_freq > 0) {
				mlight->led_freq--;
			}
		} else {
			if (mlight->menu_active == 1) {
				_ctrl_update_value(&(mlight->led_brightness), 1);
			} else {
				_ctrl_update_value(&(mlight->fan_speed), 5);
			}
		}

		enc_value = 0;
		return CTRL_ACTION_UPDATE;
	} else if (enc_btn_press == BTN_SHORT_PRESS) {
		mlight->menu_active = mlight->menu_active == 1 ? 0 : 1;
		enc_btn_press = 0;
	} else if (enc_btn_press == BTN_LONG_PRESS) {
		enc_btn_press = 0;
		return CTRL_ACTION_PAUSE;
	}

	return 0;
}

void _ctrl_update_value(uint8_t *value, uint8_t step) {
	if (enc_value > 0 && *value + step <= 100) {
		*value += step;
	} else if (enc_value < 0 && *value - step >= 0) {
		*value -= step;
	}
}

void TIM3_IRQHandler(void) {
	if (TIM3->CR1 & TIM_CR1_DIR) {
		enc_value = 1;
	} else {
		enc_value = -1;
	}
	TIM3->SR &= ~(TIM_SR_CC1IF | TIM_SR_CC2IF);
}

void EXTI0_1_IRQHandler(void) {
	if (EXTI->RPR1 & 1) {
		enc_btn = tick;
		EXTI->RPR1 |= 1;
	} else if (EXTI->FPR1 & 1) {
		if (!enc_btn_skip_press) {
			uint32_t time = tick - enc_btn;

			if (time < BTN_SHORT_PRESS_TIME) {
				enc_btn_press = BTN_SHORT_PRESS;
			} else if (time > BTN_LONG_PRESS_TIME && time < BTN_MAX_PRESS_TIME) {
				enc_btn_press = BTN_LONG_PRESS;
			} else {
				enc_btn_press = 0;
			}
		}

		enc_btn = 0;
		enc_btn_skip_press = 0;
		EXTI->FPR1 |= 1;
	}
}
