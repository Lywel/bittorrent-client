#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <math.h>
#include "buffer.h"
#include "bencode.h"
#include "peer_id.h"
#include "dico_finder.h"
#include "request_tracker.h"
#include "debug.h"
#include "hash.h"
#include "client.h"
#include "socket_init.h"

/*static long long
get_file_length(struct be_node *info)
{
  long long length = dico_find_int(info, "length");
  if (!length)
  {
    struct be_node *files = dico_find(info, "files");
    if (files)
      length = dico_find_int(files, "length");
  }

  return length;
}*/

static char *
build_tracker_uri(struct be_node *dico, CURL *curl)
{
  char *urn = dico_find_str(dico, "announce");
  char *peer_id = generate_peer_id();

  struct be_node *info_node = dico_find(dico, "info");
  s_buf *info = bencode_encode(info_node);
  debug("info bencode: '%s'", info->str);

  char *info_hash = compute_sha1(info);
  char *e_info_hash = curl_easy_escape(curl, info_hash, 20);

  char *port = calloc(5, 1);
  sprintf(port, "%d", get_port());
  debug("listening on port %s", port);

  char *format = "%s?peer_id=%s&info_hash=%s&port=%s&left=0&downloaded=0&"
                "uploaded=0&compact=1";

  long long len = strlen(format) + strlen(urn) + strlen(peer_id)
                  + strlen(e_info_hash) + strlen(port) - 8;

  char *uri = calloc(len, sizeof(char));
  if (!uri)
    return NULL;

  sprintf(uri, format,
          urn, peer_id, e_info_hash, port);

  free(port);
  free(peer_id);
  buffer_free(info);
  free(info_hash);
  curl_free(e_info_hash);
  // TODO: free other resources when byte_* will be dynamic
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
  if (!curl || init_socket() < 0)
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
get_peer_list(struct be_node *dico)
{
  s_buf *data = NULL;
  CURL *curl = build_curl_request(dico, &data);
  if (!curl)
    return NULL;

  debug("performming curl request");
  CURLcode res = curl_easy_perform(curl);
  debug("curl request is resolved with code %d", res);
  struct be_node *peer_list = NULL;

  if (res == CURLE_OK)
  {
    debug("raw answer form server: '%s'", data->str);
    peer_list = bencode_decode(data->str, data->len);
    buffer_free(data);
  }

  curl_easy_cleanup(curl);
  return peer_list;
}
