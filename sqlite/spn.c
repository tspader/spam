#include "spn.h"

s32 run_autoconf(spn_t* spn, spn_node_ctx_t* ctx) {
  spn_autoconf_t* ac = spn_autoconf_new(spn);
  spn_autoconf_add_flag(ac, "--disable-tcl");
  return spn_autoconf_run(ac);
}

s32 run_make(spn_t* spn, spn_node_ctx_t* ctx) {
  return spn_make(spn);
}

s32 run_install(spn_t* spn, spn_node_ctx_t* ctx) {
  spn_make_t* make = spn_make_new(spn);
  spn_make_add_target(make, "install");
  return spn_make_run(make);
}

spn_err_t configure(spn_t* spn, spn_config_t* config) {
  spn_node_t* autoconf = spn_add_node(config, "autoconf");
  spn_node_set_fn(autoconf, run_autoconf);

  spn_node_t* make = spn_add_node(config, "make");
  spn_node_set_fn(make, run_make);
  spn_node_link(autoconf, make);

  spn_node_t* install = spn_add_node(config, "install");
  spn_node_set_fn(install, run_install);
  spn_node_link(make, install);
  return SPN_OK;
}
