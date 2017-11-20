#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include "debug.h"

#define HANSHAKE_S 68
#define RESERVED_S 8

int
send_handshake(struct peer *peer)
{
  debug("building handshake");
  if (!peer->info)
  {
    debug("client not initialized");
    return -1;
  }

  char handshake[HANSHAKE_S];
  char reserved[RESERVED_S] =
  {
    0
  };
  /* pstrlen */
  handshake[0] = 19;
  /* pstr */
  strncpy(handshake + 1, "BitTorrent protocol", 19);
  /* reserved */
  strncpy(handshake + 20, reserved, 8);
  /* info_hash */
  strncpy(handshake + 28, g_bt.info_hash, 20);
  /* peer_id */
  strncpy(handshake + 48, g_bt.peer_id, 20);

  if (send(peer->sfd, handshake, HANSHAKE_S, 0) < 0)
  {
    perror("could not send handshake");
    return -1;
  }

  debug("handshaked sent: '%s'", handshake);
  return 0;
}

/**
 * Here peer_id should be the one of the remote
 * peer. We need to check if it matches the one
 * recieved in the handshake
 */
int
recieve_handshake(struct peer *peer)
{
  if (!peer->info)
  {
    debug("client not initialized");
    return -1;
  }

  char handshake[HANSHAKE_S];
  if (recv(peer->sfd, handshake, HANSHAKE_S, 0) < 0)
  {
    perror("could not recieve handshake");
    return -1;
  }

  peer->status = P_HDSK;
  debug("handshake: '%*s'", HANSHAKE_S, handshake);
  return 0;
}
