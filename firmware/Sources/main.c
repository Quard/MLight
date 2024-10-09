#include <stdint.h>
#include "stm32g030xx.h"
#include "mlight.h"
#include "utils.h"
#include "peripherals/adc.h"
#include "peripherals/i2c.h"
#include "features/fan.h"
#include "features/backlight.h"
#include "features/ui.h"
#include "features/control.h"
#include "features/mem.h"
#include "features/qc.h"


#define ADC_TO_VOLTS(x)  ((uint8_t)(x / 4095.f * 3.3f * 6.4462f * 10))

void mcu_init(void);

extern volatile int8_t enc_value;
extern volatile uint8_t enc_click;
volatile uint32_t tick;
volatile uint8_t ui_update_event = 1;
volatile uint16_t adc_voltage = 0;

int main(void) {
	qc_t qc;
	mlight_t mlight = {
		.fan_speed=0,
		.led_freq=0,
		.led_brightness=0,
		.voltage=0,
		.voltage_source=0,
		.menu_active=1,
		.state=0
	};

	mcu_init();

	i2c1_init();
	adc_init();
	ui_init();
	mem_read(&mlight);

	fan_init();
	backlight_init();
	control_init();

	while (!adc_voltage);
	mlight.voltage = ADC_TO_VOLTS(adc_voltage);
	if (mlight.voltage > 35 && mlight.voltage < 55) {
		if (qc_init(&qc) == QC_RET_OK) {
			mlight.voltage_source = VOLTAGE_SOURCE_QC;
			qc_set_mode(&qc, QC_MODE_12V);
		}
	}

	delay_ms(1000);

	fan_set_speed(mlight.fan_speed);
	backlight_set_freq(mlight.led_freq);
	backlight_set_brightness(mlight.led_brightness);

	mlight.voltage = ADC_TO_VOLTS(adc_voltage);
	if (mlight.voltage > 110 && mlight.voltage < 130) {
		GPIOA->MODER &= ~(0b11 << 10);
		GPIOA->MODER |= (0b01 << 10);
		GPIOA->ODR |= (1 << 5);
	}

	uint8_t mem_save_counter = 0;
	uint8_t ctrl_res;
	for(;;) {
		ctrl_res = control_update(&mlight);
		if (ctrl_res == CTRL_ACTION_UPDATE) {
			fan_set_speed(mlight.fan_speed);
			backlight_set_freq(mlight.led_freq);
			backlight_set_brightness(mlight.led_brightness);

			mem_save_counter = 50;
			mlight.state |= 1 << STATE_HAS_CHANGES;
		} else if (ctrl_res == CTRL_ACTION_PAUSE) {
			mlight.state ^= 1 << STATE_PAUSE;
			if (mlight.state & (1 << STATE_PAUSE)) {
				fan_set_speed(0);
				backlight_set_brightness(0);
			} else {
				fan_set_speed(mlight.fan_speed);
				backlight_set_brightness(mlight.led_brightness);
			}
		}

		if (ui_update_event) {
			mlight.voltage = ADC_TO_VOLTS(adc_voltage);
			ui_update_event = 0;
			ui_update(&mlight);

			if (mem_save_counter) {
				if (mem_save_counter == 1) {
					mem_save(&mlight);
					mlight.state &= ~(1 << STATE_HAS_CHANGES);
				}
				mem_save_counter--;
			}
		}
	}
}

void mcu_init(void) {
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOBEN;
	SysTick_Config(16000);

	__enable_irq();
}

void SysTick_Handler(void) {
	tick++;
	if (tick % 25 == 0) ui_update_event = 1;
}

void ADC_IRQHandler(void) {
	adc_voltage = ADC1->DR;
}
