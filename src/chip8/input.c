#include "chip8/input.h"

int keymap(unsigned char k) {
  switch (k) {
    case '1':
      return 0x1;
    case '2':
      return 0x2;
    case '3':
      return 0x3;
    case '4':
      return 0xc;

    case 'q':
      return 0x4;
    case 'w':
      return 0x5;
    case 'e':
      return 0x6;
    case 'r':
      return 0xd;

    case 'a':
      return 0x7;
    case 's':
      return 0x8;
    case 'd':
      return 0x9;
    case 'f':
      return 0xe;

    case 'z':
      return 0xa;
    case 'x':
      return 0x0;
    case 'c':
      return 0xb;
    case 'v':
      return 0xf;

    default:
      return -1;
  }
}

void keypress(CHIP8* chip8, char k) {
  debug_print("key press: %c\n", k);

  int index = keymap(k);
  if (index >= 0) {
    chip8->key_inputs[index] = 1;
  }
}

void keyrelease(CHIP8* chip8, char k) {
  debug_print("key release: %c\n", k);

  int index = keymap(k);
  if (index >= 0) {
    chip8->key_inputs[index] = 0;
  }
}
