#include <string.h>
#include <stdio.h>
#include "bencode.h"
#include "dump_peers.h"
#include "debug.h"
#include "request_tracker.h"
#include "handshake.h"
#include "dico_finder.h"
#include "socket_init.h"
#include "client.h"

struct bittorent g_bt;

static int download()
{
  debug("starting download for %s", g_bt.path);

  get_peer_list(g_bt.torrent);
  debug("got peer list from server");

  peer_connect(g_bt.peers[0]);
  send_handshake(g_bt.peers[0]);
  recieve_handshake(g_bt.peers[0]);
  return 0;
}

static int
print_usage(char *bin)
{
  fprintf(stderr, "Usage: %s /path/to/file.torrent "
                  "[--pretty-print-torrent-file|--dump-peers]\n", bin);
  return -1;
}

static int
parse_args(int argc, char **argv)
{
  char action = 0;

  while (--argc > 0)
  {
    if (!strcmp("--pretty-print-torrent-file", argv[argc]))
      action = 'p';
    else if (!strcmp("--dump-peers", argv[argc]))
      action = 'd';
    else if (!strcmp("--verbose", argv[argc]))
      g_bt.verbose = 1;
    else if (!strncmp("--", argv[argc], 2))
      return print_usage(argv[0]);
    else
      g_bt.path = argv[argc];
  }

  if (!g_bt.path)
    return print_usage(argv[0]);

  client_init();
  switch (action)
  {
  case 'p':
    return bencode_file_pretty_print();
  case 'd':
    return dump_peers();
  default:
    return download();
  }
}

int
main(int argc, char **argv)
{
  if (argc < 2)
    return print_usage(argv[0]);
  return parse_args(argc, argv);
}
