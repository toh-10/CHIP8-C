#ifndef VIDEO_H
#define VIDEO_H

#include "chip8.h"

void display_draw(CHIP8* chip8, uint8_t x, uint8_t y, uint8_t n);

void display_clear();

void video_register_callback(void (*callback)(uint8_t*));

void video_tick(CHIP8* chip8);

#endif