#include <string.h>
#include "bencode.h"

struct be_node *
dico_find(struct be_node *node, const char *key)
{
  if (!node || node->type != BE_DIC)
    return NULL;
  struct be_dico **dico = node->val.d;
  size_t i = 0;
  while (dico[i] && strcmp(dico[i]->key, key))
    ++i;

  return (dico[i] && !strcmp(key, dico[i]->key)) ? dico[i]->val : NULL;
}

char *
dico_find_str(struct be_node *node, const char *key)
{
  struct be_node *n = dico_find(node, key);
  if (!n || n->type != BE_STR)
    return NULL;
  return n ? n->val.s->str : NULL;
}

long long
dico_find_int(struct be_node *node, const char *key)
{
  struct be_node *n = dico_find(node, key);
  if (!n || n->type != BE_INT)
    return 0;
  return n ? n->val.i : 0;
}
