#include "features/ui.h"
#include "features/ui/u8g2_fonts.h"


void ui_draw_pixel_fg(uint8_t x, uint8_t y);
void ui_draw_pixel_bg(uint8_t x, uint8_t y);


SSD1306_t disp;
U8G2FontRender_t font_main;
U8G2FontRender_t font_small;
static uint8_t ui_invert = 0;
static uint8_t ui_voltage_flash = 0;

void ui_init(void) {
	disp.addr = 0x78;
	disp.width = 128;
	disp.height = 64;
	disp.i2c_write = &i2c1_write;

	ssd1306_init(&disp);

	font_small = U8G2FontRender(u8g2_font_t0_12_mf, &ui_draw_pixel_fg, &ui_draw_pixel_bg);
	font_main = U8G2FontRender(u8g2_font_spleen8x16_mf, &ui_draw_pixel_fg, &ui_draw_pixel_bg);

	U8G2FontRender_Print(&font_main, 5, 10, "MLight");
	U8G2FontRender_Print(&font_small, 100, 46, "v0.6");

	ssd1306_update(&disp);
}

void ui_update(mlight_t *mlight) {
	uint8_t buf[20] = {'\0'};

	ssd1306_fill(&disp, COLOR_BLACK);

	ssd1306_draw_rframe(&disp, 0, mlight->menu_active ? 18 : 0, 127, 18, COLOR_WHITE);
	ssd1306_draw_rectangle(&disp, 1, mlight->menu_active ? 19 : 1, 126, 17, COLOR_WHITE);

	if (mlight->menu_active == 0) ui_invert = 1;
	sprintf((char *)buf, "fan: %d%%", mlight->fan_speed);
	U8G2FontRender_Print(&font_main, 5, 0, (char *)buf);
	ui_invert = 0;

	if (mlight->menu_active == 1) ui_invert = 1;
	sprintf((char *)buf, "led: %d%%", mlight->led_brightness);
	U8G2FontRender_Print(&font_main, 5, 18, (char *)buf);
	ui_invert = 0;

	sprintf((char *)buf, "%dkHz", led_freq[mlight->led_freq]);
	U8G2FontRender_Print(&font_small, 5, 46, (char *)buf);

	if (mlight->voltage < 115 || mlight->voltage > 125) {
		ui_voltage_flash++;
		ui_invert = ui_voltage_flash & 1;
		if (ui_invert) {
			ssd1306_draw_rframe(&disp, 78, 45, 49, 16, COLOR_WHITE);
			ssd1306_draw_rectangle(&disp, 79, 46, 48, 15, COLOR_WHITE);
		}
	}
	sprintf((char *)buf, "%s %2d.%dV", mlight->voltage_source ? "QC" : "DC", mlight->voltage / 10, mlight->voltage % 10);
	U8G2FontRender_Print(&font_small, 79, 46, (char *)buf);
	ui_invert = 0;

	if (mlight->state & (1 << STATE_HAS_CHANGES)) {
		U8G2FontRender_Print(&font_small, 70, 46, "*");
	}

	if (mlight->state & (1 << STATE_PAUSE)) {
		U8G2FontRender_Print(&font_small, 40, 46, "P");
	}

	ssd1306_update(&disp);
}

void ui_draw_pixel_fg(uint8_t x, uint8_t y) {
	ssd1306_draw_pixel(&disp, x, y, ui_invert ? COLOR_BLACK : COLOR_WHITE);
}

void ui_draw_pixel_bg(uint8_t x, uint8_t y) {
	ssd1306_draw_pixel(&disp, x, y, ui_invert ? COLOR_WHITE : COLOR_BLACK);
}
