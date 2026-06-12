#include "spn.h"

spn_err_t package(spn_t* spn) {
  spn_copy(spn, SPN_DIR_SOURCE, "sp.h", SPN_DIR_INCLUDE, "");
  spn_copy(spn, SPN_DIR_SOURCE, "sp/*", SPN_DIR_INCLUDE, "sp");
  return SPN_OK;
}
