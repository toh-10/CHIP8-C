#include "chip8/chip8.h"

CHIP8* initialize() {
  CHIP8* chip8;
  CPU* cpu;

  cpu = malloc(sizeof(CPU));
  cpu->pc = ROM_ADDRESS;
  cpu->i = 0;
  cpu->sp = 0;
  cpu->v = calloc(sizeof(DATA_REGISTER), DATA_REGISTER_COUNT);
  cpu->delay_timer = 0;
  cpu->sound_timer = 0;

  chip8 = malloc(sizeof(CHIP8));
  chip8->memory = calloc(sizeof(uint8_t), MEMORY_SIZE);
  chip8->stack = calloc(sizeof(STACK), STACK_DEPTH_COUNT);
  chip8->display_buffer =
      calloc(sizeof(uint8_t), (DISPLAY_HEIGHT * DISPLAY_WIDTH));
  chip8->display_flag = true;
  chip8->key_inputs = calloc(KEY_COUNT, sizeof(uint8_t));
  chip8->cpu = cpu;

  for (int i = 0; i < 80; i++)
    chip8->memory[FONT_ADDRESS + i] = chip8_fontset[i];

  return chip8;
};

void reset(CHIP8* chip8) {}

void load_rom(uint8_t* memory, char* file_path) {
  FILE* fd;
  size_t fr;

  fd = fopen(file_path, "r");
  if (fd == NULL) error("Unable to open file: %s", file_path);

  fseek(fd, 0, SEEK_END);
  uint16_t file_size = ftell(fd);
  rewind(fd);

  char* rom_buffer = (char*)malloc(sizeof(char) * file_size);
  if (rom_buffer == NULL) error("Out of memory.");

  size_t result = fread(rom_buffer, sizeof(char), (size_t)file_size, fd);
  if (result != file_size) error("Unable to read ROM.");

  if (MEMORY_SIZE > file_size) {
    for (int i = 0; i < file_size; ++i) {
      memory[i + 512] = (uint8_t)rom_buffer[i];
    }
  }

  fclose(fd);
};

void timer_tick(CHIP8* chip8) {
  if (chip8->cpu->delay_timer > 0) chip8->cpu->delay_timer--;
  if (chip8->cpu->sound_timer > 0) {
    chip8->cpu->sound_timer--;
    if (chip8->cpu->sound_timer == 0) debug_print("BEEP!\n");
  }
}

void cpu_tick(CHIP8* chip8) {
  uint16_t opcode =
      chip8->memory[chip8->cpu->pc] << 8 | chip8->memory[chip8->cpu->pc + 1];

  instruct_op(chip8, opcode);
}

void tick(CHIP8* chip8) {
  cpu_tick(chip8);
  video_tick(chip8);
  timer_tick(chip8);
};

void run(CHIP8* chip8, bool (*should_close_callback)(),
         void (*video_callback)()) {
  struct timeval clock_now;

  video_register_callback(video_callback);

  while (!should_close_callback()) {
    gettimeofday(&clock_now, NULL);
    if (timediff_ms(&clock_now, &clock_prev) >= CLOCK_RATE_MS) {
      tick(chip8);
      clock_prev = clock_now;
    }
  }
}
