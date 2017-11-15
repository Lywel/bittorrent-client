#include <stdlib.h>
#include "request_tracker.h"
#include "bencode.h"
#include "bencode_json.h"
#include "dump_peers.h"

int dump_peers(char *path)
{
  struct be_node *torrent = bencode_file_decode(path);
  struct be_node *peers = get_peer_list(torrent);
  bencode_dump_json(peers);
  bencode_free_node(peers);
  bencode_free_node(torrent);
  return 0;
}
