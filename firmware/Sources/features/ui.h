#ifndef __FEATURES_UI_H__
#define __FEATURES_UI_H__

#include <stdio.h>
#include "stm32g030xx.h"
#include "mlight.h"
#include "peripherals/i2c.h"
#include "features/control.h"
#include "features/ui/ssd1306.h"
#include "features/ui/u8g2_font_render.h"


void ui_init(void);
void ui_update(mlight_t *mlight);
void ui_setup_menu(mlight_t *mlight);

#endif /* __FEATURES_UI_H__ */
