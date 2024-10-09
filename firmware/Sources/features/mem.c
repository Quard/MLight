#include "features/mem.h"


uint8_t _mem_get_crc(uint8_t *data, uint8_t len);


int8_t mem_save(mlight_t *mlight) {
	uint8_t buf[MEM_STORAGE_SIZE + 2] = {
		0x00,  // memory address
		mlight->fan_speed,
		mlight->led_freq,
		mlight->led_brightness,
		0x00,  // for the CRC
	};

	buf[MEM_STORAGE_SIZE + 1] = _mem_get_crc(buf + 1, MEM_STORAGE_SIZE);

	return i2c1_write(MEM_ADDR, buf, MEM_STORAGE_SIZE + 2);
}

int8_t mem_read(mlight_t *mlight) {
	uint8_t buf[MEM_STORAGE_SIZE + 1];
	buf[0] = 0x00;

	int8_t err = i2c1_write(MEM_ADDR, buf, 1);
	if (err != 0) return err;
	err = i2c1_read(MEM_ADDR, buf, MEM_STORAGE_SIZE + 1);
	if (err != 0) return err;

	uint8_t crc = _mem_get_crc(buf, MEM_STORAGE_SIZE);
	if (crc != buf[MEM_STORAGE_SIZE]) return 1;

	mlight->fan_speed = buf[0];
	mlight->led_freq = buf[1];
	mlight->led_brightness = buf[2];

	return 0;
}

uint8_t _mem_get_crc(uint8_t *data, uint8_t len) {
  uint8_t crc = 0xFF;

  for (int8_t i=0; i<len; i++) {
    crc ^= data[i];
    for (uint8_t bit=8; bit > 0; --bit) {
      if (crc & 0x80) {
        crc = (crc << 1) ^ 0x31u;
      } else {
        crc = (crc << 1);
      }
    }
  }

  return crc;
}
