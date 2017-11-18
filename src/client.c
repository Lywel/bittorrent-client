#include <stdlib.h>
#include "buffer.h"
#include "peer_id.h"
#include "bencode.h"
#include "hash.h"
#include "client.h"
#include "dico_finder.h"

void
client_init(void)
{
  g_bt.verbose = (g_bt.verbose == 1) || 0;

  // Peer id
  g_bt.peer_id = generate_peer_id();

  // Bencode parsing
  g_bt.torrent = bencode_file_decode(g_bt.path);

  // Keeping track of downloaded pieces
  int pieces_nb = dico_find(dico_find(g_bt.torrent, "info"), "pieces")->val.s->len / 20;
  int bytes = pieces_nb / 8 + pieces_nb % 8 != 0;
  char *pieces = calloc(bytes, 1);
  g_bt.pieces = pieces;

  // Info hash
  s_buf *info = bencode_encode(dico_find(g_bt.torrent, "info"));
  g_bt.info_hash = compute_sha1(info);
  buffer_free(info);
}
