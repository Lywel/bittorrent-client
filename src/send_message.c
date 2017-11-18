#include "client.h"

int
send_message(enum , struct peer *p)
{
  if (send(socketfd, message, sizeof(mess), O) < 0)
  {
    perror("could not send message");
    return -1;
  }

  return 1;  
}

void
send_keep_alive(struct peer *p)
{

}
