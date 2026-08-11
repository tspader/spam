#include "spn.h"

SPN_EXPORT
s32 stage_headers(spn_t* spn) {
  if (spn_copy(spn, SPN_DIR_MANIFEST, "config/config.h", SPN_DIR_WORK, "private/config.h")) {
    return 1;
  }
  if (spn_copy(spn, SPN_DIR_MANIFEST, "config/version.h", SPN_DIR_WORK, "private/version.h")) {
    return 1;
  }
  if (spn_copy(spn, SPN_DIR_MANIFEST, "config/pcm_symbols_list.c", SPN_DIR_WORK, "private/pcm_symbols_list.c")) {
    return 1;
  }
  if (spn_copy(spn, SPN_DIR_MANIFEST, "config/ctl_symbols_list.c", SPN_DIR_WORK, "private/ctl_symbols_list.c")) {
    return 1;
  }
  if (spn_copy(spn, SPN_DIR_SOURCE, "include/*.h", SPN_DIR_WORK, "public/alsa")) {
    return 1;
  }
  if (spn_copy(spn, SPN_DIR_SOURCE, "include/sound/*.h", SPN_DIR_WORK, "public/alsa/sound")) {
    return 1;
  }
  if (spn_copy(spn, SPN_DIR_SOURCE, "include/sound/uapi/*.h", SPN_DIR_WORK, "public/alsa/sound/uapi")) {
    return 1;
  }
  if (spn_copy(spn, SPN_DIR_MANIFEST, "config/version.h", SPN_DIR_WORK, "public/alsa/version.h")) {
    return 1;
  }
  return spn_copy(spn, SPN_DIR_MANIFEST, "config/asoundlib.h", SPN_DIR_WORK, "public/alsa/asoundlib.h");
}

SPN_EXPORT
s32 package(spn_t* spn) {
  return spn_copy(spn, SPN_DIR_WORK, "public/alsa/*", SPN_DIR_INCLUDE, "alsa");
}

SPN_EXPORT
spn_err_t configure(spn_t* spn, spn_config_t* config) {
  spn_target_t* asound = spn_get_target(spn, "asound");
  spn_target_add_include(asound, spn_get_subdir(spn, SPN_DIR_WORK, "private"));
  spn_target_add_include(asound, spn_get_subdir(spn, SPN_DIR_WORK, "public"));

  spn_target_t* devices = spn_get_target(spn, "devices");
  spn_target_add_include(devices, spn_get_subdir(spn, SPN_DIR_WORK, "public"));

  spn_node_t* headers = spn_add_node(config, "headers");
  spn_node_set_fn(headers, "stage_headers");
  spn_node_add_input(headers, spn_get_subdir(spn, SPN_DIR_MANIFEST, "config/config.h"));
  spn_node_add_input(headers, spn_get_subdir(spn, SPN_DIR_MANIFEST, "config/version.h"));
  spn_node_add_input(headers, spn_get_subdir(spn, SPN_DIR_MANIFEST, "config/asoundlib.h"));
  spn_node_add_input(headers, spn_get_subdir(spn, SPN_DIR_MANIFEST, "config/pcm_symbols_list.c"));
  spn_node_add_input(headers, spn_get_subdir(spn, SPN_DIR_MANIFEST, "config/ctl_symbols_list.c"));
  spn_node_add_output(headers, spn_get_subdir(spn, SPN_DIR_WORK, "private/config.h"));
  spn_node_add_output(headers, spn_get_subdir(spn, SPN_DIR_WORK, "public/alsa/asoundlib.h"));

  return SPN_OK;
}
