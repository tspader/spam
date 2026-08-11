#include <stdio.h>
#include <stdlib.h>

#include <SDL3/SDL.h>

#define W 640
#define H 480

static void draw(SDL_Renderer* renderer, int frame) {
  double t = (double)frame / 60.0;

  SDL_SetRenderDrawColor(renderer, (Uint8)(40 + 30 * SDL_sin(t * 1.3)),
                         (Uint8)(30 + 25 * SDL_sin(t * 0.9 + 2.0)),
                         (Uint8)(60 + 40 * SDL_sin(t * 1.7 + 4.0)), 0xFF);
  SDL_RenderClear(renderer);

  for (int i = 0; i < 3; ++i) {
    float w = 90.0f + 30.0f * i;
    float h = 70.0f;
    SDL_FRect r = {(float)((W - w) * 0.5 * (1.0 - SDL_cos(t * (1.1 + 0.4 * i)))),
                   (float)((H - h) * 0.5 * (1.0 - SDL_sin(t * (0.7 + 0.5 * i)))), w, h};
    SDL_SetRenderDrawColor(renderer, (Uint8)(128 + 127 * SDL_sin(t * 2.0 + i)),
                           (Uint8)(128 + 127 * SDL_sin(t * 2.0 + i + 2.1)),
                           (Uint8)(128 + 127 * SDL_sin(t * 2.0 + i + 4.2)), 0xFF);
    SDL_RenderFillRect(renderer, &r);
  }

  float sweep = (float)((W - 1) * 0.5 * (1.0 - SDL_cos(t * 2.3)));
  SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  SDL_RenderLine(renderer, sweep, 0.0f, sweep, (float)(H - 1));
}

int main(int argc, char** argv) {
  int frames = argc > 1 ? atoi(argv[1]) : 180;
  const char* out = argc > 2 ? argv[2] : "sdl3-window.bmp";
  if (frames < 1) {
    frames = 1;
  }

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Window* window = SDL_CreateWindow("spn sdl3 window", W, H, SDL_WINDOW_RESIZABLE);
  if (!window) {
    fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
  if (!renderer) {
    renderer = SDL_CreateRenderer(window, SDL_SOFTWARE_RENDERER);
  }
  if (!renderer) {
    fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  int ww = 0, wh = 0, pw = 0, ph = 0;
  SDL_GetWindowSize(window, &ww, &wh);
  SDL_GetCurrentRenderOutputSize(renderer, &pw, &ph);
  printf("video driver: %s\n", SDL_GetCurrentVideoDriver());
  printf("renderer:     %s\n", SDL_GetRendererName(renderer));
  printf("window:       %dx%d (drawable %dx%d)\n", ww, wh, pw, ph);

  Uint64 start = SDL_GetPerformanceCounter();
  int frame = 0;
  int running = 1;
  while (running && frame < frames) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT ||
          (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)) {
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
  SDL_Surface* shot = SDL_RenderReadPixels(renderer, NULL);
  SDL_RenderPresent(renderer);

  int ok = 0;
  if (!shot) {
    fprintf(stderr, "SDL_RenderReadPixels: %s\n", SDL_GetError());
  } else {
    if (!SDL_SaveBMP(shot, out)) {
      fprintf(stderr, "SDL_SaveBMP: %s\n", SDL_GetError());
    } else {
      printf("saved:        %s\n", out);
      ok = 1;
    }

    int lit = 0;
    for (int i = 0; i < 5; ++i) {
      int x = shot->w * (i + 1) / 6;
      int y = shot->h * (i + 1) / 6;
      Uint8 r = 0, g = 0, b = 0, a = 0;
      SDL_ReadSurfacePixel(shot, x, y, &r, &g, &b, &a);
      Uint32 rgb = ((Uint32)r << 16) | ((Uint32)g << 8) | b;
      lit += rgb != 0;
      printf("  probe (%3d,%3d) rgb=0x%06X %s\n", x, y, rgb, rgb ? "lit" : "BLACK");
    }
    ok = ok && lit == 5;
    SDL_DestroySurface(shot);
  }

  printf("%d frames in %.3f s (%.1f fps)\n", frame, seconds, frame / seconds);
  printf("%s\n", ok ? "PASS" : "FAIL");

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return ok ? 0 : 1;
}
