#include <openssl/sha.h>
#include <stdlib.h>

char *
get_sha1(const char *data)
{
  unsigned char *hash = malloc(SHA_DIGEST_LENGTH);
  SHA1(data, sizeof(data), hash);
  return hash;
}
