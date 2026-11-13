#include <libssh2.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

static int fail(LIBSSH2_SESSION* session, const char* what) {
  char* message = NULL;
  if (session) {
    libssh2_session_last_error(session, &message, NULL, 0);
  }
  fprintf(stderr, "%s: %s\n", what, message ? message : "failed");
  return 1;
}

int main(int argc, char** argv) {
  if (argc < 5) {
    fprintf(stderr, "usage: exec host port user keyfile [command]\n");
    return 1;
  }
  const char* host = argv[1];
  const char* port = argv[2];
  const char* user = argv[3];
  const char* key = argv[4];
  const char* command = argc > 5 ? argv[5] : "echo hello from libssh2";

#ifdef _WIN32
  WSADATA wsa;
  WSAStartup(MAKEWORD(2, 2), &wsa);
#endif

  struct addrinfo hints = { .ai_socktype = SOCK_STREAM };
  struct addrinfo* address = NULL;
  if (getaddrinfo(host, port, &hints, &address)) {
    return fail(NULL, "getaddrinfo");
  }
  libssh2_socket_t sock = socket(address->ai_family, address->ai_socktype, address->ai_protocol);
  if (connect(sock, address->ai_addr, (int)address->ai_addrlen)) {
    return fail(NULL, "connect");
  }
  freeaddrinfo(address);

  if (libssh2_init(0)) {
    return fail(NULL, "libssh2_init");
  }
  LIBSSH2_SESSION* session = libssh2_session_init();
  libssh2_session_flag(session, LIBSSH2_FLAG_COMPRESS, 1);
  if (libssh2_session_handshake(session, sock)) {
    return fail(session, "handshake");
  }

  printf("kex: %s\n", libssh2_session_methods(session, LIBSSH2_METHOD_KEX));
  printf("hostkey: %s\n", libssh2_session_methods(session, LIBSSH2_METHOD_HOSTKEY));
  printf("cipher: %s\n", libssh2_session_methods(session, LIBSSH2_METHOD_CRYPT_CS));
  printf("compression: %s\n", libssh2_session_methods(session, LIBSSH2_METHOD_COMP_CS));

  const unsigned char* hash = (const unsigned char*)libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA256);
  printf("fingerprint: ");
  for (int i = 0; hash && i < 32; i++) {
    printf("%02x", hash[i]);
  }
  printf("\n");

  if (libssh2_userauth_publickey_fromfile(session, user, NULL, key, NULL)) {
    return fail(session, "auth");
  }

  LIBSSH2_CHANNEL* channel = libssh2_channel_open_session(session);
  if (!channel) {
    return fail(session, "channel");
  }
  if (libssh2_channel_exec(channel, command)) {
    return fail(session, "exec");
  }

  char buffer [4096];
  ssize_t count = 0;
  while ((count = libssh2_channel_read(channel, buffer, sizeof(buffer))) > 0) {
    fwrite(buffer, 1, (size_t)count, stdout);
  }
  libssh2_channel_close(channel);
  int status = libssh2_channel_get_exit_status(channel);
  libssh2_channel_free(channel);

  libssh2_session_disconnect(session, "done");
  libssh2_session_free(session);
  libssh2_exit();
  return status;
}
