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
    return -1;
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
  struct sockaddr_in serv_addr;

  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)peer_sock->h_addr_list[0],
        (char *)&serv_addr.sin_addr.s_addr,
         peer_sock->h_length);

  serv_addr.sin_port = htons((uint16_t)dico_find_int(peer, "port"));

  debug("Peer port : %d", ntohs(serv_addr.sin_port));
  debug("Host name : %s", peer_sock->h_name);

  if (!peer_sock)
  {
    perror("Unknown Host");
    return -1;
  }

  if (connect(client.socketfd, (struct sockaddr *)&serv_addr,
                               sizeof(serv_addr)) < 0)
    debug("Connection failed");

  return 0;
}
