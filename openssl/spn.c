#include "spn.h"

static const c8* generated [] = {
  "asn1.h",
  "asn1err.h",
  "asn1t.h",
  "asyncerr.h",
  "bio.h",
  "bioerr.h",
  "bnerr.h",
  "buffererr.h",
  "cmp.h",
  "cmperr.h",
  "cms.h",
  "cmserr.h",
  "comp.h",
  "comperr.h",
  "conf.h",
  "conferr.h",
  "core_names.h",
  "crmf.h",
  "crmferr.h",
  "crypto.h",
  "cryptoerr.h",
  "ct.h",
  "cterr.h",
  "decodererr.h",
  "dherr.h",
  "dsaerr.h",
  "ecerr.h",
  "encodererr.h",
  "err.h",
  "ess.h",
  "esserr.h",
  "evperr.h",
  "fipskey.h",
  "httperr.h",
  "lhash.h",
  "obj_mac.h",
  "objectserr.h",
  "ocsp.h",
  "ocsperr.h",
  "opensslv.h",
  "pemerr.h",
  "pkcs12.h",
  "pkcs12err.h",
  "pkcs7.h",
  "pkcs7err.h",
  "proverr.h",
  "randerr.h",
  "rsaerr.h",
  "safestack.h",
  "srp.h",
  "ssl.h",
  "sslerr.h",
  "storeerr.h",
  "tserr.h",
  "ui.h",
  "uierr.h",
  "x509.h",
  "x509_acert.h",
  "x509_vfy.h",
  "x509err.h",
  "x509v3.h",
  "x509v3err.h",
};

SPN_EXPORT
s32 stage_headers(spn_t* spn) {
  if (spn_copy(spn, SPN_DIR_MANIFEST, "gen/include/openssl/*.h", SPN_DIR_INCLUDE, "openssl")) {
    return 1;
  }
  return spn_copy(spn, SPN_DIR_MANIFEST, "config/include/openssl/configuration.h", SPN_DIR_INCLUDE, "openssl/configuration.h");
}

SPN_EXPORT
spn_err_t configure(spn_t* spn, spn_config_t* config) {
  spn_node_t* headers = spn_add_node(config, "headers");
  spn_node_set_fn(headers, "stage_headers");
  for (u32 it = 0; it < sizeof(generated) / sizeof(generated[0]); it++) {
    spn_node_add_input(headers, spn_get_subdir(spn, SPN_DIR_MANIFEST, spn_fmt("gen/include/openssl/{}", generated[it])));
    spn_node_add_output(headers, spn_get_subdir(spn, SPN_DIR_INCLUDE, spn_fmt("openssl/{}", generated[it])));
  }
  spn_node_add_input(headers, spn_get_subdir(spn, SPN_DIR_MANIFEST, "config/include/openssl/configuration.h"));
  spn_node_add_output(headers, spn_get_subdir(spn, SPN_DIR_INCLUDE, "openssl/configuration.h"));
  return SPN_OK;
}
