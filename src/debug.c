#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug.h"

void debug(char *fmt, ...)
{
  fmt = fmt;
#if DEBUG
  va_list args;
  va_start(args, fmt);
  char *fmtfmt = "===%s\n";
  char *format = calloc(8 + strlen(fmtfmt) + strlen(fmt) + 1, sizeof(char));
  sprintf(format, fmtfmt, fmt);
  vfprintf(stderr, format, args);
  free(format);
  va_end(args);
#endif
}
