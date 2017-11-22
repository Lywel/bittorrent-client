#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include "bencode.h"
#include "dico_finder.h"
#include "debug.h"
#include "decode_binary_peers.h"
#include "math.h"

static char *
build_tracker_uri(struct be_node *dico, CURL *curl)
{
  long long left = 0;
  char *urn = dico_find_str(dico, "announce");
  char *e_info_hash = curl_easy_escape(curl, g_bt.info_hash, 20);
  struct be_node *length = dico_find(dico_find(g_bt.torrent, "info"), "length");

  if (!length)
  {
    struct be_node **l  = dico_find(dico_find(g_bt.torrent, "info"),
                                    "files")->val.l;
    for (int i = 0; l[i]; ++i)
    {
      left += dico_find_int(l[i], "length");
    }
  }
  else
    left = length->val.i;

  debug("listening on port %u", g_bt.port);
  char *format = "%s?peer_id=%s&info_hash=%s&port=%u&left=%d&downloaded=0&"
                 "uploaded=0&compact=1";

  long long len = strlen(format) + strlen(urn) + strlen(e_info_hash)
                  + logl(left) + 24;
  (void)len;
  char *uri = calloc(len, sizeof(char));
  if (!uri)
    return NULL;

  sprintf(uri, format, urn, g_bt.peer_id, e_info_hash, g_bt.port, left);


  curl_free(e_info_hash);
  return uri;
}

static size_t
write_callback(char *ptr, size_t size, size_t nmemb, s_buf **userdata)
{
  char *data = calloc(nmemb + 1, size);
  memcpy(data, ptr, size * nmemb);
  *userdata = buffer_init(data, size * nmemb);
  return size * nmemb;
}

static CURL *
build_curl_request(struct be_node *dico, s_buf **data)
{
  debug("curl initialization");
  CURL *curl = curl_easy_init();
  if (!curl)
    return NULL;

  char *uri = build_tracker_uri(dico, curl);
  if (!uri)
    return NULL;

  debug("curl prepared uri: '%s'", uri);
  curl_easy_setopt(curl, CURLOPT_URL, uri);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, data);

  free(uri);

  return curl;
}

struct be_node *
get_peer_list_from_tracker(struct be_node *dico)
{
  s_buf *data = NULL;
  struct be_node *peer_list = NULL;

  CURL *curl = build_curl_request(dico, &data);
  if (!curl)
    return NULL;

  debug("performming curl request");
  verbose("%x%x%x: tracker: requesting peers to %s\n", (uint8_t)g_bt.info_hash[0],
    (uint8_t)g_bt.info_hash[1], (uint8_t)g_bt.info_hash[2],
    dico_find_str(dico, "announce"));

  CURLcode res = curl_easy_perform(curl);
  debug("curl request is resolved with code %d", res);

  if (res == CURLE_OK && data)
  {
    peer_list = bencode_decode(data->str, data->len);
    buffer_free(data);
    struct be_node *peers = dico_find(peer_list, "peers");
    g_bt.peer_list_timeout = dico_find_int(peer_list, "interval");

    decode_binary_peers(peers);
    peer_list_init(peers);
  }
  else
    fprintf(stderr, "curl failed: %s\n", curl_easy_strerror(res));
  curl_easy_cleanup(curl);

  return peer_list;
}
