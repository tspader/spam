#include <stdio.h>

#include <spirv/unified1/spirv.h>

int main(void) {
  if (SpvMagicNumber != 0x07230203) {
    return 1;
  }
  printf("SpvMagicNumber = 0x%08x\n", (unsigned)SpvMagicNumber);
  return 0;
}
