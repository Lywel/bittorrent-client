#include <stdio.h>
#include <stdlib.h>
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

struct be_node *
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
    node = be_node_init(BE_LST);
    // TODO: Parse lists
    return node;
  case 'd':
    node = be_node_init(BE_DIC);
    // TODO: Parse dictionaries
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
