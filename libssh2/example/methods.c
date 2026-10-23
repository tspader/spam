#include <libssh2.h>

#include <stdio.h>

static void print_methods(LIBSSH2_SESSION* session, const char* label, int type) {
  const char** algs = NULL;
  int count = libssh2_session_supported_algs(session, type, &algs);
  printf("%s:", label);
  for (int i = 0; i < count; i++) {
    printf(" %s", algs[i]);
  }
  printf("\n");
  if (count > 0) {
    libssh2_free(session, (void*)algs);
  }
}

int main(void) {
  if (libssh2_init(0)) {
    fprintf(stderr, "libssh2_init failed\n");
    return 1;
  }

  LIBSSH2_SESSION* session = libssh2_session_init();
  if (!session) {
    fprintf(stderr, "libssh2_session_init failed\n");
    return 1;
  }

  printf("libssh2 %s\n", libssh2_version(0));
  print_methods(session, "kex", LIBSSH2_METHOD_KEX);
  print_methods(session, "hostkey", LIBSSH2_METHOD_HOSTKEY);
  print_methods(session, "cipher", LIBSSH2_METHOD_CRYPT_CS);
  print_methods(session, "mac", LIBSSH2_METHOD_MAC_CS);
  print_methods(session, "compression", LIBSSH2_METHOD_COMP_CS);

  libssh2_session_free(session);
  libssh2_exit();
  return 0;
}
