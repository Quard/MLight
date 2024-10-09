#ifndef __MLIGHT_H__
#define __MLIGHT_H__

#define VERSION_MAJOR 0
#define VERSION_MINOR 5

#define MEM_STORAGE_SIZE  3
#define LED_FREQ_OPTIONS  4

#define STATE_HAS_CHANGES  0
#define STATE_PAUSE        1

#define VOLTAGE_SOURCE_DC  0
#define VOLTAGE_SOURCE_QC  1


typedef struct {
	uint8_t fan_speed;
	uint8_t led_freq;
	uint8_t led_brightness;
	uint8_t voltage;
	uint8_t voltage_source;
	uint8_t menu_active;
	uint8_t state;
} mlight_t;

static const uint8_t led_freq[] = {1, 4, 8, 16};

#endif /* __MLIGHT_H__ */
