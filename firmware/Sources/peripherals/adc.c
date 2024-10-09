#include "peripherals/adc.h"


void adc_init(void) {
	// PA10/PA12
	RCC->APBENR2 |= RCC_APBENR2_ADCEN;
//	GPIOA->MODER &= (0b11 << 20);
	GPIOA->MODER |= 0b11 << 20;

	ADC1->CHSELR = 1 << 16;
	ADC1->CFGR1 = ADC_CFGR1_CONT;
	ADC1->CFGR2 = 0;
	ADC1->SMPR = (0b111 << ADC_SMPR_SMP1_Pos);
	ADC1->IER = ADC_IER_EOCIE;
	ADC1->CR = ADC_CR_ADEN;

	NVIC_EnableIRQ(ADC1_IRQn);

	while (!(ADC1->ISR & ADC_ISR_ADRDY)) {
		__NOP();
	}
	ADC1->CR |= ADC_CR_ADSTART;
}
