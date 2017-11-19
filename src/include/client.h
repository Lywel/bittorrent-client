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
  int am_choking;
  int am_interested;
  int peer_choking;
  int peer_interested;
};

struct bittorent
{
  char verbose;
  char *path;
  char *pieces;
  struct be_node *torrent;
  struct peer **peers;
  char *info_hash;
  char *peer_id;
  uint16_t port;
};

extern struct bittorent g_bt;

void client_init(void);

#endif
