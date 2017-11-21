#include <openssl/sha.h>
#include <stdlib.h>
#include "debug.h"
#include "peer_id.h"
#include "bencode.h"
#include "dico_finder.h"

static char *
compute_sha1(s_buf *info)
{
  unsigned char *hash = calloc(20, sizeof(char));
  SHA1((unsigned char *)info->str, info->len, hash);
  return (char *)hash;
}

void
update_sha1(struct peer *p, char *buf, unsigned int len)
{
  EVP_MD_CTX *mdctx;
  if (!p->mdctx)
    mdctx = EVP_MD_CTX_create();
  else
    mdctx = p->mdctx;

  EVP_DigestInit_ex(mdctx, EVP_sha1(), NULL);
  EVP_DigestUpdate(mdctx, buf, len);
}

void
init_client(void)
{
  g_bt.verbose = (g_bt.verbose == 1) || 0;

  // Peer id
  g_bt.peer_id = generate_peer_id();

  // Bencode parsing
  g_bt.torrent = bencode_file_decode(g_bt.path);

  struct be_node *info_dic = dico_find(g_bt.torrent, "info");
  // Keeping track of downloaded pieces
  uint32_t pieces_nb = dico_find(info_dic, "pieces")->val.s->len / 20;
  uint32_t piece_size = dico_find_int(info_dic, "piece length");
  debug("torrent has %u pieces of size %u", pieces_nb, piece_size);
  uint32_t bytes = pieces_nb / 8 + !!(pieces_nb % 8);
  debug("torrent allocates %u char to store it", bytes);
  char *pieces = calloc(bytes, sizeof(char));
  g_bt.pieces = pieces;
  g_bt.piece_size = piece_size;
  g_bt.pieces_len = bytes;

  // Info hash
  s_buf *info = bencode_encode(dico_find(g_bt.torrent, "info"));
  g_bt.info_hash = compute_sha1(info);
  buffer_free(info);
}

void
free_client(void)
{
  free(g_bt.peer_id);
  bencode_free_node(g_bt.torrent);
  free(g_bt.info_hash);
}
