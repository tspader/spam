#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef struct {
  const char* file;
  const char* rule;
  unsigned count;
  char error[1024];
} targets_t;

bool ninja_load_targets(const char* path, void (*emit)(void* user, const char* target, const char* rule), void* user, char* error, unsigned capacity);

static void print_target(void* user, const char* target, const char* rule) {
  targets_t* targets = (targets_t*)user;
  if (targets->rule && strcmp(targets->rule, rule) != 0) return;
  targets->count++;
  printf("%s %s\n", target, rule);
}

int main(int argc, char** argv) {
  targets_t targets = {
    .file = "build.ninja",
  };

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-r") == 0 && i + 1 < argc) {
      targets.rule = argv[++i];
    }
    else {
      targets.file = argv[i];
    }
  }

  if (!ninja_load_targets(targets.file, print_target, &targets, targets.error, sizeof(targets.error))) {
    fprintf(stderr, "%s\n", targets.error);
    return 1;
  }

  printf("%u %s\n", targets.count, targets.count == 1 ? "target" : "targets");
  return 0;
}
