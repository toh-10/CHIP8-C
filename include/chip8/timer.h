#ifndef TIMER_H
#define TIMER_H

#include <sys/time.h>
#include <time.h>

#include "chip8.h"

struct timeval clock_prev;

struct timeval clock_now;

uint16_t timediff_ms(struct timeval* end, struct timeval* start);

#endif