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
  "array", "buffers", "capabilities", "client", "conf", "context", "control",
  "core", "data-loop", "device", "factory", "filter", "global", "i18n", "impl",
  "impl-client", "impl-core", "impl-device", "impl-factory", "impl-link",
  "impl-metadata", "impl-module", "impl-node", "impl-port", "keys", "link",
  "log", "loop", "main-loop", "map", "mem", "module", "node", "permission",
  "pipewire", "port", "properties", "protocol", "proxy", "resource", "stream",
  "thread", "thread-loop", "timer-queue", "type", "utils", "work-queue",
};

static const c8* EXTENSIONS [] = {
  "extensions/client-node", "extensions/metadata", "extensions/profiler",
  "extensions/protocol-native", "extensions/security-context",
  "extensions/session-manager",
};

static const c8* SESSION_MANAGER [] = {
  "extensions/session-manager/impl-interfaces",
  "extensions/session-manager/interfaces",
  "extensions/session-manager/introspect",
  "extensions/session-manager/introspect-funcs",
  "extensions/session-manager/keys",
};

#define COUNT(array) (sizeof(array) / sizeof(array[0]))

static s32 stage(spn_t* spn, const c8* const* names, u32 count) {
  for (u32 it = 0; it < count; it++) {
    c8 from [128];
    c8 to [128];
    snprintf(from, sizeof(from), "src/pipewire/%s.h", names[it]);
    snprintf(to, sizeof(to), "pipewire/%s.h", names[it]);
    if (spn_copy(spn, SPN_DIR_SOURCE, from, SPN_DIR_INCLUDE, to)) {
      return 1;
    }
  }
  return 0;
}

static void declare(spn_t* spn, spn_node_t* node, const c8* const* names, u32 count) {
  for (u32 it = 0; it < count; it++) {
    c8 path [128];
    snprintf(path, sizeof(path), "pipewire/%s.h", names[it]);
    spn_node_add_output(node, spn_get_subdir(spn, SPN_DIR_INCLUDE, path));
  }
}

SPN_EXPORT
s32 stage_headers(spn_t* spn) {
  if (stage(spn, HEADERS, COUNT(HEADERS)) || stage(spn, EXTENSIONS, COUNT(EXTENSIONS))) {
    return 1;
  }

  subst_t substs [] = {
    { "@PIPEWIRE_VERSION_MAJOR@", "1" },
    { "@PIPEWIRE_VERSION_MINOR@", "6" },
    { "@PIPEWIRE_VERSION_MICRO@", "8" },
    { "@PIPEWIRE_API_VERSION@", "\"0.3\"" },
  };
  return render("/source/src/pipewire/version.h.in", "/store/include/pipewire/version.h", substs, 4);
}

SPN_EXPORT
s32 stage_session_manager(spn_t* spn) {
  return stage(spn, SESSION_MANAGER, COUNT(SESSION_MANAGER));
}

SPN_EXPORT
spn_err_t configure(spn_t* spn, spn_config_t* config) {
  spn_node_t* headers = spn_add_node(config, "headers");
  spn_node_set_fn(headers, "stage_headers");
  declare(spn, headers, HEADERS, COUNT(HEADERS));
  declare(spn, headers, EXTENSIONS, COUNT(EXTENSIONS));
  spn_node_add_output(headers, spn_get_subdir(spn, SPN_DIR_INCLUDE, "pipewire/version.h"));

  spn_node_t* session_manager = spn_add_node(config, "session_manager");
  spn_node_set_fn(session_manager, "stage_session_manager");
  declare(spn, session_manager, SESSION_MANAGER, COUNT(SESSION_MANAGER));

  return SPN_OK;
}
