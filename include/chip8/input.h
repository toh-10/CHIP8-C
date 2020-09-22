#ifndef INPUT_H
#define INPUT_H

#include "chip8.h"

int keymap(unsigned char k);

void keypress(CHIP8 *chip8, char k);

void keyrelease(CHIP8 *chip8, char k);

#endif