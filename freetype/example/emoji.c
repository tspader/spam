#include <stdio.h>

#include <ft2build.h>
#include FT_FREETYPE_H

int main(int argc, char** argv) {
  const char* path = argc > 1 ? argv[1] : "example/assets/emoji.ttf";
  unsigned long codepoint = 0x1F600;

  FT_Library library;
  if (FT_Init_FreeType(&library)) {
    fprintf(stderr, "FT_Init_FreeType failed\n");
    return 1;
  }

  FT_Face face;
  FT_Error error = FT_New_Face(library, path, 0, &face);
  if (error) {
    fprintf(stderr, "%s: %s\n", path, FT_Error_String(error));
    return 1;
  }

  FT_Select_Size(face, 0);
  FT_UInt index = FT_Get_Char_Index(face, codepoint);
  error = FT_Load_Glyph(face, index, FT_LOAD_COLOR | FT_LOAD_RENDER);
  if (error) {
    fprintf(stderr, "FT_Load_Glyph: %s\n", FT_Error_String(error));
    return 1;
  }

  FT_Bitmap* bitmap = &face->glyph->bitmap;
  printf("%s: U+%04lX is glyph %u, %ux%u, pixel mode %s\n", face->family_name, codepoint, index,
         bitmap->width, bitmap->rows, bitmap->pixel_mode == FT_PIXEL_MODE_BGRA ? "BGRA" : "not BGRA");
  if (bitmap->pixel_mode != FT_PIXEL_MODE_BGRA) {
    return 1;
  }

  for (unsigned int y = 0; y < bitmap->rows; y += 2) {
    for (unsigned int x = 0; x < bitmap->width; x++) {
      unsigned char* px = &bitmap->buffer[y * bitmap->pitch + x * 4];
      unsigned char alpha = px[3];
      unsigned luma = (px[0] + px[1] + px[2]) / 3;
      putchar(alpha < 128 ? ' ' : luma < 96 ? '@' : '#');
    }
    putchar('\n');
  }

  FT_Done_Face(face);
  FT_Done_FreeType(library);
  return 0;
}
