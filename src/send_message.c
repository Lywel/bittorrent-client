#include "client.h"

int
send_message(void *message, size_t len, struct peer *p)
{
  if (send(p->sfd, message, len, O) < 0)
  {
    perror("could not send message");
    return -1;
  }
  return 1;  
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
/*static struct message
get_message(enum type)
{
  struct message mess;
  switch (type)
  {
  case KEEP_ALIVE:
    mess.len = hton
  }
}*/
