#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
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
  return length - 1;
}

static uint32_t
handle_bitfield(struct message mess)
{
  char *pieces = g_bt.pieces;
  uint32_t index = 0;
  uint32_t len = get_len(mess);

  for (size_t i = 0; i < len; ++i)
  {
    char cur = mess.payload[i];
    char have = pieces[i];

    for (char j = 7; j >= 0 && index < len; --j)
    {
      if (!(have & (1 << j)) && (cur & (1 << j)))
      {
        g_bt.pieces[i] |= 1 << j;
        debug("I am interrested in piece nb %u", index);
        return index;
      }
      index++;
    }
  }

  return index;
}

int
recieve_piece(struct peer *p)
{
  struct piece piece;
  if (recv(p->sfd, &piece, sizeof(struct piece) - sizeof(char *), 0) < 0)
  {
    perror("Could not read piece header");
    return -1;
  }

  uint32_t length = get_len(*(struct message *)&piece) - 9;
  debug("block len : %u", length);
  piece.block = malloc(length * sizeof(char));

  if (recv(p->sfd, piece.block, length, 0) < 0)
  {
    perror("Could not read block");
    return -1;
  }

  return 0;
}

static void
handle_message(struct message mess, struct peer *p)
{
  verbose_recv(mess, p);
  uint32_t ret;
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
    if ((ret = handle_bitfield(mess)) < get_len(mess))
    {
      p->am_interested = 1;
      p->piece_nb = ret;
    }
    break;
  case 7:
    debug("recieved piece");
    break;
  }
}

int
recieve_message(struct peer *p)
{
  debug("recieve_message");
  struct message mess;
  mess.payload = NULL;
  // Filling up the first part of the struct (not reading the actual message)
  if (recv(p->sfd, &mess, sizeof(struct message) - sizeof(char *), 0) < 0)
  {
    perror("Could not read header");
    return -1;
  }

  uint32_t length = get_len(mess);
  debug("length %u", length);
  debug("message id %u", mess.id);

  if (length > 1 && mess.id > 3)
  {
    // Now reading the actual message
    mess.payload = malloc(length * sizeof(char));
    if (recv(p->sfd, mess.payload, length, 0) < 0)
    {
      perror("Could not read message");
      return -1;
    }
  }

  handle_message(mess, p);
  free(mess.payload);
  return 1;
}
