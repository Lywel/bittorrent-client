#include <stdlib.h>
#include "buffer.h"

s_buf *buffer_init(char *str, long long size)
{
  s_buf *buf = malloc(sizeof(s_buf));
  if (!buf)
    return NULL;
  buf->str = str;
  buf->len = size;
  return buf;
}

void buffer_free(s_buf *buf)
{
  if (buf)
  {
    free(buf->str);
    free(buf);
  }
}
