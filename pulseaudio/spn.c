#include "spn.h"

#include <stdio.h>
#include <string.h>

typedef struct {
  const c8* from;
  const c8* to;
} subst_t;

static s32 render(const c8* from, const c8* to, const subst_t* substs, u32 count) {
  static c8 text [64 * 1024];

  FILE* in = fopen(from, "rb");
  if (!in) {
    return 1;
  }
  size_t len = fread(text, 1, sizeof(text) - 1, in);
  fclose(in);
  if (len == sizeof(text) - 1) {
    return 1;
  }
  text[len] = 0;

  FILE* out = fopen(to, "wb");
  if (!out) {
    return 1;
  }

  const c8* cursor = text;
  while (*cursor) {
    u32 hit = count;
    for (u32 it = 0; it < count; it++) {
      if (!strncmp(cursor, substs[it].from, strlen(substs[it].from))) {
        hit = it;
        break;
      }
    }
    if (hit < count) {
      fputs(substs[hit].to, out);
      cursor += strlen(substs[hit].from);
    }
    else {
      fputc(*cursor, out);
      cursor++;
    }
  }

  fclose(out);
  return 0;
}

static const c8* HEADERS [] = {
  "cdecl", "channelmap", "context", "def", "direction", "error",
  "ext-device-manager", "ext-device-restore", "ext-stream-restore",
  "format", "gccmacro", "introspect", "mainloop-api", "mainloop-signal",
  "mainloop", "operation", "proplist", "pulseaudio", "rtclock", "sample",
  "scache", "simple", "stream", "subscribe", "thread-mainloop", "timeval",
  "utf8", "util", "volume", "xmalloc",
};

#define HEADER_COUNT (sizeof(HEADERS) / sizeof(HEADERS[0]))

SPN_EXPORT
s32 stage_headers(spn_t* spn) {
  for (u32 it = 0; it < HEADER_COUNT; it++) {
    c8 from [128];
    c8 to [128];
    snprintf(from, sizeof(from), "src/pulse/%s.h", HEADERS[it]);
    snprintf(to, sizeof(to), "pulse/%s.h", HEADERS[it]);
    if (spn_copy(spn, SPN_DIR_SOURCE, from, SPN_DIR_INCLUDE, to)) {
      return 1;
    }
  }

  subst_t substs [] = {
    { "@PA_MAJOR@", "17" },
    { "@PA_MINOR@", "0" },
    { "@PA_API_VERSION@", "12" },
    { "@PA_PROTOCOL_VERSION@", "35" },
  };
  return render("/source/src/pulse/version.h.in", "/store/include/pulse/version.h", substs, 4);
}

SPN_EXPORT
spn_err_t configure(spn_t* spn, spn_config_t* config) {
  spn_node_t* headers = spn_add_node(config, "headers");
  spn_node_set_fn(headers, "stage_headers");
  for (u32 it = 0; it < HEADER_COUNT; it++) {
    c8 path [128];
    snprintf(path, sizeof(path), "pulse/%s.h", HEADERS[it]);
    spn_node_add_output(headers, spn_get_subdir(spn, SPN_DIR_INCLUDE, path));
  }
  spn_node_add_output(headers, spn_get_subdir(spn, SPN_DIR_INCLUDE, "pulse/version.h"));
  return SPN_OK;
}
