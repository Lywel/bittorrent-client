#ifndef HANDSHAKE_H
# define HANDSHAKE_H

int send_handshake(char *peer_id, char *info_hash);
int recieve_handshake(struct be_node *peer);

#endif
