#include <mbedtls/ctr_drbg.h>
#include <mbedtls/entropy.h>
#include <mbedtls/sha256.h>
#include <mbedtls/version.h>

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
  char version[32];
  mbedtls_version_get_string_full(version);
  printf("%s\n", version);

  const char* message = "hello, mbedtls";
  unsigned char digest[32];
  if (mbedtls_sha256((const unsigned char*)message, strlen(message), digest, 0)) {
    printf("sha256 failed\n");
    return 1;
  }
  print_hex("sha256", digest, sizeof(digest));

  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context drbg;
  mbedtls_entropy_init(&entropy);
  mbedtls_ctr_drbg_init(&drbg);

  int err = mbedtls_ctr_drbg_seed(&drbg, mbedtls_entropy_func, &entropy, NULL, 0);
  if (err) {
    printf("ctr_drbg_seed failed: -0x%04x\n", -err);
    return 1;
  }

  unsigned char random[16];
  err = mbedtls_ctr_drbg_random(&drbg, random, sizeof(random));
  if (err) {
    printf("ctr_drbg_random failed: -0x%04x\n", -err);
    return 1;
  }
  print_hex("random", random, sizeof(random));

  mbedtls_ctr_drbg_free(&drbg);
  mbedtls_entropy_free(&entropy);
  return 0;
}
