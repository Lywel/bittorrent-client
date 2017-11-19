#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
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
send_request_message(struct peer *p, int begin)
{
  debug("requesting piece nb %d", p->piece_nb);
  struct request req;

  req.id = 6;

  req.index = htonl(p->piece_nb);
  req.begin = htonl(begin);
  req.length = htonl(B_SIZE);

  req.len[0] = 0;
  req.len[1] = 0;
  req.len[2] = 0;
  req.len[3] = 13;

  return send_message(&req, sizeof(req), p);
}

static struct message
get_message(enum type type, struct peer *p)
{
  struct message mess;
  mess.payload = NULL;
  switch (type)
  {
    case INTERESTED:
      p->am_interested = 1;
      set_len(1, &mess);
      mess.id = 2;
      return mess;
    case NOT_INTERESTED:
      p->am_interested = 0;
      set_len(1, &mess);
      mess.id = 3;
      return mess;
    case CHOKE:
      p->peer_choking = 1;
      set_len(1, &mess);
      mess.id = 0;
      return mess;
    case UNCHOKE:
      p->peer_choking = 0;
      set_len(1, &mess);
      mess.id = 1;
      return mess;
    default:
      return mess;
  }
}

int
send_message_type(enum type type, struct peer *p)
{
  struct message mess = get_message(type, p);
  return send_message(&mess, sizeof(mess), p);
}
