#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <stdint.h>
#include "recieve_message.h"
#include "client.h"
#include "message.h"
#include "send_message.h"
#include "debug.h"

static void
set_len(uint32_t len, struct message *mess)
{
  len = htonl(len);
  memcpy((void *)mess->len, (void *)&len, 4);
}

int
send_message(void *message, size_t len, struct peer *p)
{
  if (send(p->sfd, message, len, 0) < 0)
  {
    perror("could not send message");
    return -1;
  }
  return 0;  
}

int
send_request_message(struct peer *p, int index, int begin, int length)
{
  struct request req;
  req.index = index;
  req.begin = begin;
  req.length = length;
  req.len[0] = 3;
  req.len[1] = 1;
  req.len[2] = 0;
  req.len[3] = 0;

  return send_message(&req, sizeof(req), p);
}

static struct message
get_message(enum type type)
{
  uint32_t len;
  struct message mess;
  switch (type)
  {
    case INTERESTED:
      len = htonl(1);
      memcpy((void *)mess.len, (void *)&len, 4);
      mess.id = 2;
      return mess;
    default:
      mess.id = 0;
      break;
  }
  return mess;
}
 
int
send_message_type(enum type type, struct peer *p)
{
  struct message mess = get_message(type);
  return send_message(&mess, sizeof(mess), p);
}
