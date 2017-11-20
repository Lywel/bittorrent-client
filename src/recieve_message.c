#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "debug.h"

static void
verbose_bitfield(uint32_t len, char *bytes)
{
  for (size_t i = 0; i < len; ++i)
  {
    char cur = bytes[i];
    for (char j = 7; j >= 0; --j)
      verbose("%u", !!((1 << j) & cur));
  }
  verbose("\n");
}

static int
handle_bitfield(struct message mess, struct peer *p)
{
  if (mess.len - 1 > g_bt.pieces_len)
    return -1;
  char *bitfield = malloc((mess.len - 1) * sizeof(char));
  if (!bitfield)
    return -1;

  if (recv(p->sfd, bitfield, mess.len - 1, 0) < 0)
  {
    perror("Could not read bitfield");
    return -1;
  }
  p->am_interested = 1;

  if (g_bt.pieces_len == mess.len - 1)
  {
    memcpy(p->bitfield, bitfield, g_bt.pieces_len);
    verbose_bitfield(mess.len - 1, bitfield);
  }
  free(bitfield);

  return -1 * !!(g_bt.pieces_len != mess.len - 1);
}

int
recieve_piece(struct message mess, struct peer *p)
{
  // verify if we finished a block
  // if so checksum the block
  // compare checksum to expected one
  // if ok, write to disk
  // send HAVE to peers
  uint32_t blk_len = mess.len - 8;
  struct piece piece;
  if (recv(p->sfd, &piece, sizeof(struct piece), 0) < 0)
  {
    perror("receive_piece : could not read message");
    return -1;
  }
  debug("recieving block %u of size %u beggining at %u",
    ntohl(piece.index), blk_len, ntohl(piece.begin));

  char buf[4094];
  ssize_t read = 0;
  ssize_t offset = 0;
  while ((read = recv(p->sfd, buf, 1024, 0)) > 0)
  {
    debug("bytes %d to %d:", offset, offset + read);
    fwrite(buf, 1, read, stdout);
    puts("");
    offset += read;
    //write_file(buf, len, p->req_blk, p->req_offset);
  }
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

  if (recv(p->sfd, &mess, sizeof(struct message), 0) < 0)
  {
    perror("Could not read header");
    return -1;
  }
  mess.len = ntohl(mess.len);
  debug("length %u", mess.len);
  debug("message id %u", mess.id);

  return handle_message(mess, p);
}
