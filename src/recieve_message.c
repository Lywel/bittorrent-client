#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <stdint.h>
#include "message.h"
#include "client.h"
#include "debug.h"
#include "recieve_message.h"

static uint32_t
get_len(struct message mess)
{
  uint32_t *lenn = (uint32_t *)mess.len;
  uint32_t length = ntohl(*lenn);
  return length;
}

static size_t
handle_bitfield(struct message mess)
{
  char *pieces = g_bt.pieces;
  size_t index = 0;

  for (size_t i = 0; i < get_len(mess); ++i)
  {
    char cur = mess.payload[i];
    char have = pieces[i];

    while (cur)
    {
      if (!(have & 1) && (cur & 1))
      {
        debug("I am interrested in piece nb %u", index);
        return index;
      }
      index++;
    }

    have >>= 1;
    cur >>= 1;
  }

  return index;
}

static void
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
    handle_bitfield(mess);
    break;
  }
}

int
recieve_message(struct peer *p)
{
  struct message mess;
  if (recv(p->sfd, &mess, 4096, 0) < 0)
  {
    perror("Could not recieve message");
    return -1;
  }


  uint32_t length = get_len(mess);

  debug("length %d", length);
  debug("message id %u", mess.id);
  debug("payload %s", mess.payload);

  handle_message(mess);
  return 1;
}
