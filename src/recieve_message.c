#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <stdint.h>
#include "message.h"
#include "client.h"
#include "debug.h"
#include "recieve_message.h"
#include "send_message.h"

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
  size_t index = -1;

  for (size_t i = 0; i < get_len(mess); ++i)
  {
    char cur = mess.payload[i];
    char have = pieces[i];

    for (char i = 7; i >= 0; --i)
    {
      if (!(have & (1 << i)) && (cur & (1 << i)))
      {
        debug("I am interrested in piece nb %u", index + 1);
        return index + 1;
      }
      index++;
    }
  }

  return index;
}

static void
handle_message(struct message mess, struct peer *p)
{
  if (mess.len == 0)
  {
    debug("recieved keep alive message");
    return;
  }

  int i;
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
    if ((i = handle_bitfield(mess)) != -1)
      send_message_type(INTERESTED, p);
    break;
  case 7:
    debug("recieved piece");
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

  debug("length %u", length);
  debug("message id %u", mess.id);
  debug("payload :");
  for (unsigned long i = 0; i < length; ++i)
    printf("%x ", mess.payload[i]);
  putchar('\n');

  handle_message(mess, p);
  return 1;
}
