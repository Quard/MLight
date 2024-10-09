#include "peripherals/i2c.h"


void i2c1_init(void) {
	// SCL - PB8 SDA - PB9
	GPIOB->MODER &= ~((0b11 << 16) | (0b11 << 18));
	GPIOB->MODER |= (0b10 << 16) | (0b10 << 18);
	GPIOB->OSPEEDR |= (0b11 << 16) | (0b11 << 18);
	GPIOB->OTYPER |= (1 << 8) | (1 << 9);
	GPIOB->AFR[1] &= ~((0b1111 << 4) | (0b1111 << 0));
	GPIOB->AFR[1] |= (0b0110 << 4) | (0b0110 << 0);

	RCC->APBENR1 |= RCC_APBENR1_I2C1EN;
	I2C1->CR1 = 0;
	I2C1->TIMINGR = 0x0010061A;  // 400 kHz
//	I2C1->TIMINGR = 0x00303D5B;  // 100 kHz
	I2C1->CR2 = I2C_CR2_AUTOEND;
	I2C1->OAR1 = I2C_OAR1_OA1EN;
	I2C1->CR1 |= I2C_CR1_PE;
}

int8_t i2c1_write(uint8_t addr, uint8_t *buf, uint16_t len) {
	uint8_t size = 0;
	uint32_t cr2 = 0;

	while(I2C1->ISR & I2C_ISR_BUSY);

	while (len) {
		if (cr2 == 0) {
			cr2 = I2C_CR2_START;
		} else {
			cr2 = 0;
			while (!(I2C1->ISR & I2C_ISR_TCR)) {
				if (I2C1->ISR & I2C_ISR_STOPF) {
					I2C1->ICR = 0xFFFF;
					return -1;
				}
			}
		}

		cr2 |= addr;

		if (len > 0xFF) {
			size = 0xFF;
			cr2 |= (size << I2C_CR2_NBYTES_Pos) | I2C_CR2_RELOAD;
		} else {
			size = len;
			cr2 |= (size << I2C_CR2_NBYTES_Pos) | I2C_CR2_AUTOEND;
		}
		I2C1->CR2 = cr2;

		while (size) {
			while (!(I2C1->ISR & I2C_ISR_TXIS)) {
				if (I2C1->ISR & I2C_ISR_STOPF) {
					I2C1->ICR = 0xFFFF;
					return -1;
				}
			}
			I2C1->TXDR = *buf++;
			size--;
			len--;
		}
	}

	while (!(I2C1->ISR & I2C_ISR_STOPF));

	I2C1->ICR = 0xFFFF;

	return 0;
}

int8_t i2c1_read(uint8_t addr, uint8_t *buf, uint8_t len) {
  while(I2C1->ISR & I2C_ISR_BUSY);
  I2C1->CR2 = addr | I2C_CR2_START | (len << I2C_CR2_NBYTES_Pos) | I2C_CR2_AUTOEND | I2C_CR2_RD_WRN;

  while (len) {
    while(!(I2C1->ISR & I2C_ISR_RXNE)) {
      if (I2C1->ISR & I2C_ISR_STOPF) {
        I2C1->ICR = 0xFFFF;
        return -1;
      }
    }
    *buf++ = I2C1->RXDR;
    len--;
  }

  while (!(I2C1->ISR & I2C_ISR_STOPF));

  I2C1->ICR = 0xFFFF;

  return 0;
}

