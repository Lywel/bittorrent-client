#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bencode.h"
#include "bencode_json.h"
#include "bencode_parser.h"

struct be_node *
bencode_file_decode(char *path)
{
  FILE* f = fopen(path, "r");
  if (!f)
    return NULL;

  fseek(f, 0, SEEK_END);
  long long size = ftell(f);
  rewind(f);

  char *bencode = calloc(size + 1, sizeof(char));
  if (bencode)
    fread(bencode, sizeof(char), size, f);
  fclose(f);
  struct be_node *n = bencode_decode(bencode);
  free(bencode);
  return n;
}

char *
bencode_encode(char *bencode)
{
  return bencode;
}

struct be_node *
bencode_decode(char *bencode)
{
  long long len = strlen(bencode);
  return be_decode(&bencode, &len);
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
