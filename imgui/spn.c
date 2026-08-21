#include "spn.h"

SPN_EXPORT
spn_err_t configure(spn_t* spn, spn_config_t* config) {
  spn_target_t* imgui = spn_get_target(spn, "imgui");
  spn_target_add_include(imgui, spn_get_dir(spn, SPN_DIR_SOURCE));
  return SPN_OK;
}
