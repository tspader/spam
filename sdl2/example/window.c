#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"

#define W 640
#define H 480

static void draw(SDL_Renderer* renderer, int frame) {
  double t = (double)frame / 60.0;

  SDL_SetRenderDrawColor(renderer, (Uint8)(40 + 30 * SDL_sin(t * 1.3)),
                         (Uint8)(30 + 25 * SDL_sin(t * 0.9 + 2.0)),
                         (Uint8)(60 + 40 * SDL_sin(t * 1.7 + 4.0)), 0xFF);
  SDL_RenderClear(renderer);

  for (int i = 0; i < 3; ++i) {
    int w = 90 + 30 * i;
    int h = 70;
    SDL_Rect r = {(int)((W - w) * 0.5 * (1.0 - SDL_cos(t * (1.1 + 0.4 * i)))),
                  (int)((H - h) * 0.5 * (1.0 - SDL_sin(t * (0.7 + 0.5 * i)))), w, h};
    SDL_SetRenderDrawColor(renderer, (Uint8)(128 + 127 * SDL_sin(t * 2.0 + i)),
                           (Uint8)(128 + 127 * SDL_sin(t * 2.0 + i + 2.1)),
                           (Uint8)(128 + 127 * SDL_sin(t * 2.0 + i + 4.2)), 0xFF);
    SDL_RenderFillRect(renderer, &r);
  }

  int sweep = (int)((W - 1) * 0.5 * (1.0 - SDL_cos(t * 2.3)));
  SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  SDL_RenderDrawLine(renderer, sweep, 0, sweep, H - 1);
}

int main(int argc, char** argv) {
  int frames = argc > 1 ? atoi(argv[1]) : 180;
  const char* out = argc > 2 ? argv[2] : "sdl2-window.bmp";
  if (frames < 1) {
    frames = 1;
  }

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
    fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Window* window = SDL_CreateWindow("spn sdl2 window", SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED, W, H,
                                        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  if (!window) {
    fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
  }
  if (!renderer) {
    fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  SDL_RendererInfo info;
  SDL_GetRendererInfo(renderer, &info);
  int ww = 0, wh = 0, pw = 0, ph = 0;
  SDL_GetWindowSize(window, &ww, &wh);
  SDL_GetRendererOutputSize(renderer, &pw, &ph);
  printf("video driver: %s\n", SDL_GetCurrentVideoDriver());
  printf("renderer:     %s\n", info.name);
  printf("window:       %dx%d (drawable %dx%d)\n", ww, wh, pw, ph);

  Uint64 start = SDL_GetPerformanceCounter();
  int frame = 0;
  int running = 1;
  while (running && frame < frames) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT ||
          (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
        running = 0;
      }
    }
    draw(renderer, frame);
    SDL_RenderPresent(renderer);
    SDL_Delay(16);
    ++frame;
  }
  double seconds =
      (double)(SDL_GetPerformanceCounter() - start) / (double)SDL_GetPerformanceFrequency();

  draw(renderer, frame);
  Uint32* pixels = (Uint32*)SDL_malloc((size_t)pw * (size_t)ph * sizeof(Uint32));
  int read = pixels && SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, pixels,
                                            pw * 4) == 0;
  SDL_RenderPresent(renderer);

  int ok = 0;
  if (!read) {
    fprintf(stderr, "SDL_RenderReadPixels: %s\n", SDL_GetError());
  } else {
    SDL_Surface* shot = SDL_CreateRGBSurfaceWithFormatFrom(pixels, pw, ph, 32, pw * 4,
                                                           SDL_PIXELFORMAT_ARGB8888);
    if (!shot || SDL_SaveBMP(shot, out) != 0) {
      fprintf(stderr, "SDL_SaveBMP: %s\n", SDL_GetError());
    } else {
      printf("saved:        %s\n", out);
      ok = 1;
    }
    SDL_FreeSurface(shot);

    int lit = 0;
    for (int i = 0; i < 5; ++i) {
      int x = pw * (i + 1) / 6;
      int y = ph * (i + 1) / 6;
      Uint32 argb = pixels[(size_t)y * pw + x] & 0x00FFFFFFu;
      lit += argb != 0;
      printf("  probe (%3d,%3d) rgb=0x%06X %s\n", x, y, argb, argb ? "lit" : "BLACK");
    }
    ok = ok && lit == 5;
  }

  printf("%d frames in %.3f s (%.1f fps)\n", frame, seconds, frame / seconds);
  printf("%s\n", ok ? "PASS" : "FAIL");

  SDL_free(pixels);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return ok ? 0 : 1;
}
