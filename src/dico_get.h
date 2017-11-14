#ifndef DICO_GET_H
# define DICO_GET_H

#include "bencode_parser.h"

char *dico_get_char(struct be_dico **dico, const char *key);
long long dico_get_long(struct be_dico **dico, const char *key);

#endif
