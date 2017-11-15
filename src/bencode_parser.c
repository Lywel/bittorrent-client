#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bencode_json.h"
#include "bencode_parser.h"

static char *
bencode_get_content(char *path, long long *size)
{
  FILE* f = fopen(path, "r");
  if (!f)
    return NULL;

  fseek(f, 0, SEEK_END);
  *size = ftell(f);
  rewind(f);

  char *bencode = malloc((*size + 1) * sizeof(char));
  if (bencode)
  {
    fread(bencode, sizeof(char), *size, f);
    bencode[*size] = EOF;
  }

  fclose(f);
  return bencode;
}

static void
bencode_go_until(char end, char **bencode, long long *size)
{
  while (**bencode != end)
  {
    --(*size);
    ++(*bencode);
  }
  **bencode = 0;
  ++(*bencode);
  --(*size);
}

static char *
bencode_parse_str(char **bencode, long long *size)
{
  char *start = *bencode;
  bencode_go_until(':', bencode, size);
  long long len = atoll(start);

  char *str = malloc((len + 1) * sizeof(char));
  if (str)
  {
    strncpy(str, *bencode, len);
    str[len] = 0;
  }
  *bencode += len;
  *size -= len;

  return str;
}

static long long
bencode_parse_int(char **bencode, long long *size)
{
  ++(*bencode);
  --(*size);
  char *start = *bencode;
  bencode_go_until('e', bencode, size);
  return atoll(start);
}

static struct be_node **
bencode_parse_lst(char **bencode, long long *size)
{
  long long nb = 0;
  ++(*bencode);
  --(*size);
  struct be_node** list = NULL;
  while (**bencode != 'e')
  {
    nb++;
    list = realloc(list, (nb + 1) * sizeof(struct be_list *));
    list[nb - 1] = bencode_decode(bencode, size);
  }
  ++(*bencode);
  --(*size);

  if (!nb)
    list = realloc(list, sizeof(struct be_list *));

  list[nb] = NULL;
  return list;
}

static struct be_dico **
bencode_parse_dic(char **bencode, long long *size)
{
  long long nb = 0;
  ++(*bencode);
  --(*size);
  struct be_dico **dico = NULL;
  while (**bencode != 'e')
  {
    nb++;
    dico = realloc(dico, (nb + 1) * sizeof(struct be_dico *));
    dico[nb - 1] = malloc(sizeof(struct be_dico));
    dico[nb - 1]->key = bencode_parse_str(bencode, size);
    dico[nb - 1]->val = bencode_decode(bencode, size);
  }
  ++(*bencode);
  --(*size);

  if (!nb)
    dico = realloc(dico, sizeof(struct be_list *));

  dico[nb] = NULL;
  return dico;
}

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
bencode_decode(char **bencode, long long *size)
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

int
bencode_file_pretty_print(char *path)
{
  long long len = 0;
  char *bencode = bencode_get_content(path, &len);
  struct be_node *node = bencode_decode(&bencode, &len);
  bencode_dump_json(node);
  //bencode_free(node);
  //free(bencode);
  return 0;
}
