#include "SDL.h"
#include "SDL_mixer.h"

#include <stdio.h>
#include <string.h>

#define TONE_RATE 22050
#define TONE_FRAMES (TONE_RATE / 2)
#define WAV_BYTES (44 + TONE_FRAMES * 2)

static Uint8 wav[WAV_BYTES];
static SDL_atomic_t mixed_samples;
static SDL_atomic_t peak;

static void put16(Uint8* p, Uint32 v) {
  p[0] = (Uint8)v;
  p[1] = (Uint8)(v >> 8);
}

static void put32(Uint8* p, Uint32 v) {
  put16(p, v);
  put16(p + 2, v >> 16);
}

static void build_wav(void) {
  Uint32 i;
  memcpy(wav, "RIFF", 4);
  put32(wav + 4, WAV_BYTES - 8);
  memcpy(wav + 8, "WAVEfmt ", 8);
  put32(wav + 16, 16);
  put16(wav + 20, 1);
  put16(wav + 22, 1);
  put32(wav + 24, TONE_RATE);
  put32(wav + 28, TONE_RATE * 2);
  put16(wav + 32, 2);
  put16(wav + 34, 16);
  memcpy(wav + 36, "data", 4);
  put32(wav + 40, TONE_FRAMES * 2);
  for (i = 0; i < TONE_FRAMES; ++i) {
    double t = (double)i / (double)TONE_RATE;
    Sint16 s = (Sint16)(12000.0 * SDL_sin(6.283185307179586 * 440.0 * t));
    put16(wav + 44 + i * 2, (Uint16)s);
  }
}

static void SDLCALL postmix(void* udata, Uint8* stream, int len) {
  const Sint16* s = (const Sint16*)stream;
  int n = len / (int)sizeof(Sint16);
  int i;
  (void)udata;
  for (i = 0; i < n; ++i) {
    int a = s[i] < 0 ? -(int)s[i] : (int)s[i];
    if (a > SDL_AtomicGet(&peak)) {
      SDL_AtomicSet(&peak, a);
    }
  }
  SDL_AtomicAdd(&mixed_samples, n);
}

int main(int argc, char** argv) {
  int freq = 0, channels = 0, channel, silent;
  Uint16 format = 0;
  Mix_Chunk* chunk;
  Uint32 start, elapsed;

  (void)argc;
  (void)argv;

  SDL_setenv("SDL_AUDIODRIVER", "dummy", 1);
  SDL_setenv("SDL_VIDEODRIVER", "dummy", 1);

  if (SDL_Init(SDL_INIT_AUDIO) != 0) {
    printf("SDL_Init failed: %s\n", SDL_GetError());
    return 1;
  }
  if (Mix_OpenAudio(TONE_RATE, AUDIO_S16SYS, 2, 512) != 0) {
    printf("Mix_OpenAudio failed: %s\n", Mix_GetError());
    SDL_Quit();
    return 1;
  }
  Mix_QuerySpec(&freq, &format, &channels);
  printf("audio driver: %s\n", SDL_GetCurrentAudioDriver());
  printf("device spec:  %d Hz, format 0x%04x, %d channels\n", freq, (unsigned)format, channels);

  build_wav();
  chunk = Mix_LoadWAV_RW(SDL_RWFromConstMem(wav, (int)sizeof(wav)), 1);
  if (!chunk) {
    printf("Mix_LoadWAV_RW failed: %s\n", Mix_GetError());
    Mix_CloseAudio();
    SDL_Quit();
    return 1;
  }
  printf("wav source:   %d bytes in memory (mono s16 %d Hz, %d frames)\n", (int)sizeof(wav), TONE_RATE,
         TONE_FRAMES);
  printf("decoded:      %u bytes = %u frames at device spec\n", (unsigned)chunk->alen,
         (unsigned)(chunk->alen / (Uint32)(channels * 2)));

  Mix_SetPostMix(postmix, NULL);
  channel = Mix_PlayChannel(-1, chunk, 0);
  if (channel < 0) {
    printf("Mix_PlayChannel failed: %s\n", Mix_GetError());
    Mix_FreeChunk(chunk);
    Mix_CloseAudio();
    SDL_Quit();
    return 1;
  }

  start = SDL_GetTicks();
  while (Mix_Playing(channel) && SDL_GetTicks() - start < 5000) {
    SDL_Delay(5);
  }
  elapsed = SDL_GetTicks() - start;
  silent = SDL_AtomicGet(&peak) == 0;

  printf("played:       channel %d for %u ms, %s\n", channel, (unsigned)elapsed,
         Mix_Playing(channel) ? "TIMED OUT" : "ran to completion");
  printf("post-mix:     %d samples observed, peak amplitude %d\n", SDL_AtomicGet(&mixed_samples),
         SDL_AtomicGet(&peak));

  Mix_SetPostMix(NULL, NULL);
  Mix_FreeChunk(chunk);
  Mix_CloseAudio();
  Mix_Quit();
  SDL_Quit();

  if (silent) {
    printf("FAIL: mixer produced only silence\n");
    return 1;
  }
  printf("ok\n");
  return 0;
}
