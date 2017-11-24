#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include "recieve_message.h"
#include "debug.h"
#include "bencode.h"
#include "dico_finder.h"
#include "socket_close.h"

int
get_interesting_piece(struct peer *p)
{
  uint32_t index = 0;
  struct be_node *info_dic = dico_find(g_bt.torrent, "info");
  uint32_t pieces_nb = dico_find(info_dic, "pieces")->val.s->len / 20;

  if (!p)
  {
    for (size_t i = 0; i < g_bt.pieces_len; ++i)
    {
      char have = g_bt.pieces[i];
      for (char j = 7; j >= 0 && index < pieces_nb; --j)
      {
        if (!(have & (1 << j)))
          return 1;
        index++;
      }
    }
    return 0;
  }

  debug("looking for an interesting bit in %u bits", g_bt.pieces_len);
  for (size_t i = 0; i < g_bt.pieces_len; ++i)
  {
    char cur = p->bitfield[i];
    char have = g_bt.pieces[i];
    for (char j = 7; j >= 0 && index < pieces_nb; --j)
    {
      if (!(have & (1 << j)) && (cur & (1 << j)))
      {
        g_bt.pieces[i] |= 1 << j;
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
  if (p->downloading < 0)
  {
    p->downloading = get_interesting_piece(p);
    p->offset = 0;
  }
  if (p->downloading == -1)
  {
    peer_socket_close(p);
    return -1;
  }
  struct be_node *info_dic = dico_find(g_bt.torrent, "info");
  uint32_t pieces_nb = dico_find(info_dic, "pieces")->val.s->len / 20;

  debug("requesting piece nb %d with an offset of %d",
         p->downloading, p->offset);
  p->downloaded = 2;
  verbose("%x%x%x: msg: send: %s:%u: ", (uint8_t)g_bt.info_hash[0],
          (uint8_t)g_bt.info_hash[1], (uint8_t)g_bt.info_hash[2],
          p->ip, p->port);

    struct request req;
    req.id = 6;
    req.index = htonl(p->downloading);
    req.begin = htonl(p->offset);

    if (p->downloading == (int)pieces_nb - 1
      && p->offset + B_SIZE >= g_bt.torrent_size % g_bt.piece_size)
    {
      debug("REQUESTING THE LEGENDARY 4 MISSING PERCENTS !!!\n requesting %u bytes.", g_bt.torrent_size);
      req.length = htonl(g_bt.torrent_size % g_bt.piece_size - p->offset);
    }
    else
      req.length = htonl(B_SIZE);

  p->requested_size = ntohl(req.length);
  req.len = htonl(13);

  verbose("request %d %d %d\n", p->downloading, p->offset, B_SIZE);

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
  verbose_send(mess, p);
  return send_message(&mess, sizeof(mess), p);
}
