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

SPN_EXPORT
s32 stage_headers(spn_t* spn) {
  subst_t substs [] = {
    { "#undef XCURSOR_LIB_MAJOR", "#define XCURSOR_LIB_MAJOR 1" },
    { "#undef XCURSOR_LIB_MINOR", "#define XCURSOR_LIB_MINOR 2" },
    { "#undef XCURSOR_LIB_REVISION", "#define XCURSOR_LIB_REVISION 3" },
  };
  spn_fs_create_dir("/store/include");
  spn_fs_create_dir("/store/include/X11");
  spn_fs_create_dir("/store/include/X11/Xcursor");
  return render("/source/include/X11/Xcursor/Xcursor.h.in", "/store/include/X11/Xcursor/Xcursor.h", substs, 3);
}

SPN_EXPORT
spn_err_t configure(spn_t* spn, spn_config_t* config) {
  spn_node_t* headers = spn_add_node(config, "headers");
  spn_node_set_fn(headers, "stage_headers");
  spn_node_add_output(headers, spn_get_subdir(spn, SPN_DIR_INCLUDE, "X11/Xcursor/Xcursor.h"));
  return SPN_OK;
}
