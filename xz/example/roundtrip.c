#include <lzma.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>

int main(void) {
  const char* line = "the quick brown fox jumps over the lazy dog\n";
  size_t line_len = strlen(line);
  uint8_t text [4096];
  size_t len = 0;
  while (len + line_len <= sizeof(text)) {
    memcpy(text + len, line, line_len);
    len += line_len;
  }

  uint8_t compressed [4096];
  size_t compressed_len = 0;
  lzma_ret ret = lzma_easy_buffer_encode(LZMA_PRESET_DEFAULT, LZMA_CHECK_CRC64, NULL, text, len, compressed, &compressed_len, sizeof(compressed));
  if (ret != LZMA_OK) {
    fprintf(stderr, "encode failed: %d\n", (int)ret);
    return 1;
  }

  uint8_t restored [4096];
  size_t restored_len = 0;
  size_t consumed = 0;
  uint64_t memlimit = UINT64_MAX;
  ret = lzma_stream_buffer_decode(&memlimit, 0, NULL, compressed, &consumed, compressed_len, restored, &restored_len, sizeof(restored));
  if (ret != LZMA_OK) {
    fprintf(stderr, "decode failed: %d\n", (int)ret);
    return 1;
  }
  if (restored_len != len || memcmp(text, restored, len) != 0) {
    fprintf(stderr, "roundtrip mismatch\n");
    return 1;
  }

  printf("liblzma %s\n", lzma_version_string());
  printf("%zu bytes -> %zu bytes -> %zu bytes\n", len, compressed_len, restored_len);
  return 0;
}
