#include <stdlib.h>
#include <string.h>
#include <stddef.h>

char *
generate_peer_id(void)
{
  char *peer_id = calloc(21, sizeof(char));
  strcpy(peer_id, "MB2020");
  for (size_t i = 6; i < 20; ++i)
    peer_id[i] = 'A' + (rand() % 26);

  return peer_id;
}
