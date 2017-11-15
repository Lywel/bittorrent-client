#include <stdlib.h>
#include <string.h>
#include "bencode.h"
#include "bencode_parser.h"

static void
bencode_go_until(char end, char **bencode, long long *size)
{
  while (**bencode != end)
  {
    --(*size);
    ++(*bencode);
  }
  **bencode = 0;
  ++(*bencode);
  --(*size);
}

char *
bencode_parse_str(char **bencode, long long *size)
{
  char *start = *bencode;
  bencode_go_until(':', bencode, size);
  long long len = atoll(start);

  char *str = malloc((len + 1) * sizeof(char));
  if (str)
  {
    strncpy(str, *bencode, len);
    str[len] = 0;
  }
  *bencode += len;
  *size -= len;

  return str;
}

long long
bencode_parse_int(char **bencode, long long *size)
{
  ++(*bencode);
  --(*size);
  char *start = *bencode;
  bencode_go_until('e', bencode, size);
  return atoll(start);
}

struct be_node **
bencode_parse_lst(char **bencode, long long *size)
{
  long long nb = 0;
  ++(*bencode);
  --(*size);
  struct be_node** list = NULL;
  while (**bencode != 'e')
  {
    nb++;
    list = realloc(list, (nb + 1) * sizeof(struct be_list *));
    list[nb - 1] = be_decode(bencode, size);
  }
  ++(*bencode);
  --(*size);

  if (!nb)
    list = realloc(list, sizeof(struct be_list *));

  list[nb] = NULL;
  return list;
}

struct be_dico **
bencode_parse_dic(char **bencode, long long *size)
{
  long long nb = 0;
  ++(*bencode);
  --(*size);
  struct be_dico **dico = NULL;
  while (**bencode != 'e')
  {
    nb++;
    dico = realloc(dico, (nb + 1) * sizeof(struct be_dico *));
    dico[nb - 1] = malloc(sizeof(struct be_dico));
    dico[nb - 1]->key = bencode_parse_str(bencode, size);
    dico[nb - 1]->val = be_decode(bencode, size);
  }
  ++(*bencode);
  --(*size);

  if (!nb)
    dico = realloc(dico, sizeof(struct be_list *));

  dico[nb] = NULL;
  return dico;
}

