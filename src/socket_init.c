#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h> //check if legit
#include <arpa/inet.h>
#include "client.h"
#include "debug.h"
#include "dico_finder.h"

#define HANSHAKE_S 68
#define RESERVED_S 8

struct client client = 
{
  0,
  NULL
};

uint16_t
get_port(void)
{
  if (!client.info)
    return 0;
  uint16_t port = client.info->sin_port;
  return ntohs(port);
}

/**
 * init a new socket and returns its file descriptor
 */
int
init_socket(void)
{
  struct sockaddr_in *info = malloc(sizeof(struct sockaddr_in));
  int sock = socket(AF_INET, SOCK_STREAM, 0);

  if (sock < 0)
  {
    perror("Can not initialize socket.");
    exit(errno);
  }

  info->sin_family = AF_INET;
  info->sin_addr.s_addr = INADDR_ANY;

  uint16_t portt = 6881;
  info->sin_port = htons(portt);

  for (; bind(sock, (struct sockaddr *)info, sizeof(info)) > 0
                    && portt <= 6889; ++portt)
    info->sin_port = htons(portt);
  
  if (portt > 6889)
  {
    perror("Can not use port on range 6881 - 6889.");
    exit(1);
  }

  client.socketfd = sock;
  client.info = info;

  return sock;
}

/**
 * the tracker returns the peers as a list of dic,
 * here peer should be one of them
 */
int
connect_to_peer(struct be_node *peer)
{
  if (!client.info)
    debug("client not initilized");

  char *ip = dico_find_str(peer, "ip");
  struct hostent *peer_sock = gethostbyname(ip);

  if (!peer_sock)
  {
    perror("Unknown Host");
    return -1;
  }

  client.info->sin_addr = *(struct in_addr *)peer_sock->h_addr_list[0];
  connect(client.socketfd, (struct sockaddr *)&client, sizeof(struct sockaddr));
  return 0;
}

int
send_handshake(char *peer_id, char *info_hash)
{
  if (!client.info)
    debug("client not initialized");

  char handshake[HANSHAKE_S];
  char reserved[RESERVED_S] = 
  {
    0
  };
  /* pstrlen */
  handshake[0] = 19;
  /* pstr */
  strncpy(handshake + 1, "BitTorrent protocol", 19);
  /* reserved */
  strncpy(handshake + 20, reserved, 8);
  /* info_hash */
  strncpy(handshake + 28, info_hash, 20);
  /* peer_id */
  strncpy(handshake + 48, peer_id, 20);

  if (send(client.socketfd, handshake, HANSHAKE_S, 0) < 0)
  {
    perror("could not send handshake");
    return -1;
  }

  return 1;
}
/*
int main(void)
{
  int sock = init_socket();
  uint16_t portt = get_port(sock);
}*/
