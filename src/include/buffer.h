#ifndef BUFFER_H
# define BUFFER_H

typedef struct buffer
{
  long long len;
  char *str;
} s_buf;

s_buf *buffer_init(char *str, long long size);
void buffer_free(s_buf *buf);

#endif
