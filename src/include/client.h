#ifndef CLIENT_H
# define CLIENT_H

#include "bencode.h"

extern struct client client;

struct client
{
  int socketfd;
  struct sockaddr_in *info;
};

uint16_t get_port(void);
int init_socket(void);
int connect_to_peer(struct be_node *peer);
int send_handshake(char *peer_id, char *info_hash);

#endif
