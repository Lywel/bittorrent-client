#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "buffer.h"
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
  struct be_node *n = bencode_decode(bencode, size);
  free(bencode);
  return n;
}

static char *
bencode_encode_dic(struct be_node *dico)
{
  long long len = 3;
  long long cur = 0;
  char *res = calloc(len, sizeof(char));
  res[cur++] = 'd';

  for (long long i = 0; dico->val.d[i]; ++i)
  {
    long long klen = strlen(dico->val.d[i]->key);
    len += klen + 2 + log10(klen);
    res = realloc(res, len);
    sprintf(&res[cur], "%lld:", klen);
    cur += log10(klen) + 2;
    memcpy(res + cur, dico->val.d[i]->key, klen);
    cur = len - 2;
    char *val = bencode_encode(dico->val.d[i]->val);
    len += strlen(val);
    res = realloc(res, len);
    memcpy(res + cur, val, strlen(val));
    free(val);
    cur = len - 2;
  }
  res[cur++] = 'e';
  res[cur] = 0;
  return res;
}

static char *
bencode_encode_lst(struct be_node *list)
{
  long long len = 3;
  long long cur = 0;
  char *res = calloc(len, sizeof(char));
  res[cur++] = 'l';

  for (long long i = 0; list->val.l[i]; ++i)
  {
    char *val = bencode_encode(list->val.l[i]);
    len += strlen(val);
    res = realloc(res, len);
    memcpy(res + cur, val, strlen(val));
    free(val);
    cur = len - 2;
  }
  res[cur++] = 'e';
  res[cur] = 0;
  return res;
}

char *
bencode_encode(struct be_node *node)
{
  long long len = 0;
  char *bencode = NULL;
  switch (node->type)
  {
  case BE_STR:
    len = node->val.s->len;
    bencode = calloc(len + log10(len) + 3, sizeof(char));
    sprintf(bencode, "%lld:", len);
    memcpy(bencode + strlen(bencode), node->val.s, len);
    return bencode;
  case BE_INT:
    len = node->val.i;
    len = len ? log10(len < 0 ? -len : len) + (len < 0) : 1;
    bencode = calloc(len + 4, sizeof(char));
    sprintf(bencode, "i%llde", node->val.i);
    return bencode;
  case BE_DIC:
     return bencode_encode_dic(node);
  default:
     return bencode_encode_lst(node);
  }
}

struct be_node *
bencode_decode(char *bencode, long long len)
{
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
