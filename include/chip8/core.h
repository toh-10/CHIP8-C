#ifndef CORE_H
#define CORE_H

#include "chip8.h"

#define FONT_ADDRESS 0x0
#define ROM_ADDRESS 0x200
#define MEMORY_SIZE 4096 - 0x200
#define MAX_ROM_SIZE 4096
#define DATA_REGISTER_SIZE 16
#define STACK_DEPTH_COUNT 16
#define DELAY_TIMER_COUNT 60
#define SOUND_TIMER_COUNT 60
#define KEY_COUNT 16
#define FONT_BYTES_PER_CHAR 5

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32
#define DISPLAY_SIZE (DISPLAY_WIDTH * DISPLAY_HEIGHT)

#define CLOCK_HZ 60
#define CLOCK_RATE_MS (int)((1.0 / CLOCK_HZ) * 1000)

typedef uint8_t DATA_REGISTER;
typedef uint16_t STACK;

typedef struct cpu {
  DATA_REGISTER v[DATA_REGISTER_SIZE];
  uint16_t i;
  uint16_t pc;
  uint16_t sp;
  uint16_t delay_timer;
  uint16_t sound_timer;
} CPU;

typedef struct chip8 {
  CPU cpu;
  STACK stack[STACK_DEPTH_COUNT];
  uint8_t key_inputs[KEY_COUNT];
  uint8_t memory[MEMORY_SIZE];
  uint8_t display_buffer[DISPLAY_SIZE];
  uint8_t display_flag;
} CHIP8;

#endif
