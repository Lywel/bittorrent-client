#include <arpa/inet.h>
#include <stdlib.h>
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

static int
handle_bitfield(struct message mess)
{
  char *pieces = g_bt.pieces;
  int index = -1;

  for (size_t i = 0; i < get_len(mess); ++i)
  {
    char cur = mess.payload[i];
    char have = pieces[i];

    for (char j = 7; j >= 0; --j)
    {
      if (!(have & (1 << j)) && (cur & (1 << j)))
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

  switch(mess.id)
  {
  case 0:
    debug("recieved choke message");
    p->am_choking = 1;
    break;
  case 1:
    debug("recieved unchocke message");
    p->am_choking = 0;
    break;
  case 2:
    debug("recieved interested message");
    p->peer_interested = 1;
    break;
  case 3:
    debug("recieved not interested message");
    p->peer_interested = 0;
    break;
  case 4:
    debug("recieved have message");
    break;
  case 5:
    if ((handle_bitfield(mess)) != -1)
      p->am_interested = 1;
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
  // Filling up the first part of the struct (not reading the actual message)
  if (recv(p->sfd, &mess, sizeof(struct message) - sizeof(char *), 0) < 0)
  {
    perror("Could not read header");
    return -1;
  }

  uint32_t length = get_len(mess);
  debug("length %u", length);
  debug("message id %u", mess.id);

  if (length > 1)
  {
  // Now reading the actual message
    mess.payload = malloc(length * sizeof(char));
    if (recv(p->sfd, mess.payload, length, 0) < 0)
    {
      perror("Could not read message");
      return -1;
    }

    debug("payload :");
    for (unsigned long i = 0; i < length; ++i)
      printf("%x ", mess.payload[i]);
    putchar('\n');
  }

  handle_message(mess, p);
  free(mess.payload);
  return 1;
}
