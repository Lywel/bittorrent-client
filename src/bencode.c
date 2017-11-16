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

static s_buf *
bencode_encode_dic(struct be_node *dico)
{
  long long len = 2;
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
    s_buf *val = bencode_encode(dico->val.d[i]->val);
    len += val->len;
    res = realloc(res, len);
    memcpy(res + cur, val->str, val->len);
    buffer_free(val);
    cur = len - 2;
  }
  res[cur++] = 'e';
  return buffer_init(res, cur);
}

static s_buf *
bencode_encode_lst(struct be_node *list)
{
  long long len = 2;
  long long cur = 0;
  char *res = calloc(len, sizeof(char));
  res[cur++] = 'l';

  for (long long i = 0; list->val.l[i]; ++i)
  {
    s_buf *val = bencode_encode(list->val.l[i]);
    len += val->len;
    res = realloc(res, len);
    memcpy(res + cur, val->str, val->len);
    buffer_free(val);
    cur = len - 2;
  }
  res[cur++] = 'e';
  return buffer_init(res, cur);
}

s_buf *
bencode_encode(struct be_node *node)
{
  long long len = 0;
  char *tmp = NULL;
  switch (node->type)
  {
  case BE_STR:
    len = node->val.s->len;
    tmp = calloc(len + log10(len) + 2, sizeof(char));
    sprintf(tmp, "%lld:", len);
    memcpy(tmp + strlen(tmp), node->val.s->str, len);
    return buffer_init(tmp, len + log10(len) + 2);
  case BE_INT:
    len = node->val.i;
    len = len ? log10(len < 0 ? -len : len) + (len < 0) : 1;
    tmp = calloc(len + 3, sizeof(char));
    sprintf(tmp, "i%llde", node->val.i);
    return buffer_init(tmp, len + 3);
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
