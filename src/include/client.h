#ifndef CLIENT_H
# define CLIENT_H

#include "bencode.h"

struct client
{
  int socketfd;
  struct sockaddr_in *info;
};

struct bittorent
{
  char *path;
  char verbose;
  char *peer_id;
  struct be_node *dico;
  char *info_hash;
  struct peer **peers;
};

struct bittorent g_bt;

void client_init(void);

#endif
