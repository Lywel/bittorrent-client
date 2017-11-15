#include <string.h>
#include "bencode.h"
#include "bencode_parser.h"
#include "dico_finder.h"

char *
dico_find_str(struct be_node *node, const char *key)
{
  if (node->type != BE_DIC)
    return NULL;
  struct be_dico **dico = node->val.d;
  size_t i = 0;
  while (dico[i] && dico[i]->val->type == BE_STR && strcmp(dico[i]->key, key))
    ++i;
  return dico[i]->val->val.s;
}

long long
dico_find_int(struct be_node *node, const char *key)
{
  if (node->type != BE_DIC)
    return NULL;
  struct be_dico **dico = node->val.d;
  size_t i = 0;
  while (dico[i] && dico[i]->val->type == BE_INT && strcmp(dico[i]->key, key))
    ++i;
  return dico[i]->val->val.i;
}
