#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <stdio.h>

#define TEXT "spn"

int main(int argc, char** argv) {
  const char* path = argc > 1 ? argv[1] : "example/assets/Roboto-Regular.ttf";

  if (!TTF_Init()) {
    fprintf(stderr, "TTF_Init failed: %s\n", SDL_GetError());
    return 1;
  }

  int version = TTF_Version();
  printf("SDL3_ttf %d.%d.%d\n", version / 1000000, (version / 1000) % 1000, version % 1000);

  TTF_Font* font = TTF_OpenFont(path, 32.0f);
  if (!font) {
    fprintf(stderr, "TTF_OpenFont(%s) failed: %s\n", path, SDL_GetError());
    TTF_Quit();
    return 1;
  }

  printf("family=%s style=%s height=%d ascent=%d\n",
    TTF_GetFontFamilyName(font),
    TTF_GetFontStyleName(font),
    TTF_GetFontHeight(font),
    TTF_GetFontAscent(font));

  int w = 0;
  int h = 0;
  if (!TTF_GetStringSize(font, TEXT, 0, &w, &h)) {
    fprintf(stderr, "TTF_GetStringSize failed: %s\n", SDL_GetError());
    TTF_CloseFont(font);
    TTF_Quit();
    return 1;
  }
  printf("measured '%s' as %dx%d\n", TEXT, w, h);

  SDL_Color color = { 255, 255, 255, 255 };
  SDL_Surface* rendered = TTF_RenderText_Blended(font, TEXT, 0, color);
  if (!rendered) {
    fprintf(stderr, "TTF_RenderText_Blended failed: %s\n", SDL_GetError());
    TTF_CloseFont(font);
    TTF_Quit();
    return 1;
  }
  printf("rendered surface %dx%d format=%s\n", rendered->w, rendered->h, SDL_GetPixelFormatName(rendered->format));

  SDL_Surface* argb = SDL_ConvertSurface(rendered, SDL_PIXELFORMAT_ARGB8888);
  if (!argb) {
    fprintf(stderr, "SDL_ConvertSurface failed: %s\n", SDL_GetError());
    SDL_DestroySurface(rendered);
    TTF_CloseFont(font);
    TTF_Quit();
    return 1;
  }

  int ink = 0;
  const Uint32* pixels = (const Uint32*)argb->pixels;
  int stride = argb->pitch / 4;
  for (int y = 0; y < argb->h; y++) {
    for (int x = 0; x < argb->w; x++) {
      unsigned alpha = (pixels[y * stride + x] >> 24) & 0xff;
      if (alpha) {
        ink++;
      }
      if (!(y % 2)) {
        putchar(" .:*#"[alpha / 52]);
      }
    }
    if (!(y % 2)) {
      putchar('\n');
    }
  }
  printf("ink pixels: %d\n", ink);

  SDL_DestroySurface(argb);
  SDL_DestroySurface(rendered);
  TTF_CloseFont(font);
  TTF_Quit();

  if (!w || !h || !ink) {
    fprintf(stderr, "empty render\n");
    return 1;
  }
  return 0;
}
