#ifndef TIMER_H
#define TIMER_H

#include <sys/time.h>
#include <time.h>

#include "chip8.h"

uint16_t timediff_ms(struct timeval* end, struct timeval* start);

#endif