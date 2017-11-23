#include <sys/socket.h>
#include <time.h>
#include <stdio.h>
#include "debug.h"
#include "get_peer_list.h"
#include "network_loop.h"
#include "recieve_message.h"
#include "send_message.h"
#include "handshake.h"
#include "socket_init.h"
#include "socket_close.h"

static int
connect_succeeded(struct peer *peer)
{
    int sfd = peer->sfd;
    int result;
    socklen_t result_len = sizeof(result);
    if (getsockopt(sfd, SOL_SOCKET, SO_ERROR, &result, &result_len) < 0)
    {
      debug("peer droped the connection (getsockopt)");
      perror("getsockopt");
      peer_socket_close(peer);
      return 0;
    }
    if (result != 0)
    {
      debug("connexion failed with error code %d", result);
      peer_socket_close(peer);
      return 0;
    }
    debug("socket available for write or read");
    return 1;
}

static int
handle_event(struct epoll_event evt)
{
  if ((evt.events & EPOLLERR) || (evt.events & EPOLLRDHUP)
    || (evt.events & EPOLLHUP))
  {
    debug("peer droped the connection");
    peer_socket_close(evt.data.ptr);
    return -1;
  }
  struct peer *peer = evt.data.ptr;

  debug("event with peer: %s", peer->ip);
  if ((evt.events & EPOLLIN))
  {
    if (peer->status == P_CO)
      recieve_handshake(peer);
    else
      recieve_message(peer);
  }
  if ((evt.events & EPOLLOUT))
  {
    if (peer->status == P_DECO && connect_succeeded(peer))
    {
      peer->status = P_CO;
      send_handshake(peer);
    }
    if (!peer->am_choking && peer->am_interested && peer->downloaded == 1)
      send_request_message(peer);
  }
  return 0;
}


int
network_loop(int efd, struct epoll_event *events)
{
  time_t loop_start = time(NULL);

  while (1)
  {
    int n = epoll_wait(efd, events, 64, 1000);
    if (n < 0)
    {
      perror("epoll wait timedout");
      continue;
    }
    if (!n)
      debug("EPOOL: NO EVENTS");

    for (int i = 0; i < n; ++i)
      handle_event(events[i]);

    if (time(NULL) - loop_start > g_bt.peer_list_timeout)
    {
      loop_start = time(NULL);
      struct be_node *peers = get_peer_list_from_tracker(g_bt.torrent);
      free(peers);
      for (long long i = 0; g_bt.peers[i]; ++i)
        if (g_bt.peers[i]->sfd < 0 && get_interesting_piece(NULL))
          init_epoll_event(g_bt.peers[i], efd);
    }
  }
  return 0;
}
