#include <qrcodegen.h>

#include <stdio.h>

int main(void) {
  uint8_t qrcode[qrcodegen_BUFFER_LEN_MAX];
  uint8_t temp[qrcodegen_BUFFER_LEN_MAX];
  bool ok = qrcodegen_encodeText("https://spn.dev", temp, qrcode,
      qrcodegen_Ecc_MEDIUM, qrcodegen_VERSION_MIN, qrcodegen_VERSION_MAX,
      qrcodegen_Mask_AUTO, true);
  if (!ok) {
    return 1;
  }

  int size = qrcodegen_getSize(qrcode);
  for (int y = 0; y < size; y++) {
    for (int x = 0; x < size; x++) {
      fputs(qrcodegen_getModule(qrcode, x, y) ? "##" : "  ", stdout);
    }
    fputc('\n', stdout);
  }
  return 0;
}
