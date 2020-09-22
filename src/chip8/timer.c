#include "chip8/timer.h"

uint16_t timediff_ms(struct timeval* end, struct timeval* start) {
  uint16_t diff;

  diff = (end->tv_sec - start->tv_sec) * 1000 +
         (end->tv_usec - start->tv_usec) / 1000;
  return diff;
}
