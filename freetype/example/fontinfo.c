#include <stdio.h>

#include <ft2build.h>
#include FT_FREETYPE_H

int render(FT_Library library, const char* path, const char* text) {
  FT_Face face;
  FT_Error error = FT_New_Face(library, path, 0, &face);
  if (error) {
    fprintf(stderr, "%s: FT_New_Face failed: %s\n", path, FT_Error_String(error));
    return 1;
  }

  printf("%s: %s %s, %ld glyphs\n", path, face->family_name, face->style_name, face->num_glyphs);

  FT_Set_Pixel_Sizes(face, 0, 16);
  for (const char* c = text; *c; c++) {
    error = FT_Load_Char(face, (FT_ULong)*c, FT_LOAD_RENDER);
    if (error) {
      fprintf(stderr, "%s: FT_Load_Char('%c') failed: %s\n", path, *c, FT_Error_String(error));
      FT_Done_Face(face);
      return 1;
    }

    FT_Bitmap* bitmap = &face->glyph->bitmap;
    printf("'%c' %ux%u\n", *c, bitmap->width, bitmap->rows);
    for (unsigned int y = 0; y < bitmap->rows; y++) {
      for (unsigned int x = 0; x < bitmap->width; x++) {
        unsigned char level = bitmap->buffer[y * bitmap->pitch + x];
        putchar(" .:*#"[level / 52]);
      }
      putchar('\n');
    }
  }

  FT_Done_Face(face);
  return 0;
}

int main(int argc, char** argv) {
  FT_Library library;
  FT_Error error = FT_Init_FreeType(&library);
  if (error) {
    fprintf(stderr, "FT_Init_FreeType failed (%d)\n", error);
    return 1;
  }

  FT_Int major, minor, patch;
  FT_Library_Version(library, &major, &minor, &patch);
  printf("freetype %d.%d.%d\n", major, minor, patch);

  int result = 0;
  if (argc > 1) {
    result = render(library, argv[1], argc > 2 ? argv[2] : "spn");
  }

  FT_Done_FreeType(library);
  return result;
}
