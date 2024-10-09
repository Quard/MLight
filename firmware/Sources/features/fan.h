#ifndef __FEATURES_FAN_H__
#define __FEATURES_FAN_H__

#include "stm32g0xx.h"


void fan_init(void);
void fan_set_speed(uint8_t speed);

#endif /* __FEATURES_FAN_H__ */
