#ifndef BENCODE_H
# define BENCODE_H
# include <stdio.h>
# include "buffer.h"

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
    s_buf *s;
    long long i;
    struct be_node **l;
    struct be_dico **d;
  } val;
};

struct be_node *bencode_file_decode(char *path);
s_buf *bencode_encode(struct be_node *node);
struct be_node *bencode_decode(char *bencode, long long size);
void bencode_free_node(struct be_node *node);
int bencode_file_pretty_print();

#endif
