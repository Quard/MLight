#ifndef __FEATURES_BACKLIGHT_H__
#define __FEATURES_BACKLIGHT_H__

#include "stm32g030xx.h"


void backlight_init(void);
void backlight_set_brightness(uint8_t brightness);
void backlight_set_freq(uint8_t option);

#endif /* __FEATURES_BACKLIGHT_H__ */
