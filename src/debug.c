#include <stdarg.h>
#include <stdio.h>
#include "debug.h"

void debug(char *fmt, ...)
{
  fmt = fmt;
#if DEBUG
  va_list args;
  va_start(args, fmt);
  fprintf(stderr, fmt, args);
  va_end(args);
#endif
}
