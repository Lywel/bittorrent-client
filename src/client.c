#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h> //check if legit
#include <arpa/inet.h>
#include "client.h"
#include "debug.h"
#include "dico_finder.h"

static struct client *client = NULL;

uint16_t
get_port(void)
{
  if (!client)
    return 0;
  uint16_t port = client->info->sin_port;
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

  struct client *cli = malloc(sizeof(client));
  cli->socketfd = sock;
  cli->info = info;

  client = cli; 
  return sock;
}

/**
 * the tracker returns the peers as a list of dic,
 * here peer should be one of them
 */
int
connect_to_peer(struct be_node *peer)
{
  if (!client)
    debug("client not initilized");

  char *ip = dico_find_str(peer, "ip");
  struct hostent *peer_sock = gethostbyname(ip);

  if (!peer_sock)
  {
    perror("Unknown Host");
    return -1;
  }

  client->info->sin_addr = *(struct in_addr *)peer_sock->h_addr_list[0];
  connect(client->socketfd, (struct sockaddr *)client, sizeof(struct sockaddr));
  return 0;
}
/*
int main(void)
{
  int sock = init_socket();
  uint16_t portt = get_port(sock);
}*/
