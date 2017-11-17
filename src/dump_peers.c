#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "request_tracker.h"
#include "dico_finder.h"
#include "bencode.h"
#include "bencode_json.h"
#include "dump_peers.h"
#include "debug.h"

int dump_peers(char *path)
{
  debug("dumping peers for: '%s'", path);
  struct be_node *torrent = bencode_file_decode(path);
  if (!torrent)
    return -1;
  struct be_node *peers = get_peer_list(torrent);
  if (!peers)
    return -1;
  debug("got peer list from server");
  debug("decoding peer list");
  decode_peers_ip(dico_find(peers, "peers"));
  debug("done");
  bencode_dump_json(peers);
  putchar('\n');

  bencode_free_node(peers);
  bencode_free_node(torrent);
  return 0;
}

static struct be_node *
parse_port(char *str)
{
  struct be_node *ip = malloc(sizeof(struct be_node));
  if (ip)
  {
    ip->type = BE_INT;
    uint16_t port_net = *str;
    debug("port: %d", ntohs(port_net));
    ip->val.i = ntohs(port_net);
  }
  return ip;
}

static struct be_node *
parse_ip(char *str)
{
  struct be_node *ip = malloc(sizeof(struct be_node));
  if (ip)
  {
    ip->type = BE_STR;
    char *tmp = calloc(16, sizeof(char));
    debug("ip: %d:%d:%d:%d",
      (uint8_t)str[0], (uint8_t)str[1],
      (uint8_t)str[2], (uint8_t)str[3]);
    sprintf(tmp, "%d:%d:%d:%d",
      (uint8_t)str[0], (uint8_t)str[1],
      (uint8_t)str[2], (uint8_t)str[3]);
    ip->val.s = buffer_init(tmp, strlen(tmp));
  }
  return ip;
}

static struct be_node *
parse_peer_ip(char *str)
{
  struct be_node *peer = malloc(sizeof(struct be_node));
  if (!peer)
    return NULL;

  peer->type = BE_DIC;
  peer->val.d = malloc(3 * sizeof(struct be_dico *));
  if (!peer)
    return NULL;
  peer->val.d[0] = malloc(sizeof(struct be_dico));
  peer->val.d[1] = malloc(sizeof(struct be_dico));
  if (!peer->val.d[0] || !peer->val.d[1])
    return NULL;
  peer->val.d[0]->key = calloc(3, sizeof(char));
  strcpy(peer->val.d[0]->key, "ip");
  peer->val.d[0]->val = parse_ip(str);
  peer->val.d[1]->key = calloc(5, sizeof(char));
  strcpy(peer->val.d[1]->key, "port");
  peer->val.d[1]->val = parse_port(str + 4);
  peer->val.d[2] = NULL;

  return peer;
}

void decode_peers_ip(struct be_node *node)
{
  if (!node || node->type == BE_LST)
    return;

  s_buf *peers = node->val.s;

  struct be_node **res = calloc(peers->len / 6 + 1, sizeof(struct be_node *));
  if (!res)
    return;

  for (long long i = 0; i < peers->len / 6; ++i)
    res[i] = parse_peer_ip(peers->str + i * 6);

  buffer_free(peers);
  node->type = BE_LST;
  node->val.l = res;
}
