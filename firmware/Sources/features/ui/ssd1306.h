#ifndef __FEATURES_UI_SSD1306_H__
#define __FEATURES_UI_SSD1306_H__

#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include "stm32g030xx.h"


#define COLOR_BLACK 0x00
#define COLOR_WHITE 0x01


typedef int8_t (*fn_i2c_write)(uint8_t addr, uint8_t *buf, uint16_t len);

typedef struct {
	uint8_t addr;   // 0x78 or 0x7A
	uint8_t width;
	uint8_t height;
	fn_i2c_write i2c_write;
} SSD1306_t;

int8_t ssd1306_init(SSD1306_t *dev);
int8_t ssd1306_update(SSD1306_t *dev);
void ssd1306_fill(SSD1306_t *dev, uint8_t color);
void ssd1306_draw_pixel(SSD1306_t *dev, uint8_t x, uint8_t y, uint8_t color);
void ssd1306_draw_line_straight(SSD1306_t *dev, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color);
void ssd1306_draw_line(SSD1306_t *dev, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color);
void ssd1306_draw_rectangle(SSD1306_t *dev, uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color);
void ssd1306_draw_frame(SSD1306_t *dev, uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color);
void ssd1306_draw_rframe(SSD1306_t *dev, uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color);


#endif /* __FEATURES_UI_SSD1306_H__ */
