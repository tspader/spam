#include <stdio.h>

#include "SDL.h"

#define SIDE 64
#define BMP_CAP (SIDE * SIDE * 4 + 4096)

static Uint32 at(SDL_Surface* surface, int x, int y) {
  const Uint8* row = (const Uint8*)surface->pixels + (size_t)y * surface->pitch;
  return ((const Uint32*)row)[x] & 0x00FFFFFFu;
}

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;

  SDL_SetHint(SDL_HINT_VIDEODRIVER, "dummy");
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Surface* canvas = SDL_CreateRGBSurfaceWithFormat(0, SIDE, SIDE, 32, SDL_PIXELFORMAT_ARGB8888);
  SDL_Surface* sprite = SDL_CreateRGBSurfaceWithFormat(0, 16, 16, 32, SDL_PIXELFORMAT_ARGB8888);
  if (!canvas || !sprite) {
    fprintf(stderr, "SDL_CreateRGBSurfaceWithFormat: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  printf("canvas format: %s\n", SDL_GetPixelFormatName(canvas->format->format));

  SDL_FillRect(canvas, NULL, SDL_MapRGB(canvas->format, 0x20, 0x20, 0x20));
  SDL_Rect band = {0, 0, SIDE, 16};
  SDL_FillRect(canvas, &band, SDL_MapRGB(canvas->format, 0x00, 0x00, 0xFF));
  SDL_FillRect(sprite, NULL, SDL_MapRGB(sprite->format, 0xFF, 0xC0, 0x00));

  SDL_Rect dst = {32, 32, 16, 16};
  if (SDL_BlitSurface(sprite, NULL, canvas, &dst) != 0) {
    fprintf(stderr, "SDL_BlitSurface: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  Uint8* bmp = (Uint8*)SDL_malloc(BMP_CAP);
  SDL_RWops* out = SDL_RWFromMem(bmp, BMP_CAP);
  if (!bmp || !out || SDL_SaveBMP_RW(canvas, out, 1) != 0) {
    fprintf(stderr, "SDL_SaveBMP_RW: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  SDL_RWops* in = SDL_RWFromMem(bmp, BMP_CAP);
  SDL_Surface* loaded = SDL_LoadBMP_RW(in, 1);
  if (!loaded) {
    fprintf(stderr, "SDL_LoadBMP_RW: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  SDL_Surface* back = SDL_ConvertSurfaceFormat(loaded, SDL_PIXELFORMAT_ARGB8888, 0);
  if (!back || SDL_LockSurface(back) != 0 || SDL_LockSurface(canvas) != 0) {
    fprintf(stderr, "SDL_ConvertSurfaceFormat/Lock: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  printf("bmp roundtrip: %dx%d %s -> %s\n", loaded->w, loaded->h,
         SDL_GetPixelFormatName(loaded->format->format),
         SDL_GetPixelFormatName(back->format->format));

  struct {
    const char* what;
    int x, y;
    Uint32 want;
  } probes[] = {
      {"band blue", 8, 4, 0x0000FF},
      {"background", 8, 40, 0x202020},
      {"sprite", 40, 40, 0xFFC000},
      {"below sprite", 40, 56, 0x202020},
  };

  int ok = 1;
  for (size_t i = 0; i < SDL_arraysize(probes); ++i) {
    Uint32 drawn = at(canvas, probes[i].x, probes[i].y);
    Uint32 reread = at(back, probes[i].x, probes[i].y);
    int good = drawn == probes[i].want && reread == probes[i].want;
    ok &= good;
    printf("  %-13s (%2d,%2d) drawn=0x%06X bmp=0x%06X want=0x%06X %s\n", probes[i].what,
           probes[i].x, probes[i].y, drawn, reread, probes[i].want, good ? "ok" : "MISMATCH");
  }

  printf("%s\n", ok ? "PASS" : "FAIL");

  SDL_UnlockSurface(canvas);
  SDL_UnlockSurface(back);
  SDL_FreeSurface(back);
  SDL_FreeSurface(loaded);
  SDL_FreeSurface(sprite);
  SDL_FreeSurface(canvas);
  SDL_free(bmp);
  SDL_Quit();
  return ok ? 0 : 1;
}
