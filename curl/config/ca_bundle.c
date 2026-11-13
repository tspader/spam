#include <string.h>
#include <unistd.h>

char spn_curl_ca_bundle [64];

static const char* locations [] = {
  "/etc/ssl/certs/ca-certificates.crt",
  "/etc/pki/tls/certs/ca-bundle.crt",
  "/etc/ssl/ca-bundle.pem",
  "/etc/pki/tls/cacert.pem",
  "/etc/pki/ca-trust/extracted/pem/tls-ca-bundle.pem",
  "/etc/ssl/cert.pem",
};

__attribute__((constructor))
static void find_ca_bundle(void) {
  const char* found = locations[0];
  for (size_t it = 0; it < sizeof(locations) / sizeof(locations[0]); it++) {
    if (!access(locations[it], R_OK)) {
      found = locations[it];
      break;
    }
  }
  strcpy(spn_curl_ca_bundle, found);
}
