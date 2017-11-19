#include <arpa/inet.h>
#include <string.h>
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

static int
handle_bitfield(struct message mess, struct peer *p)
{
  p->am_interested = 1;
  if (g_bt.pieces_len == get_len(mess))
    memcpy(p->bitfield, mess.payload, g_bt.pieces_len);
  else
    return -1;
  return 0;
}

int
recieve_piece(struct message mess, struct peer *p)
{
  // verify if we finished a block
  // if so checksum the block
  // compare checksum to expected one
  // if ok, write to disk
  // send HAVE to peers
  p = p;
  uint32_t len = get_len(mess);
  for (uint32_t i = 0; i < len; ++i)
    putchar(mess.payload[i]);
  return 0;
}

static int
handle_message(struct message mess, struct peer *p)
{
  verbose_recv(mess, p);
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
    return handle_bitfield(mess, p);
  case 7:
    return recieve_piece(mess, p);
  }
  return 0;
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

  int ret = handle_message(mess, p);
  free(mess.payload);
  return ret;
}
