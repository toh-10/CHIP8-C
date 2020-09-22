#ifndef PLATFORM_H
#define PLATFORM_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
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

static GLuint index_buffer[] = {0, 1, 2, 2, 3, 0};

static GLfloat vertex_buffer[] = {-1.0f, 1.0f,  0.0f, 0.0f, 0.0f,   // vertex 1
                                  1.0f,  1.0f,  0.0f, 1.0f, 0.0f,   // vertex 2
                                  1.0f,  -1.0f, 0.0f, 1.0f, 1.0f,   // vertex 3
                                  -1.0f, -1.0f, 0.0f, 0.0f, 1.0f};  // vertex 4

static void paint_pixel(GLuint row, GLuint col, COLOR* color);

static void paint_cell(GLuint row, GLuint col, COLOR* color);

static void set_large_pixels(uint8_t* display_buffer);

static void render();

static void setup_object();

int gfx_closed();

void gfx_setup();

void gfx_draw(uint8_t* display_buffer);

void gfx_terminate();

#endif
