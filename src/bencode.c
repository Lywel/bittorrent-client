#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bencode.h"
#include "bencode_json.h"
#include "bencode_parser.h"

static struct be_node *
be_node_init(enum be_type type)
{
  struct be_node *node = malloc(sizeof(struct be_node));
  if (node)
  {
    memset(node, 0, sizeof(struct be_node));
    node->type = type;
  }
  return node;
}

struct be_node *
be_decode(char **bencode, long long *size)
{
  struct be_node *node = NULL;

  if (!*size)
    return node;

  switch (**bencode)
  {
  case 'i':
    node = be_node_init(BE_INT);
    node->val.i = bencode_parse_int(bencode, size);
    return node;
  case 'l':
    node = be_node_init(BE_LST);
    node->val.l = bencode_parse_lst(bencode, size);
    return node;
  case 'd':
    node = be_node_init(BE_DIC);
    node->val.d = bencode_parse_dic(bencode, size);
    return node;
  default:
    node = be_node_init(BE_STR);
    node->val.s = bencode_parse_str(bencode, size);
    return node;
  }
}

struct be_node *
bencode_file_decode(char *path)
{
  FILE* f = fopen(path, "r");
  if (!f)
    return NULL;

  fseek(f, 0, SEEK_END);
  long long size = ftell(f);
  rewind(f);

  char *bencode = malloc((size + 1) * sizeof(char));
  if (bencode)
    fread(bencode, sizeof(char), size, f);

  fclose(f);
  struct be_node *n = bencode_decode(bencode, size);
  free(bencode);
  return n;
}

struct be_node *
bencode_decode(char *bencode, long long size)
{
  return be_decode(&bencode, &size);
}

void
bencode_free_node(struct be_node *node)
{
  if (node)
  {
    long long i;
    switch (node->type)
    {
    case BE_LST:
      for (i = 0; node->val.l[i]; ++i)
        bencode_free_node(node->val.l[i]);
      free(node->val.l);
      break;
    case BE_DIC:
      for (i = 0; node->val.d[i]; ++i)
      {
        free(node->val.d[i]->key);
        bencode_free_node(node->val.d[i]->val);
        free(node->val.d[i]);
      }
      free(node->val.d);
      break;
    case BE_STR:
      free(node->val.s);
      break;
    default:
      break;
    }
    free(node);
  }
}

int
bencode_file_pretty_print(char *path)
{
  struct be_node *node = bencode_file_decode(path);
  bencode_dump_json(node);
  bencode_free_node(node);
  return 0;
}
