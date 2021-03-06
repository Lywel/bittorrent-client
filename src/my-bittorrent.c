#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include "bencode.h"
#include "bencode_json.h"
#include "get_peer_list.h"
#include "debug.h"
#include "dico_finder.h"
#include "socket_init.h"
#include "socket_close.h"
#include "network_loop.h"

#define NI_MAXHOST 1025

struct bittorent g_bt;

static int
download(void)
{
  debug("starting download for %s", g_bt.path);

  struct be_node *peers = get_peer_list_from_tracker(g_bt.torrent);
  create_files();

  debug("peer list is ready");

  int efd = epoll_create1(0);
  if (efd < 0)
  {
    perror ("epoll_create");
    return -1;
  }

  for (long long i = 0; g_bt.peers[i]; ++i)
    init_epoll_event(g_bt.peers[i], efd);

  struct epoll_event *events = calloc(64, sizeof(struct epoll_event));
  int res = network_loop(efd, events);
  free(events);
  bencode_free_node(peers);
  return res;
}

static int
print_usage(char *bin)
{
  fprintf(stderr, "Usage: %s /path/to/file.torrent "
                  "[--pretty-print-torrent-file|--dump-peers]\n", bin);
  return -1;
}

static int
pretty_print_torrent_file(void)
{
  bencode_dump_json(g_bt.torrent);
  return 0;
}

static int
dump_peers(struct peer **peers)
{
  struct be_node *peer_list = get_peer_list_from_tracker(g_bt.torrent);
  struct be_node *peer = dico_find(peer_list, "peers");

  for (long long i = 0; peers[i]; ++i)
  {
    char hbuf[NI_MAXHOST];
    if (getnameinfo((struct sockaddr *)peers[i]->info,
          sizeof(struct sockaddr_in), hbuf, sizeof(hbuf), NULL, 0,
          NI_NUMERICSERV | NI_NUMERICHOST))
      printf("%s:%lld\n", dico_find_str(peer->val.l[i], "ip"),
                    dico_find_int(peer->val.l[i], "port"));
    else
      printf("%s:%lld\n", hbuf, dico_find_int(peer->val.l[i], "port"));
  }

  bencode_free_node(peer_list);
  return 0;
}

static int
parse_args(int argc, char **argv)
{
  char action = 0;
  g_bt.verbose = 0;

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

  init_client();
  int res = 0;
  struct be_node *sl = NULL;  
  switch (action)
  {
  case 'p':
    res = pretty_print_torrent_file();
    break;
  case 'd':
    get_peer_list_from_tracker(g_bt.torrent);
    free(sl);
    res = dump_peers(g_bt.peers);
    break;
  default:
    res = download();
  }
  free_client();
  return res;
}

int
main(int argc, char **argv)
{
  if (argc < 2)
    return print_usage(argv[0]);
  return parse_args(argc, argv);
}
