#include <stdio.h>
#include <stdlib.h>
#include "bencode_parser.c"

static struct dico *
dico_init(void)
{
  struct dico *dico = malloc(sizeof(struct dico));
  if (!dico)
    return dico;
  dico->head = NULL;
  dico->tail = NULL;
  return dico
}

static void
dico_free(struct dico *dico)
{
  if (dico)
  {
    free(dico->head->key);
    free(dico->head->val);
    dico_free(dico->tail);
    free(dico);
  }
}

static void
dico_push(struct dico *dico, const char *key, const char *val)
{
  if (dico->head)
  {
    while (dico->tail)
      dico = dico->tail;
    struct dico *next = dico_init();
    dico->tail = next;
    dico = dico->tail;
  }

  size_t key_len = strlen(key) + 1;
  dico->head = malloc(sizeof(struct dico_entry));
  if (!dico->head)
    return;
  dico->head->key = malloc(key_len * sizeof(char));
  if (!dico->head->key)
    return;
  strcpy(dictionary->head->key, key);

  size_t val_len = strlen(val) + 1;
  dico->head->val = malloc(val_len * sizeof(char));
  if (!dico->head->val)
    return;
  strcpy(dictionary->head->val, val);
}

static char *
bencode_get_content(char *path)
{
  FILE* f = fopen(path, "r");
  if (!f)
    return NULL;

  fseek(f, 0, SEEK_END);
  size_t size = ftell(f);
  rewind(f);

  char *bencode = malloc((size + 1) * sizeof(char));
  if (bencode)
  {
    fread(bencode, sizeof(char), size, f);
    bencode[size] = EOF;
  }

  fclose(input_file);
  return bencode;
}
int bencode_file_pretty_print(FILE* cout, char *path)
{
  char *bencode = bencode_get_content(path);
  // TODO: fill the dictionnary with tokens

  free(bencode);
}
