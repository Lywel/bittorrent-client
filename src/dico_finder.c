#include <string.h>
#include "bencode_parser.h"
#include "dico_finder.h"

char *
dico_find_str(struct be_dico **dico, const char *key)
{
  size_t i = 0;
  while (dico[i] && dico[i]->val->type == BE_STR && strcmp(dico[i]->key, key))
    ++i;
  return dico[i]->val->val.s;
}

long long
dico_find_int(struct be_dico **dico, const char *key)
{
  size_t i = 0;
  while (dico[i] && dico[i]->val->type == BE_INT && strcmp(dico[i]->key, key))
    ++i;
  return dico[i]->val->val.i;
}
