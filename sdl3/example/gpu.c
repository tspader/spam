#include <stdio.h>
#include <stdlib.h>

#include <SDL3/SDL.h>

#define W 640
#define H 480
#define TEX_W 64
#define TEX_H 64

static const SDL_FColor CLEAR = {0.2f, 0.4f, 0.8f, 1.0f};

static int check_pixels(const Uint8* pixels) {
  Uint8 want[4] = {(Uint8)(CLEAR.r * 255.0f + 0.5f), (Uint8)(CLEAR.g * 255.0f + 0.5f),
                   (Uint8)(CLEAR.b * 255.0f + 0.5f), (Uint8)(CLEAR.a * 255.0f + 0.5f)};
  for (int i = 0; i < TEX_W * TEX_H; ++i) {
    for (int c = 0; c < 4; ++c) {
      int d = (int)pixels[i * 4 + c] - (int)want[c];
      if (d < -2 || d > 2) {
        fprintf(stderr, "pixel %d channel %d: got %d want %d\n", i, c, pixels[i * 4 + c],
                want[c]);
        return 0;
      }
    }
  }
  return 1;
}

int main(int argc, char** argv) {
  int frames = argc > 1 ? atoi(argv[1]) : 180;
  if (frames < 1) {
    frames = 1;
  }

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
    return 1;
  }

  SDL_GPUDevice* device = SDL_CreateGPUDevice(
      SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL, true,
      NULL);
  if (!device) {
    fprintf(stderr, "SDL_CreateGPUDevice: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }
  printf("gpu driver:   %s\n", SDL_GetGPUDeviceDriver(device));

  SDL_Window* window = SDL_CreateWindow("spn sdl3 gpu", W, H, SDL_WINDOW_RESIZABLE);
  if (!window) {
    fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
    SDL_DestroyGPUDevice(device);
    SDL_Quit();
    return 1;
  }
  if (!SDL_ClaimWindowForGPUDevice(device, window)) {
    fprintf(stderr, "SDL_ClaimWindowForGPUDevice: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_DestroyGPUDevice(device);
    SDL_Quit();
    return 1;
  }
  printf("video driver: %s\n", SDL_GetCurrentVideoDriver());

  Uint64 start = SDL_GetPerformanceCounter();
  int frame = 0;
  int running = 1;
  int presented = 0;
  while (running && frame < frames) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT ||
          (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)) {
        running = 0;
      }
    }

    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(device);
    if (!cmd) {
      fprintf(stderr, "SDL_AcquireGPUCommandBuffer: %s\n", SDL_GetError());
      break;
    }
    SDL_GPUTexture* swapchain = NULL;
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmd, window, &swapchain, NULL, NULL)) {
      fprintf(stderr, "SDL_WaitAndAcquireGPUSwapchainTexture: %s\n", SDL_GetError());
      SDL_SubmitGPUCommandBuffer(cmd);
      break;
    }
    if (swapchain) {
      double t = (double)frame / 60.0;
      SDL_GPUColorTargetInfo target = {0};
      target.texture = swapchain;
      target.clear_color.r = (float)(0.5 + 0.5 * SDL_sin(t * 1.3));
      target.clear_color.g = (float)(0.5 + 0.5 * SDL_sin(t * 0.9 + 2.0));
      target.clear_color.b = (float)(0.5 + 0.5 * SDL_sin(t * 1.7 + 4.0));
      target.clear_color.a = 1.0f;
      target.load_op = SDL_GPU_LOADOP_CLEAR;
      target.store_op = SDL_GPU_STOREOP_STORE;
      SDL_GPURenderPass* pass = SDL_BeginGPURenderPass(cmd, &target, 1, NULL);
      SDL_EndGPURenderPass(pass);
      ++presented;
    }
    SDL_SubmitGPUCommandBuffer(cmd);
    ++frame;
  }
  double seconds =
      (double)(SDL_GetPerformanceCounter() - start) / (double)SDL_GetPerformanceFrequency();

  int ok = 0;
  SDL_GPUTextureCreateInfo texture_info = {0};
  texture_info.type = SDL_GPU_TEXTURETYPE_2D;
  texture_info.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
  texture_info.usage = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET;
  texture_info.width = TEX_W;
  texture_info.height = TEX_H;
  texture_info.layer_count_or_depth = 1;
  texture_info.num_levels = 1;
  SDL_GPUTexture* texture = SDL_CreateGPUTexture(device, &texture_info);

  SDL_GPUTransferBufferCreateInfo transfer_info = {0};
  transfer_info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_DOWNLOAD;
  transfer_info.size = TEX_W * TEX_H * 4;
  SDL_GPUTransferBuffer* transfer = SDL_CreateGPUTransferBuffer(device, &transfer_info);

  if (!texture || !transfer) {
    fprintf(stderr, "gpu resource: %s\n", SDL_GetError());
  } else {
    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(device);
    SDL_GPUColorTargetInfo target = {0};
    target.texture = texture;
    target.clear_color = CLEAR;
    target.load_op = SDL_GPU_LOADOP_CLEAR;
    target.store_op = SDL_GPU_STOREOP_STORE;
    SDL_GPURenderPass* pass = SDL_BeginGPURenderPass(cmd, &target, 1, NULL);
    SDL_EndGPURenderPass(pass);

    SDL_GPUCopyPass* copy = SDL_BeginGPUCopyPass(cmd);
    SDL_GPUTextureRegion region = {0};
    region.texture = texture;
    region.w = TEX_W;
    region.h = TEX_H;
    region.d = 1;
    SDL_GPUTextureTransferInfo dst = {0};
    dst.transfer_buffer = transfer;
    SDL_DownloadFromGPUTexture(copy, &region, &dst);
    SDL_EndGPUCopyPass(copy);

    SDL_GPUFence* fence = SDL_SubmitGPUCommandBufferAndAcquireFence(cmd);
    if (!fence || !SDL_WaitForGPUFences(device, true, &fence, 1)) {
      fprintf(stderr, "gpu submit: %s\n", SDL_GetError());
    } else {
      Uint8* pixels = SDL_MapGPUTransferBuffer(device, transfer, false);
      if (!pixels) {
        fprintf(stderr, "SDL_MapGPUTransferBuffer: %s\n", SDL_GetError());
      } else {
        ok = check_pixels(pixels);
        printf("readback:     %dx%d clear (%.1f, %.1f, %.1f) %s\n", TEX_W, TEX_H, CLEAR.r,
               CLEAR.g, CLEAR.b, ok ? "matches" : "MISMATCH");
        SDL_UnmapGPUTransferBuffer(device, transfer);
      }
    }
    if (fence) {
      SDL_ReleaseGPUFence(device, fence);
    }
  }

  ok = ok && presented == frame && frame > 0;
  printf("%d frames (%d presented) in %.3f s (%.1f fps)\n", frame, presented, seconds,
         frame / seconds);
  printf("%s\n", ok ? "PASS" : "FAIL");

  if (transfer) {
    SDL_ReleaseGPUTransferBuffer(device, transfer);
  }
  if (texture) {
    SDL_ReleaseGPUTexture(device, texture);
  }
  SDL_ReleaseWindowFromGPUDevice(device, window);
  SDL_DestroyWindow(window);
  SDL_DestroyGPUDevice(device);
  SDL_Quit();
  return ok ? 0 : 1;
}
