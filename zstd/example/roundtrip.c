#include <zstd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
  const char* line = "the quick brown fox jumps over the lazy dog\n";
  size_t line_len = strlen(line);
  size_t len = line_len * 64;
  size_t bound = ZSTD_compressBound(len);
  char* text = malloc(len);
  char* compressed = malloc(bound);
  char* restored = malloc(len);
  if (!text || !compressed || !restored) {
    fprintf(stderr, "out of memory\n");
    return 1;
  }
  for (size_t i = 0; i < 64; i++) {
    memcpy(text + i * line_len, line, line_len);
  }

  size_t csize = ZSTD_compress(compressed, bound, text, len, ZSTD_defaultCLevel());
  if (ZSTD_isError(csize)) {
    fprintf(stderr, "compress failed: %s\n", ZSTD_getErrorName(csize));
    return 1;
  }

  size_t dsize = ZSTD_decompress(restored, len, compressed, csize);
  if (ZSTD_isError(dsize)) {
    fprintf(stderr, "decompress failed: %s\n", ZSTD_getErrorName(dsize));
    return 1;
  }
  if (dsize != len || memcmp(text, restored, len) != 0) {
    fprintf(stderr, "roundtrip mismatch\n");
    return 1;
  }

  printf("zstd %s\n", ZSTD_versionString());
  printf("%zu bytes -> %zu bytes -> %zu bytes\n", len, csize, dsize);

  free(restored);
  free(compressed);
  free(text);
  return 0;
}
