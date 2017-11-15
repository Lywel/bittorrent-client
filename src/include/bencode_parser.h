#ifndef BENCODE_PARSER_H
# define BENCODE_PARSER_H
# include <stdio.h>

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
    struct be_dico **d;
  } val;
};

struct be_node *bencode_decode(char **bencode, long long *size);
int bencode_file_pretty_print(char *path);

#endif
