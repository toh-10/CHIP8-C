#ifndef CHIP8_H
#define CHIP8_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "core.h"
#include "error.h"
#include "fonts.h"
#include "input.h"
#include "instructions.h"
#include "timer.h"
#include "util.h"
#include "video.h"

CHIP8 *initialize();

void load_rom(uint8_t *memory, char *file_path);

void next_tick(CHIP8 *chip8);

void run(CHIP8 *chip8, bool (*should_close_callback)(),
         void (*video_callback)());

#endif
