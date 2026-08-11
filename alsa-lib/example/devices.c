#include <alsa/asoundlib.h>

#include <stdio.h>

int main(void) {
  printf("alsa-lib %s\n", snd_asoundlib_version());

  int card = -1;
  while (snd_card_next(&card) == 0 && card >= 0) {
    char* name = NULL;
    char* longname = NULL;
    snd_card_get_name(card, &name);
    snd_card_get_longname(card, &longname);
    printf("card %d: %s (%s)\n", card, name ? name : "?", longname ? longname : "?");
    free(name);
    free(longname);
  }

  void** hints = NULL;
  if (snd_device_name_hint(-1, "pcm", &hints) == 0) {
    for (void** hint = hints; *hint; hint++) {
      char* name = snd_device_name_get_hint(*hint, "NAME");
      if (name) {
        printf("pcm: %s\n", name);
        free(name);
      }
    }
    snd_device_name_free_hint(hints);
  }

  snd_pcm_t* pcm = NULL;
  int err = snd_pcm_open(&pcm, "null", SND_PCM_STREAM_PLAYBACK, 0);
  if (err < 0) {
    printf("null pcm: %s\n", snd_strerror(err));
    return 1;
  }
  err = snd_pcm_set_params(pcm, SND_PCM_FORMAT_S16_LE, SND_PCM_ACCESS_RW_INTERLEAVED, 2, 48000, 1, 100000);
  if (err < 0) {
    printf("set_params: %s\n", snd_strerror(err));
    return 1;
  }
  short frames[128] = { 0 };
  snd_pcm_sframes_t n = snd_pcm_writei(pcm, frames, 64);
  printf("wrote %ld frames to null pcm\n", (long)n);
  snd_pcm_close(pcm);
  snd_config_update_free_global();
  return 0;
}
