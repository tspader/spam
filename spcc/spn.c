#include "spn.h"

s32 run_autoconf(spn_t* spn, spn_node_ctx_t* ctx) {
  spn_profile_t* profile = spn_get_profile(spn);

  spn_autoconf_t* ac = spn_autoconf_new(spn);
  if (spn_profile_get_libc(profile) == SPN_LIBC_MUSL) {
    spn_autoconf_add_flag(ac, "--config-musl");
  }

  return spn_autoconf_run(ac);
}

s32 run_make(spn_t* spn, spn_node_ctx_t* ctx) {
  return spn_make(spn);
}

spn_err_t configure(spn_t* spn, spn_config_t* config) {
  spn_node_t* autoconf = spn_add_node(config, "autoconf");
  spn_node_set_fn(autoconf, run_autoconf);

  spn_node_t* make = spn_add_node(config, "make");
  spn_node_set_fn(make, run_make);
  spn_node_link(autoconf, make);
  return SPN_OK;
}

spn_err_t package(spn_t* spn) {
  spn_make_t* make = spn_make_new(spn);
  spn_make_add_target(make, "install");
  spn_make_run(make);
  return SPN_OK;
}
