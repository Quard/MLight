#include "features/ui/ssd1306.h"


#define SSD1306_BUF_SIZE  (128 * 64 / 8 + 1)

#define ssd1306_write_cmd(dev, command)   ssd1306_i2c_write((dev), 0x00, (command))
#define ssd1306_write_data(dev, data)     ssd1306_i2c_write((dev), 0x40, (data))
#define swap(a, b) {uint8_t t = a; a = b; b = t; }


int8_t ssd1306_i2c_write(SSD1306_t *dev, uint8_t reg, uint8_t data);


static uint8_t ssd1306_buf[SSD1306_BUF_SIZE];
static uint8_t ssd1306_init_script[] = {
	0xAE,  // display off
	0x20,  // memory addressing mode
	0x10,  // 00 - horizontal; 01 - vertical; 10 - page (reset); 11 - invalid
	0xB0,  // page start for page addressing mode
	0x00,  // low column address
	0x10,  // high column address
	0x40,  // start line address
	0x8F,  //	0x81,  // contrast
	0xFF,
#ifndef DISPLAY_ROTATE
	0xA1,  // segment remap 0 to 127
	0xC8,  // COM output scan direction
#else
	0xA0,
	0xC0,
#endif
	0xA6,  // normal display
	0xA8,  // multiplex ratio (1 to 64)
	0x3F,
	0xA4,  // 0xA4 - output follow RAM content; 0xA5 - output ignore RAM content
	0xD3,  // display offset
	0x00,  // no offset
	0xD5,  // display clock divide ratio/oscillator frequency
	0xF0,  // divide ratio
	0xD9,  // pre-charge period
	0x22,  // ^^
	0xDA,  // com pins hardware configuration
	0x00,  //	0x12,  // ^^
	0xDB,  // VCOMH
	0x20,  // 0x20 - 0.77xVcc
	0x8D,  // DC-DC enable
	0x14,
	0xAF,  // display on
};


int8_t ssd1306_init(SSD1306_t *dev) {
	ssd1306_init_script[22] = dev->height == 32 ? 0x02 : 0x12;

	for (uint8_t i=0; i < sizeof(ssd1306_init_script); i++) {
		ssd1306_write_cmd(dev, ssd1306_init_script[i]);
	}

	memset(&ssd1306_buf, COLOR_BLACK, SSD1306_BUF_SIZE);

	return 0;
}

int8_t ssd1306_update(SSD1306_t *dev) {
	ssd1306_buf[0] = 0x40;
	return dev->i2c_write(dev->addr, &ssd1306_buf[0], SSD1306_BUF_SIZE);
}

void ssd1306_fill(SSD1306_t *dev, uint8_t color) {
	memset(&ssd1306_buf, color == COLOR_BLACK ? 0x00 : 0xFF, SSD1306_BUF_SIZE);
}

void ssd1306_draw_pixel(SSD1306_t *dev, uint8_t x, uint8_t y, uint8_t color) {
	if (x < 0 || x > dev->width || y < 0 || y > dev->height) {
		return;
	}

	if (color == COLOR_WHITE) {
		ssd1306_buf[x + (y / 8) * dev->width + 1] |= 1 << (y % 8);
	} else {
		ssd1306_buf[x + (y / 8) * dev->width + 1] &= ~(1 << (y % 8));
	}
}

void ssd1306_draw_line_straight(SSD1306_t *dev, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color) {
	uint8_t start, end;

	if (x0 == x1) {
		start = MIN(y0, y1);
		end = MAX(y0, y1);
		for (;start < end; start++) {
			ssd1306_draw_pixel(dev, x0, start, color);
		}
	} else if (y0 == y1) {
		start = MIN(x0, x1);
		end = MAX(x0, x1);
		for (;start < end; start++) {
			ssd1306_draw_pixel(dev, start, y0, color);
		}
	}
}

void ssd1306_draw_line(SSD1306_t *dev, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color) {
	int16_t delta_x = abs(x1 - x0);
	int16_t delta_y = abs(y1 - y0);
	int8_t sign_x = x0 < x1 ? 1 : -1;
	int8_t sign_y = y0 < y1 ? 1 : -1;
	int16_t err0 = delta_x - delta_y;

	ssd1306_draw_pixel(dev, x1, y1, color);
	while (x0 != x1 || y0 != y1) {
		ssd1306_draw_pixel(dev, x0, y0, color);
		int16_t err1 = err0 * 2;
		if (err1 > -delta_y) {
			err0 -= delta_y;
			x0 += sign_x;
		}
		if (err1 < delta_x) {
			err0 += delta_x;
			y0 += sign_y;
		}
	}
}

void ssd1306_draw_rectangle(SSD1306_t *dev, uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color) {
  for (uint8_t i=0; i<height; i++) {
	  ssd1306_draw_line_straight(dev, x, y + i, x + width, y + i, color);
  }
}

void ssd1306_draw_frame(SSD1306_t *dev, uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color) {
	ssd1306_draw_line_straight(dev, x, y, x + width, y, color);
	ssd1306_draw_line_straight(dev, x, y + height, x + width, y + height, color);
	ssd1306_draw_line_straight(dev, x, y, x, y + height, color);
	ssd1306_draw_line_straight(dev, x + width, y, x + width, y + height, color);
}

void ssd1306_draw_rframe(SSD1306_t *dev, uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color) {
	ssd1306_draw_line_straight(dev, x + 1, y, x + width - 1, y, color);
	ssd1306_draw_line_straight(dev, x + 1, y + height, x + width - 1, y + height, color);
	ssd1306_draw_line_straight(dev, x, y + 1, x, y + height - 1, color);
	ssd1306_draw_line_straight(dev, x + width, y + 1, x + width, y + height - 1, color);
}

int8_t ssd1306_i2c_write(SSD1306_t *dev, uint8_t reg, uint8_t data) {
	uint8_t dt[2] = {reg, data};

	return dev->i2c_write(dev->addr, dt, 2);
}
