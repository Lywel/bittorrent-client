#ifndef SOCKET_INIT_H
# define SOCKET_INIT_H

# include <stdint.h>
# include "client.h"

void init_epoll_event(struct peer *peer, int efd);
int peer_socket_init(struct peer *peer);
int peer_connect(struct peer *peer);

#endif
