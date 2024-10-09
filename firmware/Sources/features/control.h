#ifndef __FEATURES_CONTROL_H__
#define __FEATURES_CONTROL_H__

#include "stm32g030xx.h"
#include "mlight.h"

#define BTN_SHORT_PRESS  1
#define BTN_LONG_PRESS   2
#define BTN_SHORT_PRESS_TIME  300
#define BTN_LONG_PRESS_TIME   900
#define BTN_MAX_PRESS_TIME    3000

#define CTRL_ACTION_UPDATE 1
#define CTRL_ACTION_PAUSE  2

void control_init(void);
uint8_t control_update(mlight_t *mlight);

#endif /* __FEATURES_CONTROL_H__ */
