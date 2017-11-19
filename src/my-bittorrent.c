#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include "bencode.h"
#include "dump_peers.h"
#include "debug.h"
#include "request_tracker.h"
#include "recieve_message.h"
#include "handshake.h"
#include "dico_finder.h"
#include "socket_init.h"
#include "socket_close.h"
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

static int
event_loop(int efd, struct epoll_event *events)
{
  while (1)
  {
    debug("event loop starts");
    int n = epoll_wait(efd, events, 13, -1);
    if (n < 0)
    {
      perror("epoll wait timedout");
      continue;
    }
    debug("there is %d events to handle", n);
    for (int i = 0; i < n; ++i)
    {
      debug("handling event %d", i);

      //send_handshake(g_bt.peers[i]) + recieve_handshake(g_bt.peers[i] n);
      debug("EPOLLERR: %x", EPOLLERR);
      debug("EPOLLHUP: %x", EPOLLHUP);
      debug("EPOLLIN: %x", EPOLLIN);
      debug("curent event: %x", events[i].events);
      for (long long i = 0; g_bt.peers[i]; ++i)
      {
        if (events[i].data.fd == g_bt.peers[i]->sfd)
        {
          debug("peer: %s", g_bt.peers[i]->ip);
          //recieve_message(g_bt.peers[i]);
          break;
        }
      }
      /*
      if (events[i].events & EPOLLERR)
      {
        debug("epoll error");
        close(events[i].data.fd);
        continue;
      }
      else
      {
      }*/
    }
  }
  return 0;
}

static int download()
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

  struct epoll_event event;

  long long i;
  for (i = 0; g_bt.peers[i]; ++i)
  {
    if (peer_socket_init(g_bt.peers[i])
      + make_socket_non_blocking(g_bt.peers[i]->sfd)
      + peer_connect(g_bt.peers[i]) < 0)
    {
      peer_socket_close(g_bt.peers[i]);
      continue;
    };
    event.data.fd = g_bt.peers[i]->sfd;
    event.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(efd, EPOLL_CTL_ADD, g_bt.peers[i]->sfd, &event) < 0)
      break;
  }
  struct epoll_event *events = calloc(13, sizeof(event));
  int res = event_loop(efd, events);
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
