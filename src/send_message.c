#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include "recieve_message.h"
#include "debug.h"

static uint32_t
get_interesting_piece(struct peer *p)
{
  // TODO : should not check the last bits of the last byte
  uint32_t index = 0;
  debug("looking for an interesting bit in %u bits", g_bt.pieces_len);
  for (size_t i = 0; i < g_bt.pieces_len; ++i)
  {
    char cur = p->bitfield[i];
    char have = g_bt.pieces[i];
    for (char j = 7; j >= 0; --j)
    {
      if (!(have & (1 << j)) && (cur & (1 << j)))
      {
        g_bt.pieces[i] |= 1 << j;
        p->downloading = index;
        p->offset = 0;
        return index;
      }
      index++;
    }
  }
  return -1;
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
send_request_message(struct peer *p)
{
  uint32_t index;
  if (p->downloading < 0)
    index = get_interesting_piece(p);
  else
    index = p->downloading;

  debug("requesting piece nb %d with an offset of %d", index, p->offset);

  struct request req;

  req.id = 6;

  req.index = htonl(index);
  req.begin = htonl(p->offset);
  req.length = htonl(B_SIZE);
  req.len = htonl(13);

  return send_message(&req, sizeof(req), p);
}

static struct message
get_message(enum type type, struct peer *p)
{
  struct message mess;
  switch (type)
  {
    case INTERESTED:
      p->am_interested = 1;
      mess.len = htonl(1);
      mess.id = 2;
      return mess;
    case NOT_INTERESTED:
      p->am_interested = 0;
      mess.len = htonl(1);
      mess.id = 3;
      return mess;
    case CHOKE:
      p->peer_choking = 1;
      mess.len = htonl(1);
      mess.id = 0;
      return mess;
    case UNCHOKE:
      p->peer_choking = 0;
      mess.len = htonl(1);
      mess.id = 1;
      return mess;
    default:
      mess.len = htonl(-1);
      mess.id = -1;
      return mess;
  }
}

int
send_message_type(enum type type, struct peer *p)
{
  struct message mess = get_message(type, p);
  return send_message(&mess, sizeof(mess), p);
}
