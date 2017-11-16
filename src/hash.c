#include <openssl/sha.h>
#include <stdlib.h>
#include "buffer.h"

unsigned char *
compute_sha1(s_buf *data)
{
  unsigned char *hash = calloc(SHA_DIGEST_LENGTH + 1, sizeof(char));
  SHA1((unsigned char *)data->str, data->len, hash);
  return hash;
}
