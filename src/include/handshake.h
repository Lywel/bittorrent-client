#ifndef HANDSHAKE_H
# define HANDSHAKE_H
# include "client.h"

int send_handshake(struct peer *peer);
int recieve_handshake(struct peer *peer);

#endif
