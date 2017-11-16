#include <openssl/sha.h>
#include <stdlib.h>
#include "buffer.h"

char *
compute_sha1(s_buf *info)
{
  unsigned char *hash = calloc(20, sizeof(char));
  SHA1((unsigned char *)info->str, info->len, hash);
  return (char *)hash;
}

/*
void
compute_sha1(s_buf *info, unsigned char **digest)
{
  EVP_MD_CTX *mdctx;
  unsigned int len = 20;

  mdctx = EVP_MD_CTX_create();
  EVP_DigestInit_ex(mdctx, EVP_sha1(), NULL);
  *digest = (unsigned char *)OPENSSL_malloc(EVP_MD_size(EVP_sha1()));
  EVP_DigestUpdate(mdctx, info->str, info->len);
  EVP_DigestFinal_ex(mdctx, *digest, &len);

  EVP_MD_CTX_destroy(mdctx);
}
*/
