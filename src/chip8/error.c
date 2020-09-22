#include "chip8/error.h"

void error(char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  fprintf(stderr, "[ERROR] ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(EXIT_FAILURE);
}
