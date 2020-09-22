#include "include/chip8/chip8.h"
#include "include/platform/glfw.h"

int main(int argc, char* argv[]) {
  CHIP8* chip8;
  char* file_path;

  if (argc != 2) {
    error("Invalid argument number: %d\n", argc);
  }

  chip8 = initialize();
  gfx_setup(chip8);

  file_path = argv[1];
  load_rom(chip8->memory, file_path);

  run(chip8, *gfx_closed, *gfx_draw);

  gfx_terminate();

  return 0;
};
