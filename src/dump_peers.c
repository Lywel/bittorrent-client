#include <stdlib.h>
#include "dump_peers.h"
#include "bencode.h"

int dump_peers(char *path)
{
  struct be_node *node = bencode_file_decode(path);
  bencode_free_node(node);
  return 0;
}
