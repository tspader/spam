#include "spn.h"

SPN_EXPORT
spn_err_t configure(spn_t* spn, spn_config_t* config) {
  spn_target_t* dxc = spn_get_target(spn, "dxcompiler");
  spn_target_add_include(dxc, spn_get_subdir(spn, SPN_DIR_MANIFEST, "config"));
  spn_target_add_include(dxc, spn_get_subdir(spn, SPN_DIR_MANIFEST, "gen/include"));
  spn_target_add_include(dxc, spn_get_subdir(spn, SPN_DIR_MANIFEST, "gen/tools/clang/include"));
  spn_target_add_include(dxc, spn_get_subdir(spn, SPN_DIR_MANIFEST, "gen/lib/HLSL"));
  spn_target_add_include(dxc, spn_get_subdir(spn, SPN_DIR_MANIFEST, "gen/lib/DxilValidation"));
  spn_target_add_include(dxc, spn_get_subdir(spn, SPN_DIR_MANIFEST, "gen/lib/DxilPIXPasses"));
  spn_target_add_include(dxc, spn_get_subdir(spn, SPN_DIR_MANIFEST, "gen/tools/clang/lib/Basic"));
  spn_target_add_include(dxc, spn_get_subdir(spn, SPN_DIR_MANIFEST, "gen/tools/clang/lib/Sema"));
  spn_target_add_include(dxc, spn_get_subdir(spn, SPN_DIR_MANIFEST, "gen/tools/clang/tools/dxcompiler"));
  spn_target_add_include(dxc, spn_get_subdir(spn, SPN_DIR_MANIFEST, "gen/utils/version"));
  spn_target_add_include(dxc, spn_get_subdir(spn, SPN_DIR_SOURCE, "include"));
  spn_target_add_include(dxc, spn_get_subdir(spn, SPN_DIR_SOURCE, "tools/clang/include"));
  spn_target_add_include(dxc, spn_get_subdir(spn, SPN_DIR_SOURCE, "tools/clang/tools/dxcvalidator"));
  spn_target_add_include(dxc, spn_get_subdir(spn, SPN_DIR_SOURCE, "tools/clang/tools/dxcompiler"));

  const spn_t* directx = spn_get_dep(spn, "directx_headers");
  spn_target_add_include(dxc, spn_get_subdir(directx, SPN_DIR_INCLUDE, "directx"));
  spn_target_add_include(dxc, spn_get_subdir(directx, SPN_DIR_INCLUDE, "wsl/stubs"));

  spn_target_t* example = spn_get_target(spn, "compile");
  if (example) {
    spn_target_add_include(example, spn_get_subdir(spn, SPN_DIR_SOURCE, "include"));
  }

  return SPN_OK;
}
