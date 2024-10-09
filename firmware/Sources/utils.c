#include "utils.h"


extern uint32_t tick;


void delay_ms(uint32_t ms) {
  uint32_t deadline = tick + ms;

  while (tick < deadline) {
    __NOP();
  }
}
