#include <stdlib.h>
#include "request_tracker.h"
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
  bencode_dump_json(peers);
  bencode_free_node(peers);
  bencode_free_node(torrent);
  return 0;
}

static struct be_node *
parse_ip(char *str)
{
  struct be_node *ip = malloc(sizeof(struct be_node));
  if (ip)
  {
    ip->type = BE_INT;
    uint16_t port_net = *((void *)str);
    ip->val.i = ntohs(port_net);
  }
  return ip;
}

static struct be_node *
parse_port(char *str)
{
  struct be_node *ip = malloc(sizeof(struct be_node));
  if (ip)
  {
    ip->type = BE_STR;
    char *tmp = calloc(16, sizeof(char))
    sscanf(tmp, "%d:%d:%d:%d",
      ntohs(str[0]), ntohs(str[1]), ntohs(str[2]), ntohs(str[3]));
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
  peer->val.d[0]->key = "ip";
  peer->val.d[0]->val = parse_ip(str);
  peer->val.d[1]->key = "port";
  peer->val.d[1]->val = parse_port(str + 4);

  return peer;
}

void decode_peers_ip(struct be_node **node)
{
  if (!node || peers->type == BE_LST)
    return;

  s_buf *peers = node->->val.s;
  struct be_node *res = malloc(sizeof(struct be_node));
  if (!res)
    return;

  res->type = BE_LST;
  res->val.l = calloc(peers->len / 6 + 1, sizeof(struct be_node *));
  for (long long i = 0; i < peers->len / 6; ++i)
    res->val[i] = parse_peer_ip(peers->val + i * 6);
  res->val[peers->len / 6] = NULL;

  buffer_free(res);
  *node = res;
}
