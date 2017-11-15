#ifndef DICO_FINDER_H
# define DICO_FINDER_H

#include "bencode_parser.h"

char *dico_find_str(struct be_dico **dico, const char *key);
long long dico_find_int(struct be_dico **dico, const char *key);

#endif
