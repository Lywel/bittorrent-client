#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <stdint.h>
#include "message.h"
#include "client.h"
#include "debug.h"
#include "recieve_message.h"

int
recieve_message(void)
{
  struct message mess;
  if (recv(client.socketfd, &mess, 4096, 0) < 0)
  {
    perror("Could not recieve message");
    return -1;
  }


  uint32_t *lenn = (uint32_t *)mess.len;
  uint32_t length = ntohl(*lenn);

  debug("length %d", length);
  debug("message id %u", mess.id);
  debug("payload %s", mess.payload);

  handle_message(mess);
  return 1;
}

void
handle_message(struct message mess)
{
  if (mess.len == 0)
  {
    debug("recieved keep alive message");
    return;
  }

  switch(mess.id)
  {
  case 0:
    debug("recieved choke message");
    break;
  case 1:
    debug("recieved unchocke message");
    break;
  case 2:
    debug("recieved interested message");
    break;
  case 3:
    debug("recieved not interested message");
    break;
  case 4:
    debug("recieved have message");
    break;
  case 5:
    debug("recieved bitfield");
    break;
  }
}
