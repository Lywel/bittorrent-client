#ifndef RECIEVE_MESSAGE_H
# define RECIEVE_MESSAGE_H

#include "message.h"
#include "client.h"

enum type
{
  KEEP_ALIVE,
  CHOKE,
  UNCHOKE,
  INTERESTED,
  NOT_INTERESTED,
  HAVE,
  BITFIELD
};

int recieve_message(struct peer *p);

#endif
