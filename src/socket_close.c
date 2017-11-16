#include <stdlib.h>
#include "client.h"
#include <unistd.h>

void
socket_close(void)
{
  if (close(client.socketfd) < 0)
    perror("can't close socket.");

  free(client.info);
}
