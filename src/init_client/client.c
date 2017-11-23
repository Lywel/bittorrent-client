#include <openssl/sha.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include "debug.h"
#include "peer_id.h"
#include "bencode.h"
#include "dico_finder.h"

/*static void
adjust_pieces(char *pieces)
{
  int last_zero = 0;
  char cur = g_bt.pieces[g_bt.pieces_len - 1];
  char c = cur;
  for (int i = 0; i < 8; ++i, c >>= 1)
  {
     if (!(c & 0x01) && !last_zero)
       last_zero = i;
     else if (last_zero && c & 0x01)
       last_zero = 0;
  }

  if (last_zero != 0 && last_zero != 7)
    for (int i = last_zero; i < 8; ++i)
      cur |= 1 << i;
}*/

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
  {
    mdctx = EVP_MD_CTX_create();
    EVP_DigestInit_ex(mdctx, EVP_sha1(), NULL);
  }
  else
    mdctx = p->mdctx;

  EVP_DigestUpdate(mdctx, buf, len);
  p->mdctx = mdctx;
}

static void
create_file(struct be_node *n)
{
  char wd[512];
  getcwd(wd, 512);
  char tmp[512];
  struct be_node **list = n->val.l;
  uint32_t i;
  for (i = 0; list[i]; ++i)
  {
    memset(tmp, 0, 512);
    memcpy(tmp, list[i]->val.s->str, list[i]->val.s->len);
    if (list[i + 1])
    {
      mkdir(tmp, 0700);
      chdir(tmp);
    }
  }
  FILE *f = fopen(tmp, "w");
  fclose(f);
  chdir(wd);
}

void
create_files()
{
  struct be_node *info_dic = dico_find(g_bt.torrent, "info");
  struct be_node *files = dico_find(info_dic, "files");
  if (!files)
  {
    char *path = dico_find_str(info_dic, "name");
    FILE *f = fopen(path, "w");
    fclose(f);
  }
  else
  {
    char *path = dico_find_str(info_dic, "name");
    mkdir(path, 0700);
    chdir(path);
    for (uint32_t i = 0; files->val.l[i]; ++i)
      create_file(dico_find(files->val.l[i], "path"));
  }
}

void
init_client(void)
{
  g_bt.verbose = (g_bt.verbose == 1) || 0;

  // Peer id
  g_bt.peer_id = generate_peer_id();

  // Bencode parsing
  g_bt.torrent = bencode_file_decode(g_bt.path);

  // Peer list
  g_bt.peers = NULL;

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
  s_buf *info_be = bencode_encode(info_dic);
  g_bt.info_hash = compute_sha1(info_be);
  buffer_free(info_be);

}

void
free_client(void)
{
  free(g_bt.peer_id);
  bencode_free_node(g_bt.torrent);
  free(g_bt.info_hash);
}
