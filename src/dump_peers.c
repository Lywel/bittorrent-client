#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "request_tracker.h"
#include "dico_finder.h"
#include "dump_peers.h"
#include "client.h"
#include "debug.h"

int dump_peers()
{
  debug("dumping peers for: '%s'", g_bt.path);
  struct be_node *peers = dico_find(get_peer_list(g_bt.dico), "peers");

  debug("got peer list from server");

  for (long long i = 0; peers->val.l[i]; ++i)
    printf("%s:%lld\n", dico_find_str(peers->val.l[i], "ip"),
                    dico_find_int(peers->val.l[i], "port"));

  bencode_free_node(peers);
  return 0;
}
