#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "request_tracker.h"
#include "dico_finder.h"
#include "bencode.h"
#include "bencode_json.h"
#include "dump_peers.h"
#include "debug.h"
#include "socket_init.h"
#include "handshake.h"
#include "peer_id.h"
#include "hash.h"

int dump_peers(char *path)
{
  debug("dumping peers for: '%s'", path);
  struct be_node *torrent = bencode_file_decode(path);
  struct be_node *peers = dico_find(get_peer_list(torrent), "peers");

  debug("got peer list from server");

  for (long long i = 0; peers->val.l[i]; ++i)
    printf("%s:%lld\n", dico_find_str(peers->val.l[i], "ip"),
                    dico_find_int(peers->val.l[i], "port"));

  bencode_free_node(peers);
  bencode_free_node(torrent);
  return 0;
}
