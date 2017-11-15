#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <math.h>
#include "bencode.h"
#include "peer_id.h"
#include "dico_finder.h"
#include "request_tracker.h"
#include "debug.h"
#include "hash.h"

/**
 * 117 beeing the length of peer_id + info_hash + port added to all the
 * nescessary keywords related to the GET post
*/
#define C_SIZE 83

static int
get_file_length(struct be_node *info)
{
  int length = dico_find_int(info, "length");
  if (!length)
  {
    struct be_node *files = dico_find(info, "files");
    if (files)
      length = dico_find_int(files, "length");
  }

  return length;
}

static char *
create_request(struct be_node *dico)
{
  char *peer_id = generate_peer_id();
  char *announce = dico_find_str(dico, "announce");

  struct be_node *info = dico_find(dico, "info");
  unsigned char *pieces = (unsigned char *)dico_find_str(info, "pieces");
  unsigned char *info_hash = get_sha1(pieces);

  char *port = "6882";
  char *bytes_left = "100"; //PLACE HOLDER, SHOULD START WITH FILE SIZE
  char *bytes_dwl = "00"; //bytes_dwl_get();
  char *bytes_upl = "00"; //bytes_upl_get();
  size_t req_len = strlen(peer_id) + strlen(announce)
                   + strlen(port) + strlen(bytes_left)
                   + strlen(bytes_dwl) + strlen(bytes_upl) + C_SIZE;

  char *request = calloc(req_len, sizeof(char));

  sprintf(request, "%s?peer_id=%s&info_hash=%s&port=%s&left=%s&downloaded=%s&"
                   "uploaded=%s&compact=1",
                   announce, peer_id, info_hash, port, bytes_left, bytes_dwl,
                   bytes_upl);
  return request;
}

static size_t
write_callback(char *ptr, size_t size, size_t nmemb, char **userdata)
{
  *userdata = calloc(nmemb + 1, size);
  memcpy(*userdata, ptr, size * nmemb);
  return size * nmemb;
}

struct be_node *
get_peer_list(struct be_node *dico)
{
  CURL *curl;
  printf("puts");
  debug("Creating request\n");
  char *req = create_request(dico);
  char *answer = NULL;

  curl = curl_easy_init();
  if (!curl)
    return NULL;

  char *request = curl_easy_escape(curl, req, strlen(req));
  if (!request)
    return NULL;
  free(req);

  curl_easy_setopt(curl, CURLOPT_URL, request);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &answer);

  curl_easy_perform(curl);
  curl_free(request);

  long long ans_len = strlen(answer);
  struct be_node *peer_list = bencode_decode(answer, ans_len);
  free(answer);

  return peer_list;
}
