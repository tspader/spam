#include <git2.h>
#include <stdio.h>

int main(int argc, char** argv) {
  const char* url = argc > 1 ? argv[1] : "https://github.com/tspader/spandex.git";
  git_libgit2_init();
  git_remote* remote = NULL;
  const git_remote_head** heads = NULL;
  size_t count = 0;
  if (git_remote_create_detached(&remote, url) < 0 ||
      git_remote_connect(remote, GIT_DIRECTION_FETCH, NULL, NULL, NULL) < 0 ||
      git_remote_ls(&heads, &count, remote) < 0) {
    const git_error* err = git_error_last();
    fprintf(stderr, "fail: %s\n", err ? err->message : "unknown");
    return 1;
  }
  printf("ok %d refs from %s\n", (int)count, url);
  return 0;
}
