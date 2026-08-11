#include <mbedtls/ctr_drbg.h>
#include <mbedtls/entropy.h>
#include <mbedtls/error.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/ssl.h>

#include <stdio.h>
#include <string.h>

#define HOST "example.com"
#define PORT "443"

int main(void) {
  mbedtls_net_context net;
  mbedtls_ssl_context ssl;
  mbedtls_ssl_config conf;
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context drbg;

  mbedtls_net_init(&net);
  mbedtls_ssl_init(&ssl);
  mbedtls_ssl_config_init(&conf);
  mbedtls_entropy_init(&entropy);
  mbedtls_ctr_drbg_init(&drbg);

  int err = mbedtls_ctr_drbg_seed(&drbg, mbedtls_entropy_func, &entropy, NULL, 0);
  char message[128];

  if (!err) {
    err = mbedtls_net_connect(&net, HOST, PORT, MBEDTLS_NET_PROTO_TCP);
  }
  if (!err) {
    err = mbedtls_ssl_config_defaults(&conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
  }
  if (!err) {
    mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_NONE);
    mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &drbg);
    err = mbedtls_ssl_setup(&ssl, &conf);
  }
  if (!err) {
    err = mbedtls_ssl_set_hostname(&ssl, HOST);
  }
  if (!err) {
    mbedtls_ssl_set_bio(&ssl, &net, mbedtls_net_send, mbedtls_net_recv, NULL);
    do {
      err = mbedtls_ssl_handshake(&ssl);
    } while (err == MBEDTLS_ERR_SSL_WANT_READ || err == MBEDTLS_ERR_SSL_WANT_WRITE);
  }

  if (err) {
    mbedtls_strerror(err, message, sizeof(message));
    printf("failed: %s (-0x%04x)\n", message, -err);
    return 1;
  }

  printf("connected to %s: %s, %s\n", HOST, mbedtls_ssl_get_version(&ssl), mbedtls_ssl_get_ciphersuite(&ssl));

  const char* request = "GET / HTTP/1.1\r\nHost: " HOST "\r\nConnection: close\r\n\r\n";
  size_t sent = 0;
  while (sent < strlen(request)) {
    int n = mbedtls_ssl_write(&ssl, (const unsigned char*)request + sent, strlen(request) - sent);
    if (n == MBEDTLS_ERR_SSL_WANT_READ || n == MBEDTLS_ERR_SSL_WANT_WRITE) {
      continue;
    }
    if (n < 0) {
      printf("write failed: -0x%04x\n", -n);
      return 1;
    }
    sent += (size_t)n;
  }

  unsigned char response[256];
  int n;
  do {
    n = mbedtls_ssl_read(&ssl, response, sizeof(response) - 1);
  } while (n == MBEDTLS_ERR_SSL_WANT_READ || n == MBEDTLS_ERR_SSL_WANT_WRITE);
  if (n <= 0) {
    printf("read failed: -0x%04x\n", -n);
    return 1;
  }
  response[n] = 0;

  char* line = strstr((char*)response, "\r\n");
  if (line) {
    *line = 0;
  }
  printf("%s\n", response);

  mbedtls_ssl_close_notify(&ssl);
  mbedtls_net_free(&net);
  mbedtls_ssl_free(&ssl);
  mbedtls_ssl_config_free(&conf);
  mbedtls_ctr_drbg_free(&drbg);
  mbedtls_entropy_free(&entropy);
  return 0;
}
