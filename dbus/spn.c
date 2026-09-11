#include "spn.h"

SPN_EXPORT
s32 stage_headers(spn_t* spn) {
  return spn_copy(spn, SPN_DIR_MANIFEST, "config/dbus-arch-deps.h", SPN_DIR_WORK, "public/dbus/dbus-arch-deps.h");
}

SPN_EXPORT
s32 publish_headers(spn_t* spn) {
  return spn_copy(spn, SPN_DIR_MANIFEST, "config/dbus-arch-deps.h", SPN_DIR_INCLUDE, "dbus/dbus-arch-deps.h");
}

SPN_EXPORT
spn_err_t configure(spn_t* spn, spn_config_t* config) {
  spn_target_t* version = spn_get_target(spn, "version");
  spn_target_add_include(version, spn_get_dir(spn, SPN_DIR_SOURCE));
  spn_target_add_include(version, spn_get_subdir(spn, SPN_DIR_WORK, "public"));

  spn_node_t* headers = spn_add_node(config, "headers");
  spn_node_set_fn(headers, "stage_headers");
  spn_node_add_input(headers, spn_get_subdir(spn, SPN_DIR_MANIFEST, "config/dbus-arch-deps.h"));
  spn_node_add_output(headers, spn_get_subdir(spn, SPN_DIR_WORK, "public/dbus/dbus-arch-deps.h"));

  spn_node_t* publish = spn_add_node(config, "publish");
  spn_node_set_fn(publish, "publish_headers");
  spn_node_add_input(publish, spn_get_subdir(spn, SPN_DIR_MANIFEST, "config/dbus-arch-deps.h"));
  spn_node_add_output(publish, spn_get_subdir(spn, SPN_DIR_INCLUDE, "dbus/dbus-arch-deps.h"));

  return SPN_OK;
}
