#ifndef MESSAGE_H
# define MESSAGE_H

#include <stdint.h>

#define B_SIZE 16384

struct __attribute__((__packed__)) message
{
    unsigned char len[4];
    uint8_t id;
    unsigned char *payload;
};

struct __attribute__((__packed__)) request
{
  unsigned char len[4];
  uint8_t id;
  int index;
  int begin;
  int length;
};

struct __attribute__((__packed__)) piece
{
  unsigned char len[4];
  uint8_t id;
  int index;
  int begin;
  unsigned char *block;
};

#endif
