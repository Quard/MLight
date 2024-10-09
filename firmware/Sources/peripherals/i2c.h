#ifndef __PERIPHERALS_I2C_H__
#define __PERIPHERALS_I2C_H__

#include "stm32g030xx.h"


void i2c1_init(void);
int8_t i2c1_write(uint8_t addr, uint8_t *buf, uint16_t len);
int8_t i2c1_read(uint8_t addr, uint8_t *buf, uint8_t len);

#endif /* __PERIPHERALS_I2C_H__ */
