#include <backtrace.h>
#include <backtrace-supported.h>
#include <stdint.h>
#include <stdio.h>

static void on_error(void* data, const char* message, int code) {
  fprintf(stderr, "libbacktrace: %s (%d)\n", message, code);
}

static int on_frame(void* data, uintptr_t pc, const char* file, int line, const char* function) {
  int* frames = data;
  printf("#%d %s at %s:%d\n", *frames, function ? function : "??", file ? file : "??", line);
  *frames += 1;
  return 0;
}

__attribute__((noinline))
static int leaf(struct backtrace_state* state) {
  int frames = 0;
  backtrace_full(state, 0, on_frame, on_error, &frames);
  return frames;
}

__attribute__((noinline))
static int branch(struct backtrace_state* state) {
  return leaf(state) + 0;
}

int main(void) {
  struct backtrace_state* state = backtrace_create_state(NULL, BACKTRACE_SUPPORTS_THREADS, on_error, NULL);
  return branch(state) ? 0 : 1;
}
