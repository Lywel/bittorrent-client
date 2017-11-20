#ifndef MESSAGE_H
# define MESSAGE_H

#include <stdint.h>

#define B_SIZE 16384

struct __attribute__((__packed__)) message
{
    uint32_t len;
    uint8_t id;
};

struct __attribute__((__packed__)) request
{
  uint32_t len;
  uint8_t id;
  int index;
  int begin;
  int length;
};

struct __attribute__((__packed__)) piece
{
  int index;
  int begin;
};

#endif
