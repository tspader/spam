#include "spn.h"

spn_err_t package(spn_t* spn) {
  spn_copy(spn, SPN_DIR_SOURCE, "utest.h", SPN_DIR_INCLUDE, "");
  return SPN_OK;
}
