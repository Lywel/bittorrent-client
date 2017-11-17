#ifndef CLIENT_H
# define CLIENT_H

#include "bencode.h"

extern struct client client;

struct client
{
  int socketfd;
  struct sockaddr_in *info;
};

#endif
