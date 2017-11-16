#include <openssl/evp.h>
#include <stdlib.h>

void
compute_sha1(const unsigned char *message, size_t message_len,
             unsigned char **digest)
{
  EVP_MD_CTX *mdctx;
  unsigned int len = 20;

  mdctx = EVP_MD_CTX_create();
  EVP_DigestInit_ex(mdctx, EVP_sha1(), NULL);
  *digest = (unsigned char *)OPENSSL_malloc(EVP_MD_size(EVP_sha1()));
  EVP_DigestFinal_ex(mdctx, *digest, &len);

  EVP_MD_CTX_destroy(mdctx);
}
