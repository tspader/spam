#include "spn.h"

SPN_EXPORT
s32 stage_headers(spn_t* spn) {
  return spn_copy(spn, SPN_DIR_MANIFEST, "gen/luajit.h", SPN_DIR_INCLUDE, "luajit.h");
}

SPN_EXPORT
spn_err_t configure(spn_t* spn, spn_config_t* config) {
  spn_node_t* headers = spn_add_node(config, "headers");
  spn_node_set_fn(headers, "stage_headers");
  spn_node_add_input(headers, spn_get_subdir(spn, SPN_DIR_MANIFEST, "gen/luajit.h"));
  spn_node_add_output(headers, spn_get_subdir(spn, SPN_DIR_INCLUDE, "luajit.h"));
  return SPN_OK;
}
