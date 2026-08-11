#include <stdio.h>

#include <ft2build.h>
#include FT_FREETYPE_H

int main(int argc, char** argv) {
  const char* path = argc > 1 ? argv[1] : "example/assets/JetBrainsMono-Regular.ttf";
  const char* text = argc > 2 ? argv[2] : "fi";

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
  FT_Set_Pixel_Sizes(face, 0, 16);

  printf("%s %s, autohinted with the harfbuzz shaper\n", face->family_name, face->style_name);
  for (const char* c = text; *c; c++) {
    error = FT_Load_Char(face, (FT_ULong)*c, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT);
    if (error) {
      fprintf(stderr, "FT_Load_Char('%c'): %s\n", *c, FT_Error_String(error));
      return 1;
    }

    FT_Bitmap* bitmap = &face->glyph->bitmap;
    printf("'%c' %ux%u\n", *c, bitmap->width, bitmap->rows);
    for (unsigned int y = 0; y < bitmap->rows; y++) {
      for (unsigned int x = 0; x < bitmap->width; x++) {
        putchar(" .:*#"[bitmap->buffer[y * bitmap->pitch + x] / 52]);
      }
      putchar('\n');
    }
  }

  FT_Done_Face(face);
  FT_Done_FreeType(library);
  return 0;
}
