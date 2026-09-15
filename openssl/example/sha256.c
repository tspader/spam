#include <openssl/crypto.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include <stdio.h>
#include <string.h>

static void print_hex(const char* label, const unsigned char* buf, size_t len) {
  printf("%s: ", label);
  for (size_t i = 0; i < len; i++) {
    printf("%02x", buf[i]);
  }
  printf("\n");
}

int main(void) {
  printf("%s\n", OpenSSL_version(OPENSSL_VERSION));

  const char* message = "hello, openssl";
  unsigned char digest[EVP_MAX_MD_SIZE];
  unsigned int digest_len = 0;
  if (!EVP_Digest(message, strlen(message), digest, &digest_len, EVP_sha256(), NULL)) {
    printf("EVP_Digest failed\n");
    return 1;
  }
  print_hex("sha256", digest, digest_len);

  unsigned char random[16];
  if (RAND_bytes(random, sizeof(random)) != 1) {
    printf("RAND_bytes failed\n");
    return 1;
  }
  print_hex("random", random, sizeof(random));
  return 0;
}
