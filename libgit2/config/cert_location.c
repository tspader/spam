#include <stdlib.h>
#include <unistd.h>

static char* locations [] = {
  "/etc/ssl/certs/ca-certificates.crt",
  "/etc/pki/tls/certs/ca-bundle.crt",
  "/etc/ssl/ca-bundle.pem",
  "/etc/pki/tls/cacert.pem",
  "/etc/pki/ca-trust/extracted/pem/tls-ca-bundle.pem",
  "/etc/ssl/cert.pem",
};

char* git2_default_cert_location(void) {
  char* env = getenv("SSL_CERT_FILE");
  if (env && !access(env, R_OK)) {
    return env;
  }
  env = getenv("SSL_CERT_DIR");
  if (env && !access(env, R_OK)) {
    return env;
  }
  for (size_t it = 0; it < sizeof(locations) / sizeof(locations[0]); it++) {
    if (!access(locations[it], R_OK)) {
      return locations[it];
    }
  }
  return NULL;
}
