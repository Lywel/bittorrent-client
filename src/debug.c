#include <stdarg.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug.h"

void
debug(char *fmt, ...)
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

void
verbose(char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  if (g_bt.verbose)
    vprintf(fmt, args);
  va_end(args);
}

void
verbose_recv(struct message mess, struct peer *p)
{
  verbose("%x%x%x: msg: recv: %s:%u ", (uint8_t)g_bt.info_hash[0],
          (uint8_t)g_bt.info_hash[1], (uint8_t)g_bt.info_hash[2],
          p->ip, p->port);
  if (mess.id == 0)
  {
    if (mess.len)
      verbose("choke\n");
    else
      verbose("keep alive\n");
  }
  else if (mess.id == 1)
    verbose("unchoke\n");
  else if (mess.id == 2)
    verbose("interested\n");
  else if (mess.id == 3)
    verbose("not interested\n");
  else if (mess.id == 4)
    verbose("have\n");
  else if (mess.id == 5)
  {
    if (g_bt.pieces_len == mess.len - 1)
      verbose("bitfield ");
    else
      verbose("piece continuation\n");
  }
  else if (mess.id == 6)
    verbose("request\n");
  else if (mess.id == 7)
    verbose("piece\n");
  else if (mess.id == 8)
    verbose("cancel\n");
  else if (mess.id == 9)
    verbose("port\n");
  else
    verbose("not supported\n");
}
