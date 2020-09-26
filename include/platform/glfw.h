#ifndef PLATFORM_H
#define PLATFORM_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <sys/time.h>
#include <time.h>

#include "chip8/chip8.h"

#define SCREEN_TITLE "chip8"
#define DISPLAY_PIXEL_SIZE 15
#define SCREEN_WIDTH (DISPLAY_WIDTH * DISPLAY_PIXEL_SIZE)
#define SCREEN_HEIGHT (DISPLAY_HEIGHT * DISPLAY_PIXEL_SIZE)

#define COLOR_1 1
#define COLOR_2 2

#define COLOR_1_R 19
#define COLOR_1_G 161
#define COLOR_1_B 14

#define COLOR_2_R 40
#define COLOR_2_G 40
#define COLOR_2_B 40

typedef struct gfx_color {
  GLubyte r;
  GLubyte g;
  GLubyte b;
} COLOR;

bool gfx_closed();

void gfx_setup();

void gfx_draw(uint8_t* display_buffer);

void gfx_terminate();

#endif
