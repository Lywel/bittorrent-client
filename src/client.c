#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h> //check if legit
#include <arpa/inet.h>

static uint16_t port;

uint16_t
get_port(int socket)
{
  return ntohs(port);
}

/**
 * init a new socket and returns its file descriptor
 */
int
init_socket(void)
{
  struct sockaddr_in *client = malloc(sizeof(struct sockaddr_in));
  int sock = socket(AF_INET, SOCK_STREAM, 0);

  if (sock < 0)
  {
    perror("Can not initialize socket.");
    exit(errno);
  }

  client->sin_family = AF_INET;
  client->sin_addr.s_addr = INADDR_ANY;

  uint16_t portt = 6881;
  client->sin_port = htons(portt);

  for (; bind(sock, (struct sockaddr *)client, sizeof(client)) > 0
                    && port <= 6889; ++portt)
    client->sin_port = htons(portt);

  port = client->sin_port;
  return sock;
}
/*
int main(void)
{
  int sock = init_socket();
  uint16_t portt = get_port(sock);
}*/
