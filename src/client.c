#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h> //check if legit
#include <arpa/inet.h>

/**
 * init a new socket and returns the ports it's init on
 */
uint32_t
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

  uint32_t port = 6881;
  client->sin_port = htons(port);

  for (; bind(sock, (struct sockaddr *)client, sizeof(client)) > 0
                    && port <= 6889; ++port)
    client->sin_port = htons(port);

  return port;
}
