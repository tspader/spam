#include <zlib.h>

#include <stdio.h>
#include <string.h>

int main(void) {
  const char* text = "the quick brown fox jumps over the lazy dog, again and again and again";
  uLong len = (uLong)strlen(text) + 1;
  Bytef compressed [256];
  uLongf compressed_len = sizeof(compressed);
  char restored [256];
  uLongf restored_len = sizeof(restored);

  if (compress(compressed, &compressed_len, (const Bytef*)text, len) != Z_OK) {
    fprintf(stderr, "compress failed\n");
    return 1;
  }
  if (uncompress((Bytef*)restored, &restored_len, compressed, compressed_len) != Z_OK) {
    fprintf(stderr, "uncompress failed\n");
    return 1;
  }
  if (restored_len != len || strcmp(text, restored) != 0) {
    fprintf(stderr, "roundtrip mismatch\n");
    return 1;
  }

  printf("zlib %s\n", zlibVersion());
  printf("%lu bytes -> %lu bytes -> %lu bytes\n", len, compressed_len, restored_len);
  return 0;
}
