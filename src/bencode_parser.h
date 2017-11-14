#ifndef BENCODE_PARSER_H
# define BENCODE_PARSER_H

enum be_type
{
  BE_STR,
  BE_INT,
  BE_LST,
  BE_DIC
};

struct be_dico
{
  char *key;
  struct be_node *val;
};

struct be_node
{
  enum be_type type;
  union
  {
    char *s;
    long long i;
    struct be_node **l;
    struct be_dico *d;
  } val;
};

int bencode_file_pretty_print(FILE *cout, char *path);

#endif
