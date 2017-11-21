#ifndef CLIENT_H
# define CLIENT_H

#include <stdint.h>
#include <openssl/evp.h>

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
  uint32_t offset;
  uint32_t last_block;
  char *bitfield;
  EVP_MD_CTX *mdctx;
  int downloading;
  char downloaded;
  enum peer_status status;
};

struct bittorent
{
  char verbose;
  char *path;
  char *pieces;
  uint32_t pieces_len;
  uint32_t piece_size;
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
