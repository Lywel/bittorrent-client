#ifndef HANDSHAKE_H
# define HANDSHAKE_H

int send_handshake(struct be_node *peer);
int recieve_handshake(char *peer);

#endif
