#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "debug.h"
#include "recieve_message.h"
#include "bencode.h"
#include "dico_finder.h"
#include "buffer.h"

static void
verbose_bitfield(uint32_t len, char *bytes)
{
  struct be_node *info_dic = dico_find(g_bt.torrent, "info");
  uint32_t pieces_nb = dico_find(info_dic, "pieces")->val.s->len / 20;
  uint32_t index = 0;
  for (uint32_t i = 0; i < len; ++i)
  {
    char cur = bytes[i];
    for (char j = 7; j >= 0 && index < pieces_nb; --j, ++index)
      verbose("%u", !!((1 << j) & cur));
  }
  verbose("\n");
}

static char *
get_file_path(struct be_node *n, char *p)
{
  char *res = p;
  memset(p, 0, 512);
  struct be_node **list = dico_find(n, "path")->val.l;
  for (uint32_t i = 0; list[i]; ++i)
  {
    memcpy(p, list[i]->val.s->str, list[i]->val.s->len);
    p += list[i]->val.s->len + 1;
    *(p - 1) = list[i+1] ? '/' : 0;
  }
  return res;
}

static void
write_data(char *data, struct peer *p, uint32_t len)
{
  if (p->downloading < 0)
    return;
  struct be_node *info = dico_find(g_bt.torrent, "info");
  struct be_node *files = dico_find(info, "files");
  uint32_t pos = p->downloading * g_bt.piece_size + p->offset;

  if (!files)
  {
    FILE *f = fopen(dico_find_str(info, "name"), "r+");
    fseek(f, pos, SEEK_SET);
    fwrite(data, 1, len, f);
    fclose(f);
  }
  else
  {
    uint32_t acc = 0;
    uint32_t i = 0;
    uint32_t flen = 0;
    while (acc < pos)
    {
      flen = dico_find_int(files->val.l[i], "length");
      if (acc + flen > pos)
        break;
      acc += flen;
      i++;
    }
    char path[512];
    uint32_t written = 0;
    FILE *f = fopen(get_file_path(files->val.l[i], path), "r+");

    flen = dico_find_int(files->val.l[i], "length");
    debug("%u bytes to write on byte %u of %u bytes file", len, pos - acc, flen);
    fseek(f, pos - acc, SEEK_SET);

    acc = 0;
    while ((written = fwrite(data, 1, len < flen ? len: flen, f)) < len)
    {
      debug("writting %u/%u bytes to %s",
          written, len, get_file_path(files->val.l[i], path));
      acc += written;
      len -= written;
      data += written;
      if (acc >= flen)
      {
        i++;
        flen = dico_find_int(files->val.l[i], "length");
        fclose(f);
        f = fopen(get_file_path(files->val.l[i], path), "r+");
      }
    }
    fclose(f);
  }
}

  static void
  verify_piece(struct peer *p)
  {
    unsigned int len = 0;
    unsigned char final_hash[20];
    EVP_DigestFinal_ex(p->mdctx, final_hash, &len);
    struct be_node *info_dic = dico_find(g_bt.torrent, "info");
    char *pieces_hash = dico_find_str(info_dic, "pieces");
    if (memcmp(pieces_hash + p->downloading * 20, final_hash, 20))
    {
      EVP_MD_CTX_destroy(p->mdctx);
      p->mdctx = NULL;
      p->downloaded = 1;
    }
    else
    {
      debug("PIECE NB %d NOT VERIFIED :(\n", p->downloading);
      g_bt.pieces[p->downloading / 8] &= ~(1 << (7 - p->downloading % 8));
    }
  }

static int
recieve_data(struct message mess, struct peer *p)
{
  if (mess.id != 7)
  {
    write_data((void *)&mess, p, sizeof(struct message));
    p->offset += sizeof(struct message);
  }

  char buf[B_SIZE];
  ssize_t read = 0;
  while ((read = recv(p->sfd, buf, B_SIZE, 0)) > 0)
  {
    update_sha1(p, buf, read);
    debug("bytes %d to %d:", p->offset, p->offset + read);
    write_data(buf, p, read);
    p->offset += read;
  }
  if (p->offset >= p->last_block + B_SIZE)
  {
    verbose("%x%x%x: msg: recv: %s:%u: piece %u %u\n",
        (uint8_t)g_bt.info_hash[0], (uint8_t)g_bt.info_hash[1],
        (uint8_t)g_bt.info_hash[2], p->ip, p->port, p->downloading, p->offset);

    debug("Piece %u : SUCCESS downloaded block %u/%u", p->downloading,
      p->offset / B_SIZE, g_bt.piece_size / B_SIZE);

    p->last_block += B_SIZE;
    p->offset = p->last_block;
    p->downloaded = 1;
  }
  if (p->offset >= g_bt.piece_size)
  {
    debug("PIECE %u IS DOWNLOADED", p->downloading);
    verify_piece(p);
    debug("OUR LOCAL BITFIELD IS:");
    verbose_bitfield(g_bt.pieces_len, g_bt.pieces);
    p->downloading = -1;
  }
  return read;
}

static int
recieve_piece(struct message mess, struct peer *p)
{
  struct piece piece;
  if (recv(p->sfd, &piece, sizeof(struct piece), 0) < 0)
  {
    perror("receive_piece : could not read message");
    return -1;
  }

  p->downloaded = 0;
  verbose("%u %u\n", ntohl(piece.index), ntohl(piece.begin));

  p->downloading = ntohl(piece.index);
  p->offset = ntohl(piece.begin);
  p->last_block = p->offset;
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
    if (p->downloaded == 2)
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
