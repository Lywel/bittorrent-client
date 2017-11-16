#include <openssl/sha.h>
#include <stdlib.h>

unsigned char *
compute_sha1(unsigned char *data)
{
  unsigned char *hash = calloc(SHA_DIGEST_LENGTH + 1, sizeof(char));
  SHA1(data, sizeof(data), hash);
  return hash;
}
