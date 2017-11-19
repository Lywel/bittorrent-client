#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "client.h"
#include "socket_init.h"
#include "debug.h"
#include "dico_finder.h"

/**
 * init a new socket and returns its file descriptor
 */
int
peer_socket_init(struct peer *peer)
{
  struct sockaddr_in *info = malloc(sizeof(struct sockaddr_in));
  int sock = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

  if (sock < 0)
  {
    perror("Can not initialize socket.");
    free(info);
    return -1;
  }

  info->sin_family = AF_INET;
  info->sin_addr.s_addr = INADDR_ANY;

  uint16_t port = 6881;
  info->sin_port = htons(port);

  for (; bind(sock, (struct sockaddr *)info, sizeof(info)) > 0
                    && port <= 6890; ++port)
    info->sin_port = htons(port);

  if (port > 6889)
  {
    perror("Can not use port on range 6881 - 6889.");
    return -1;
  }

  peer->sfd = sock;
  peer->info = info;
  peer->am_interested = 0;
  peer->am_choking = 1;
  peer->peer_choking = 1;
  peer->peer_interested = 0;
  peer->status = P_DECO;
  return 0;
}

/**
 * the tracker returns the peers as a list of dic,
 * here peer should be one of them
 */
int
peer_connect(struct peer *peer)
{
  if (!peer->info)
  {
    debug("client not initilized");
    return -1;
  }

  struct hostent *peer_sock = gethostbyname(peer->ip);
  struct sockaddr_in serv_addr;

  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)peer_sock->h_addr_list[0],
        (char *)&serv_addr.sin_addr.s_addr,
         peer_sock->h_length);

  serv_addr.sin_port = htons(peer->port);

  debug("Peer port : %d", ntohs(serv_addr.sin_port));
  debug("Host name : %s", peer_sock->h_name);

  if (!peer_sock)
  {
    perror("Unknown Host");
    return -1;
  }

  int state = connect(peer->sfd, (struct sockaddr *)&serv_addr,
                      sizeof(serv_addr));
  if (state < 0 && errno != EINPROGRESS)
  {
    perror("connect failed");
    debug("Connection failed");
    return -1;
  }
  verbose("%x%x%x: peers: connect: %s:%u\n", (uint8_t)g_bt.info_hash[0],
    (uint8_t)g_bt.info_hash[1], (uint8_t)g_bt.info_hash[2],
    peer->ip, peer->port);

  return 0;
}
