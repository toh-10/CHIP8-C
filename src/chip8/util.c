#include "chip8/util.h"

void debug_print(char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  fprintf(stdout, "[DEBUG] ");
  vfprintf(stdout, fmt, ap);
  fprintf(stdout, "\n");
}
