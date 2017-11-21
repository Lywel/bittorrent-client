#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "debug.h"
#include "dico_finder.h"
#include "buffer.h"

static void
verbose_bitfield(uint32_t len, char *bytes)
{
  for (uint32_t i = 0; i < len; ++i)
  {
    char cur = bytes[i];
    for (char j = 7; j >= 0; --j)
      verbose("%u", !!((1 << j) & cur));
  }
  verbose("\n");
}

static void
write_data(void *data, struct peer *p, uint32_t len)
{
  struct be_node *info = dico_find(g_bt.torrent, "info");
  struct be_node *files = dico_find(info, "files");
  char *path;
  if (!files)
    path = dico_find_str(info, "name");
  else
    path = NULL;
  FILE *f = fopen(path, "a");
  fseek(f, p->downloading * g_bt.piece_size + p->offset, SEEK_SET);
  fwrite(data, 1, len, f);
  fclose(f);
}

static void
verify_piece(uint32_t id)
{
  id = id;
  // struct be_node *info_dic = dico_find(g_bt.torrent, "info");
  // s_buf *pieces_hash = dico_find_str(info_dic, "pieces")->val.s;
  // TODO:
  // Compute hash while recieveing the data from the peer
  // And store it on the peer struct
  // Then we just have to compare it here with the expected hash
  // if it doesnt match we unset the corecpondding piece byte
  //
  // if (strncmp(pieces_hash->str + id * 20, )
}

static int
recieve_data(struct message mess, struct peer *p)
{
  // TODO: replace fwrite with write to the correcsponding file
  // TODO: Compute the hash while recieving the data
  if (mess.id != 7)
  {
    write_data(&mess, p, sizeof(struct message));
    p->offset += sizeof(struct message);
  }

  char buf[B_SIZE];
  ssize_t read = 0;
  while ((read = recv(p->sfd, buf, B_SIZE, 0)) > 0)
  {
    debug("bytes %d to %d:", p->offset, p->offset + read);
    write_data(buf, p, read);
    p->offset += read;
  }
  if (p->offset > p->last_block + B_SIZE)
  {
    debug("Piece %u : SUCCESS downloaded block %u/%u", p->downloading,
      p->offset / B_SIZE, g_bt.piece_size / B_SIZE);
    p->last_block += B_SIZE;
    p->offset = p->last_block;
    p->last_piece_download_is_finished_and_we_have_to_request_a_new_one = 1;
  }
  if (p->offset >= g_bt.piece_size)
  {
    debug("PIECE %u IS DOWNLOADED", p->downloading);
    verify_piece(p->downloading);
    debug("OUR LOCAL BITFIELD IS:");
    verbose_bitfield(g_bt.pieces_len, g_bt.pieces);
    p->downloading = -1;
  }
  return read;
}

static int
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

  p->downloading = ntohl(piece.index);
  p->offset = ntohl(piece.begin);
  return recieve_data(mess, p);
}

static int
handle_bitfield(struct message mess, struct peer *p)
{
  char *bitfield = malloc((mess.len - 1) * sizeof(char));
  if (!bitfield)
    return -1;

  if (recv(p->sfd, bitfield, mess.len - 1, 0) < 0)
  {
    perror("Could not read bitfield");
    return -1;
  }
  p->am_interested = 1;

  memcpy(p->bitfield, bitfield, g_bt.pieces_len);
  verbose_bitfield(mess.len - 1, bitfield);
  free(bitfield);

  return 0;
}

static int
handle_message(struct message mess, struct peer *p)
{
  verbose_recv(mess, p);
  debug("downloading piece %d", p->downloading);
  switch(mess.id)
  {
  case 0:
    if (mess.len)
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
    if (p->downloading < 0 && mess.len - 1 == g_bt.pieces_len)
      return handle_bitfield(mess, p);
    return recieve_data(mess, p);
  case 7:
    if (p->downloading < 0)
      return recieve_piece(mess, p);
    return recieve_data(mess, p);
  case 8:
    if (mess.len != 13)
      return recieve_data(mess, p);
    return 0;
  case 9:
    if (mess.len != 3)
      return recieve_data(mess, p);
    return 0;
  default:
    debug("recieved continuation data");
    return recieve_data(mess, p);
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
