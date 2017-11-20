#include <stdio.h>
#include <ctype.h>
#include "bencode.h"
#include "bencode_json.h"

static void
list_dump_json(struct be_node **list)
{
  for (long long i = 0; list[i]; ++i)
  {
    bencode_dump_json(list[i]);
    if (list[i + 1])
      putchar(',');
  }
}

static void
dico_dump_json(struct be_dico **dico)
{
  for (long long i = 0; dico[i]; ++i)
  {
    printf("\"%s\":", dico[i]->key);
    bencode_dump_json(dico[i]->val);
    if (dico[i + 1])
      putchar(',');
  }
}

void
bencode_dump_json(struct be_node *node)
{
  if (!node)
    return;
  switch (node->type)
  {
  case BE_STR:
    putchar('"');
    for (long long i = 0; i < node->val.s->len; ++i)
    {
      unsigned char c = node->val.s->str[i];
      printf((!isprint(c) || c == '"' || c == '\\') ? "\\u00%02x":"%c", c);
    }
    putchar('"');
    break;
  case BE_INT:
    printf("%lld", node->val.i);
    break;
  case BE_LST:
    printf("[");
    list_dump_json(node->val.l);
    printf("]");
    break;
  case BE_DIC:
    printf("{");
    dico_dump_json(node->val.d);
    printf("}");
    break;
  }
}
