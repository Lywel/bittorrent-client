#ifndef CLIENT_H
# define CLIENT_H
# include <stdint.h>

enum peer_status
{
  P_DECO,
  P_CO,
  P_HDSK,
  P_READ
};

struct peer
{
  int sfd;
  struct sockaddr_in *info;
  char *ip;
  uint16_t port;
  char am_choking;
  char am_interested;
  char peer_choking;
  char peer_interested;
  enum peer_status status;
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

void init_client(void);
void free_client(void);

#endif
