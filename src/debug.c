#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug.h"
#include "client.h"

void debug(char *fmt, ...)
{
  fmt = fmt;
#if DEBUG
  va_list args;
  va_start(args, fmt);
  char *fmtfmt = KCYN "==DEBUG== %s\n" KNRM;
  char *format = calloc(strlen(fmtfmt) + strlen(fmt) + 1, sizeof(char));
  sprintf(format, fmtfmt, fmt);
  vfprintf(stderr, format, args);
  free(format);
  va_end(args);
#endif
}

void verbose(char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  if (g_bt.verbose)
    vprintf(fmt, args);
  va_end(args);
}
