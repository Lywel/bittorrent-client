#include <string.h>
#include <stdio.h>
#include "bencode.h"
#include "dump_peers.h"
#include "debug.h"
#include "request_tracker.h"
#include "handshake.h"
#include "dico_finder.h"
#include "hash.h"
#include "socket_init.h"
#include "peer_id.h"
#include "recieve_message.h"

static int download(char *path)
{
  debug("starting download for %s", path);

  struct be_node *torrent = bencode_file_decode(path);
  struct be_node *peers = get_peer_list(torrent);
  debug("got peer list from server");

  struct be_node *peers_ip = dico_find(peers, "peers");
  s_buf *info = bencode_encode(dico_find(torrent, "info"));
  char *info_hash = compute_sha1(info);

  connect_to_peer(peers_ip->val.l[0]);
  send_handshake(generate_peer_id(), info_hash);
  recieve_handshake(peers_ip->val.l[0]);
  recieve_message();
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
  char *torrent = NULL;
  char action = 0;

  while (--argc > 0)
  {
    if (!strcmp("--pretty-print-torrent-file", argv[argc]))
      action = 'p';
    else if (!strcmp("--dump-peers", argv[argc]))
      action = 'd';
    else if (!strncmp("--", argv[argc], 2))
      return print_usage(argv[0]);
    else
      torrent = argv[argc];
  }

  if (!torrent)
    return print_usage(argv[0]);

  switch (action)
  {
  case 'p':
    return bencode_file_pretty_print(torrent);
  case 'd':
    return dump_peers(torrent);
  default:
    return download(torrent);
  }
}

int
main(int argc, char **argv)
{
  if (argc < 2)
    return print_usage(argv[0]);
  return parse_args(argc, argv);
}
