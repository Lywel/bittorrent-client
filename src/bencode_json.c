#include <stdio.h>
#include "bencode_parser.h"
#include "bencode_json.h"

static void
list_dump_json(FILE* cout, struct be_node **list)
{
  for (long long i = 0; list[i]; ++i)
  {
    bencode_dump_json(cout, list[i]);
    if (list[i + 1])
      fprintf(cout, ",");
  }
}

static void
dico_dump_json(FILE* cout, struct be_dico **dico)
{
  for (long long i = 0; dico[i]; ++i)
  {
    fprintf(cout, "\"%s\":", dico[i]->key);
    bencode_dump_json(cout, dico[i]->val);
    if (dico[i + 1])
      fprintf(cout, ",");
  }
}

void
bencode_dump_json(FILE* cout, struct be_node *node)
{
  if (!node)
    return;
  switch (node->type)
  {
  case BE_STR:
    fprintf(cout, "\"");
    for (long long i = 0; node->val.s[i]; ++i)
    {
      unsigned char c = node->val.s[i];
      fprintf(cout, (c < 0x20 || c > 0x7E || c == '"') ? "\\u00%02x":"%c", c);
    }
    fprintf(cout, "\"");
    break;
  case BE_INT:
    fprintf(cout, "%lld", node->val.i);
    break;
  case BE_LST:
    fprintf(cout, "[");
    list_dump_json(cout, node->val.l);
    fprintf(cout, "]");
    break;
  case BE_DIC:
    fprintf(cout, "{");
    dico_dump_json(cout, node->val.d);
    fprintf(cout, "}");
    break;
  }
}
