#include <stdlib.h>
#include <string.h>
#include <stddef.h>

char *
peerid_generate()
{
  char *peer_id = calloc(21, 1);
  strcpy(peer_id, "MB2020");
  for (size_t i = 6; i < 20; ++i)
    peer_id[i] = 'A' + (random() % 26);

  return peer_id;
}
