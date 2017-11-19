#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "bencode.h"
#include "dump_peers.h"
#include "debug.h"
#include "request_tracker.h"
#include "dico_finder.h"
#include "socket_init.h"
#include "socket_close.h"
#include "network_loop.h"
#include "client.h"

struct bittorent g_bt;

static int
make_socket_non_blocking(int sfd)
{
  int flags;
  if ((flags = fcntl(sfd, F_GETFL, 0)) < 0)
  {
    debug("fcntl failed");
    return -1;
  }
  if (fcntl(sfd, F_SETFL, flags | O_NONBLOCK) < 0)
  {
    debug("fcntl failed");
    return -1;
  }
  return 0;
}

static void
init_epoll_event(struct peer *peer, int efd)
{
  struct epoll_event event;
  if (peer_socket_init(peer)
    + make_socket_non_blocking(peer->sfd)
    + peer_connect(peer) < 0)
  {
    debug("FAILED : peer init / connect");
    peer_socket_close(peer);
    return;
  }
  event.data.ptr = peer;
  event.events = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLET;
  epoll_ctl(efd, EPOLL_CTL_ADD, peer->sfd, &event);
  return;
}

static int
download(void)
{
  debug("starting download for %s", g_bt.path);

  get_peer_list(g_bt.torrent);
  debug("peer list is ready");

  int efd = epoll_create1(0);
  if (efd < 0)
  {
    perror ("epoll_create");
    return -1;
  }

  for (long long i = 0; i < 2 && g_bt.peers[i]; ++i)
    init_epoll_event(g_bt.peers[i], efd);

  struct epoll_event *events = calloc(64, sizeof(struct epoll_event));
  int res = network_loop(efd, events);
  free(events);
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
