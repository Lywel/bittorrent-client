#ifndef SOCKET_INIT_H
# define SOCKET_INIT_H

#include <stdint.h>

uint16_t get_port(void);
int init_socket(void);
int connect_to_peer(struct be_node *peer);

#endif
