#include <openssl/sha.h>
#include <stdlib.h>
#include "buffer.h"
#include "debug.h"
#include "peer_id.h"
#include "bencode.h"
#include "client.h"
#include "dico_finder.h"

static char *
compute_sha1(s_buf *info)
{
  unsigned char *hash = calloc(20, sizeof(char));
  SHA1((unsigned char *)info->str, info->len, hash);
  return (char *)hash;
}

/*
void
compute_sha1(s_buf *info, unsigned char **digest)
{
  EVP_MD_CTX *mdctx;
  unsigned int len = 20;

  mdctx = EVP_MD_CTX_create();
  EVP_DigestInit_ex(mdctx, EVP_sha1(), NULL);
  *digest = (unsigned char *)OPENSSL_malloc(EVP_MD_size(EVP_sha1()));
  EVP_DigestUpdate(mdctx, info->str, info->len);
  EVP_DigestFinal_ex(mdctx, *digest, &len);

  EVP_MD_CTX_destroy(mdctx);
}
*/

void
init_client(void)
{
  g_bt.verbose = (g_bt.verbose == 1) || 0;

  // Peer id
  g_bt.peer_id = generate_peer_id();

  // Bencode parsing
  g_bt.torrent = bencode_file_decode(g_bt.path);

  // Keeping track of downloaded pieces
  uint32_t pieces_nb = dico_find(dico_find(g_bt.torrent, "info"), "pieces")->val.s->len / 20;
  debug("torrent has %u pieces", pieces_nb);
  uint32_t bytes = pieces_nb / 8 + !!(pieces_nb % 8);
  debug("torrent allocates %u char to store it", bytes);
  char *pieces = calloc(bytes, sizeof(char));
  g_bt.pieces = pieces;
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
