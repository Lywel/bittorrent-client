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
void update_sha1(struct peer *p, char *buf, unsigned int len);

#endif
