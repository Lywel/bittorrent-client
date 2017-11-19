#ifndef RECIEVE_MESSAGE_H
# define RECIEVE_MESSAGE_H
# include "client.h"
# include "message.h"

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
int recieve_piece(struct message mess, struct peer *p);

#endif
