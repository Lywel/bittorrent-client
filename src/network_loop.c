#include <sys/socket.h>
#include <stdio.h>
#include "debug.h"
#include "network_loop.h"
#include "recieve_message.h"
#include "send_message.h"
#include "handshake.h"
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

int
network_loop(int efd, struct epoll_event *events)
{
  while (1)
  {
    int n = epoll_wait(efd, events, 64, 1000);
    if (n < 0)
    {
      perror("epoll wait timedout");
      continue;
    }
    debug("event_wait return %d events", n);

    for (int i = 0; i < n; ++i)
    {
      struct epoll_event evt = events[i];
      if ((evt.events & EPOLLERR) || (evt.events & EPOLLRDHUP)|| (evt.events & EPOLLHUP))
      {
        debug("peer droped the connection");
        peer_socket_close(evt.data.ptr);
        continue;
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
        if (!peer->am_choking && peer->am_interested)
          send_request_message(peer);
      }
    }
  }
  return 0;
}
