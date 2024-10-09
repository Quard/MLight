#ifndef __FEATURES_MEM_H__
#define __FEATURES_MEM_H__

#include "stm32g030xx.h"
#include "mlight.h"
#include "peripherals/i2c.h"


#define MEM_ADDR (0b1010000 << 1)


int8_t mem_save(mlight_t *mlight);
int8_t mem_read(mlight_t *mlight);

#endif /* __FEATURES_MEM_H__ */
