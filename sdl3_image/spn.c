#include "spn.h"

SPN_EXPORT
spn_err_t configure(spn_t* spn, spn_config_t* config) {
  spn_target_t* load = spn_get_target(spn, "load");
  if (load) {
    spn_target_embed_file_ex(
      load,
      spn_get_subdir(spn, SPN_DIR_MANIFEST, "example/sample.png"),
      "sdl3_image_sample_png",
      "unsigned char",
      "unsigned long long"
    );
  }

  return SPN_OK;
}
