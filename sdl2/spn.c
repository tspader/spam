#include "spn.h"

SPN_EXPORT
s32 stage_headers(spn_t* spn) {
  if (spn_copy(spn, SPN_DIR_SOURCE, "include/*.h", SPN_DIR_WORK, "SDL2")) {
    return 1;
  }
  return spn_copy(spn, SPN_DIR_MANIFEST, "config/*", SPN_DIR_WORK, "SDL2");
}

SPN_EXPORT
s32 package(spn_t* spn) {
  return spn_copy(spn, SPN_DIR_WORK, "SDL2/*", SPN_DIR_INCLUDE, "");
}

SPN_EXPORT
spn_err_t configure(spn_t* spn, spn_config_t* config) {
  const c8* staged = spn_get_subdir(spn, SPN_DIR_WORK, "SDL2");

  spn_target_t* sdl = spn_get_target(spn, "SDL2");
  spn_target_add_include(sdl, staged);
  spn_target_add_include(sdl, spn_get_subdir(spn, SPN_DIR_SOURCE, "src/video/khronos"));

  spn_target_t* sdl_main = spn_get_target(spn, "SDL2main");
  spn_target_add_include(sdl_main, staged);

  spn_target_t* window = spn_get_target(spn, "window");
  spn_target_add_include(window, staged);

  spn_target_t* surface = spn_get_target(spn, "surface");
  spn_target_add_include(surface, staged);

  spn_node_t* headers = spn_add_node(config, "headers");
  spn_node_set_fn(headers, "stage_headers");
  spn_node_add_input(headers, spn_get_subdir(spn, SPN_DIR_MANIFEST, "config/SDL_config.h"));
  spn_node_add_input(headers, spn_get_subdir(spn, SPN_DIR_MANIFEST, "config/SDL_config_spn.h"));
  spn_node_add_input(headers, spn_get_subdir(spn, SPN_DIR_MANIFEST, "config/SDL_revision.h"));
  spn_node_add_output(headers, spn_get_subdir(spn, SPN_DIR_WORK, "SDL2/SDL_config.h"));

  return SPN_OK;
}
