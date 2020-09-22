#include "chip8/video.h"

static void (*video_callback)(uint8_t*);

void display_draw(CHIP8* chip8, uint8_t x, uint8_t y, uint8_t n) {
  uint8_t sprite_row, sprite_col, x_coordinate, y_coordinate, byte_to_write;
  uint16_t pixel_index;

  for (sprite_row = 0; sprite_row < n; sprite_row++) {
    byte_to_write = chip8->memory[chip8->cpu->i + sprite_row];

    for (sprite_col = 0; sprite_col < 8; sprite_col++) {
      if (byte_to_write & (0x80 >> sprite_col)) {
        x_coordinate = (x + sprite_col);
        y_coordinate = (y + sprite_row);
        pixel_index = y_coordinate * DISPLAY_WIDTH + x_coordinate;

        if (chip8->display_buffer[pixel_index] == 0x01) chip8->cpu->v[0xF] = 1;
        chip8->display_buffer[pixel_index] ^= 0x01;
      }
    }
  }
  chip8->display_flag = true;
}

void display_clear(CHIP8* chip8) {
  memset(chip8->display_buffer, 0,
         sizeof(uint8_t) * (DISPLAY_HEIGHT * DISPLAY_WIDTH));
  chip8->display_flag = true;
}

void video_register_callback(void (*callback)(uint8_t*)) {
  video_callback = callback;
}

void video_tick(CHIP8* chip8) {
  if (chip8->display_flag) {
    video_callback(chip8->display_buffer);
    chip8->display_flag = false;
  }
}
