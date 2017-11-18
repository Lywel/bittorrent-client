#ifndef CLIENT_H
# define CLIENT_H

# include <stdint.h>
# include "bencode.h"

struct peer
{
  int sfd;
  struct sockaddr_in *info;
  char *ip;
  uint16_t port;
};

struct bittorent
{
  char verbose;
  char *path;
  struct be_node *torrent;
  struct peer **peers;
  char *info_hash;
  char *peer_id;
  uint16_t port;
};

struct bittorent g_bt;

void client_init(void);

#endif
