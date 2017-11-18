#include <stdlib.h>
#include "client.h"
#include <unistd.h>

void
socket_close(struct peer *peer)
{
  if (close(peer->sfd) < 0)
    perror("can't close socket.");

  free(peer->info);
}
