#include <stdio.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_FONT_FORMATS_H

static int show(FT_Library library, const char* path) {
  FT_Face face;
  FT_Error error = FT_New_Face(library, path, 0, &face);
  if (error) {
    fprintf(stderr, "%s: %s\n", path, FT_Error_String(error));
    return 1;
  }
  printf("%-40s %s, %s %s, %ld glyphs\n", path, FT_Get_Font_Format(face), face->family_name, face->style_name, face->num_glyphs);
  FT_Done_Face(face);
  return 0;
}

int main(int argc, char** argv) {
  FT_Library library;
  if (FT_Init_FreeType(&library)) {
    fprintf(stderr, "FT_Init_FreeType failed\n");
    return 1;
  }

  int result = 0;
  if (argc > 1) {
    for (int i = 1; i < argc; i++) {
      result |= show(library, argv[i]);
    }
  }
  else {
    result |= show(library, "example/assets/JetBrainsMono-Regular.ttf");
    result |= show(library, "example/assets/JetBrainsMono-Regular.woff");
    result |= show(library, "example/assets/JetBrainsMono-Regular.woff2");
  }

  FT_Done_FreeType(library);
  return result;
}
