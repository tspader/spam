#include <blake3.h>

#include <stdio.h>
#include <string.h>

int main(void) {
  printf("blake3 %s\n", blake3_version());

  const char* message = "hello, blake3";
  blake3_hasher hasher;
  blake3_hasher_init(&hasher);
  blake3_hasher_update(&hasher, message, strlen(message));

  unsigned char digest[BLAKE3_OUT_LEN];
  blake3_hasher_finalize(&hasher, digest, sizeof(digest));

  for (size_t i = 0; i < sizeof(digest); i++) {
    printf("%02x", digest[i]);
  }
  printf("\n");
  return 0;
}
