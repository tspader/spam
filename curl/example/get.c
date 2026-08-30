#include <curl/curl.h>
#include <stdio.h>

int main(int argc, char** argv) {
  const char* url = argc > 1 ? argv[1] : "https://example.com/";

  curl_global_init(CURL_GLOBAL_DEFAULT);
  CURL* curl = curl_easy_init();
  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

  CURLcode err = curl_easy_perform(curl);
  if (err) {
    printf("failed: %s\n", curl_easy_strerror(err));
    return 1;
  }

  long status = 0;
  curl_off_t bytes = 0;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);
  curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD_T, &bytes);
  printf("\n%s: %ld, %" CURL_FORMAT_CURL_OFF_T " bytes\n", url, status, bytes);

  curl_easy_cleanup(curl);
  curl_global_cleanup();
  return 0;
}
