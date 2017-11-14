#include "bencode_parser.h"
#include "dico_get.h"
#include <string.h>

char *
dico_get_char(struct be_dico **dico, const char *key)
{
  size_t i = 0;
  while (dico[i]->val->type == BE_STR && strcmp(dico[i]->key, key))
    ++i;
  return dico[i]->val->val.s;
}

long long
dico_get_long(struct be_dico **dico, const char *key)
{
  size_t i = 0;
  while (dico[i]->val->type == BE_INT && strcmp(dico[i]->key, key))
    ++i;
  return dico[i]->val->val.i;
}
