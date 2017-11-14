#include <string.h>
#include <stdio.h>
#include <peer_id.h>

/**
 * 117 beeing the length of peer_id + info_hash + port added to all the
 * nescessary keywords related to the GET post
*/ 
#define C_SIZE 117

static char *
request_create(struct be_node *dico)
{
  char *peer_id = peer_id_generate();
  char *announce = dico_get(dico, "announce");
  char *info_hash = dico_get(dico, "info_hash");
  char *port = dico_get(dico, "port");
  char *bytes_left = bytes_left_get();
  char *bytes_dwl = bytes_dwl_get();
  char *bytes_upl = bytes_upl_get();

  char *request = malloc(C_SIZE + strlen(bytes_left) + strlen(bytes_dwl)
                                + strlen(bytes_upl) + strlen(announce));

  sprintf(request, "%s?peer_id=%s&info_hash=%s&port=%s&left=%s&downloaded=%s&"
                   "uploaded=%s&compact=1",
                   announce, peer_id, info_hash, port, bytes_left, bytes_dwl,
                   bytes_upl);

  return request;
}
