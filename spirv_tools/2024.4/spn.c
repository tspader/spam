#include "spn.h"

SPN_EXPORT
spn_err_t configure(spn_t* spn, spn_config_t* config) {
  spn_target_t* spirv_tools = spn_get_target(spn, "spirv_tools");
  spn_target_add_include(spirv_tools, spn_get_dir(spn, SPN_DIR_SOURCE));
  const spn_t* spirv_headers = spn_get_dep(spn, "spirv_headers");
  spn_target_add_include(spirv_tools, spn_get_subdir(spirv_headers, SPN_DIR_SOURCE, "include"));
  return SPN_OK;
}
