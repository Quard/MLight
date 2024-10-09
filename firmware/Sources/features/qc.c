#include "features/qc.h"


void _qc_dp_output(void);
void _qc_dm_output(void);
void _qc_dm_input(void);
void _qc_dp_set(qc_pin_mode_t mode);
void _qc_dm_set(qc_pin_mode_t mode);
uint8_t _qc_dm_is_high(void);


int8_t qc_init(qc_t *qc) {
	_qc_dp_output();
	_qc_dm_input();

	_qc_dp_set(QC_PIN_33);
	delay_ms(QC_DELAY_START_CHECK);
	if (_qc_dm_is_high()) {
		qc->mode = QC_MODE_5V;
		qc->voltage = 50;
	} else {
		return QC_RET_NOT_SUPPORTED;
	}

	_qc_dp_set(QC_PIN_06);
	delay_ms(QC_DELAY_HANDSHAKE);

	_qc_dp_set(QC_PIN_33);
	delay_ms(QC_DELAY_START_CHECK);
	if (_qc_dm_is_high()) {
		return QC_RET_10_ONLY;
	}

	_qc_dm_output();

	return QC_RET_OK;
}

void qc_set_mode(qc_t *qc, qc_mode_t mode) {
	if (qc->mode == QC_MODE_CONTINUOUS && mode != QC_MODE_CONTINUOUS) {
		// transition from the Continuous Mode to any other mode requires to switch to 5V mode first
		_qc_dp_set(QC_PIN_06);
		_qc_dm_set(QC_PIN_GND);
		delay_ms(QC_DELAY_NEW_MODE);

		if (mode == QC_MODE_5V) {
			qc->mode = QC_MODE_5V;
			qc->voltage = 50;
			return;
		}
	}

	switch (mode) {
	case QC_MODE_5V:
		_qc_dp_set(QC_PIN_06);
		_qc_dm_set(QC_PIN_GND);
		qc->voltage = 50;
		break;
	case QC_MODE_9V:
		_qc_dp_set(QC_PIN_33);
		_qc_dm_set(QC_PIN_06);
		qc->voltage = 90;
		break;
	case QC_MODE_12V:
		_qc_dp_set(QC_PIN_06);
		_qc_dm_set(QC_PIN_06);
		qc->voltage = 120;
		break;
	case QC_MODE_20V:
		_qc_dp_set(QC_PIN_33);
		_qc_dm_set(QC_PIN_33);
		qc->voltage = 200;
		break;
	case QC_MODE_CONTINUOUS:
		_qc_dp_set(QC_PIN_06);
		_qc_dm_set(QC_PIN_33);
		break;
	}

	qc->mode = mode;
	delay_ms(QC_DELAY_NEW_MODE);
}

void _qc_dp_output(void) {
	// PA2 + PA3
	GPIOA->MODER &= ~(0b1111 << 4);
	GPIOA->MODER |= (0b0101 << 4);
}

void _qc_dm_output(void) {
	// PA0 + PA1
	GPIOA->MODER &= ~(0b1111 << 0);
	GPIOA->MODER |= (0b0101 << 0);
}

void _qc_dm_input(void) {
	// PA0 + PA1
	GPIOA->MODER &= ~(0b1111 << 0);
}

void _qc_dp_set(qc_pin_mode_t mode) {
	switch (mode) {
	case QC_PIN_GND:
		GPIOA->ODR &= ~(0b11 << 2);
		break;
	case QC_PIN_06:
		GPIOA->ODR &= ~(0b11 << 2);
		GPIOA->ODR |= (0b1 << 3);
		break;
	case QC_PIN_33:
		GPIOA->ODR |= (0b11 << 2);
		break;
	}
}

void _qc_dm_set(qc_pin_mode_t mode) {
	switch (mode) {
	case QC_PIN_GND:
		GPIOA->ODR &= ~(0b11 << 0);
		break;
	case QC_PIN_06:
		GPIOA->ODR &= ~(0b11 << 0);
		GPIOA->ODR |= (0b1 << 1);
		break;
	case QC_PIN_33:
		GPIOA->ODR |= (0b11 << 0);
		break;
	}
}

uint8_t _qc_dm_is_high(void) {
	return GPIOA->IDR & 0b11 ? 1 : 0;
}
