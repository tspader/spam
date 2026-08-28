#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "load.embed.h"

#include <stdio.h>

static const char* SAMPLE = "spn-sdl3-image-sample.png";

static int fail(const char* what) {
  printf("sdl3_image: %s failed: %s\n", what, SDL_GetError());
  return 1;
}

int main(void) {
  int version = IMG_Version();
  printf("sdl3_image: version %d.%d.%d\n",
         SDL_VERSIONNUM_MAJOR(version),
         SDL_VERSIONNUM_MINOR(version),
         SDL_VERSIONNUM_MICRO(version));

  if (!SDL_Init(0)) {
    return fail("SDL_Init");
  }

  size_t size = (size_t)sdl3_image_sample_png_size;
  if (!SDL_SaveFile(SAMPLE, sdl3_image_sample_png, size)) {
    SDL_Quit();
    return fail("SDL_SaveFile");
  }

  SDL_Surface* from_file = IMG_Load(SAMPLE);
  if (!from_file) {
    SDL_RemovePath(SAMPLE);
    SDL_Quit();
    return fail("IMG_Load");
  }

  printf("sdl3_image: IMG_Load(%s) -> %dx%d %s\n",
         SAMPLE,
         from_file->w,
         from_file->h,
         SDL_GetPixelFormatName(from_file->format));

  SDL_IOStream* io = SDL_IOFromConstMem(sdl3_image_sample_png, size);
  if (!io) {
    SDL_DestroySurface(from_file);
    SDL_RemovePath(SAMPLE);
    SDL_Quit();
    return fail("SDL_IOFromConstMem");
  }

  SDL_Surface* from_memory = IMG_Load_IO(io, true);
  if (!from_memory) {
    SDL_DestroySurface(from_file);
    SDL_RemovePath(SAMPLE);
    SDL_Quit();
    return fail("IMG_Load_IO");
  }

  printf("sdl3_image: IMG_Load_IO(memory) -> %dx%d %s\n",
         from_memory->w,
         from_memory->h,
         SDL_GetPixelFormatName(from_memory->format));

  int ok = from_file->w == from_memory->w && from_file->h == from_memory->h;

  SDL_DestroySurface(from_memory);
  SDL_DestroySurface(from_file);
  SDL_RemovePath(SAMPLE);
  SDL_Quit();

  if (!ok) {
    printf("sdl3_image: file and memory surfaces disagree\n");
    return 1;
  }

  printf("sdl3_image: ok\n");
  return 0;
}
