#include <stdio.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_LCD_FILTER_H

int main(int argc, char** argv) {
  const char* path = argc > 1 ? argv[1] : "example/assets/JetBrainsMono-Regular.ttf";

  FT_Library library;
  if (FT_Init_FreeType(&library)) {
    fprintf(stderr, "FT_Init_FreeType failed\n");
    return 1;
  }

  FT_Error error = FT_Library_SetLcdFilter(library, FT_LCD_FILTER_DEFAULT);
  if (error) {
    fprintf(stderr, "FT_Library_SetLcdFilter: %s\n", FT_Error_String(error));
    return 1;
  }

  FT_Face face;
  error = FT_New_Face(library, path, 0, &face);
  if (error) {
    fprintf(stderr, "%s: %s\n", path, FT_Error_String(error));
    return 1;
  }
  FT_Set_Pixel_Sizes(face, 0, 16);

  FT_Load_Char(face, 'g', FT_LOAD_RENDER);
  printf("gray: %ux%u\n", face->glyph->bitmap.width, face->glyph->bitmap.rows);

  FT_Load_Char(face, 'g', FT_LOAD_RENDER | FT_LOAD_TARGET_LCD);
  FT_Bitmap* bitmap = &face->glyph->bitmap;
  printf("lcd:  %ux%u (%u pixels, 3 subpixels each)\n", bitmap->width, bitmap->rows, bitmap->width / 3);

  for (unsigned int y = 0; y < bitmap->rows; y++) {
    for (unsigned int x = 0; x < bitmap->width / 3; x++) {
      unsigned char* px = &bitmap->buffer[y * bitmap->pitch + x * 3];
      unsigned level = (px[0] + px[1] + px[2]) / 3;
      putchar(" .:*#"[level / 52]);
    }
    putchar('\n');
  }

  FT_Done_Face(face);
  FT_Done_FreeType(library);
  return 0;
}
