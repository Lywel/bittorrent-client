#include <stdio.h>
#include <stdlib.h>
#include "debug.h"
#include "bencode_parser.c"

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

  fclose(input_file);
  return bencode;
}

static void
bencode_go_until(char end, char **bencode, long long *size)
{
  while (**bencode != end)
  {
    *size--;
    *bencode++;
  }
  *bencode++;
  *size--;
}

static char *
bencode_parse_str(char **bencode, long long *size)
{
  char **start = bencode;
  bencode_go_until(':', bencode, size);
  long long len = strtoll(*start, *bencode - 1, 10);

  char *str = malloc((len + 1) * sizeof(char));
  if (str)
  {
    strncpy(str, bencode, len);
    str[len] = 0;
  }
  *bencode += len;
  *size -= len;

  return str;
}

static long long
bencode_parse_int(char **bencode, long long *size)
{
  *bencode++;
  *size--;
  char **start = bencode;
  bencode_go_until('e', bencode, size);
  return strtoll(*start, *bencode - 1, 10);
}

static struct be_node **
bencode_parse_lst(char **bencode, long long *size)
{

}

static struct be_node *
bencode_decode(char **bencode, long long *size)
{
  struct be_node *node = NULL;

  if (!*size)
    return node;

  switch (**bencode)
  {
  case '0' ... '9':
    node = be_node_init(BE_STR);
    node->val.s = bencode_parse_str(bencode, size);
    return node;
  case 'i':
    node = be_node_init(BE_INT);
    node->val.i = bencode_parse_int(bencode, size);
    return node;
  case 'l':
    node = bencode_parse_lst(bencode, size);
    return node;
  case 'd':
    node = bencode_parse_dic(bencode, size);
    return node;
  default:
    return node;
  }
}

int
bencode_file_pretty_print(FILE* cout, char *path)
{
  long long len = 0;
  char *bencode = bencode_get_content(path, &len);
  struct be_node *be_tree = bencode_decode(&bencode, &len);

  free(bencode);
}
