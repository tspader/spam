#define SP_IMPLEMENTATION
#include "sp.h"

#ifdef SP_WIN32
  #define HOST_PYTHON "python"
#else
  #define HOST_PYTHON "python3"
#endif

static const c8* protocols [] = {
  "xproto", "bigreq", "xc_misc", "composite", "damage", "dbe", "dpms", "dri2", "dri3",
  "glx", "present", "randr", "record", "render", "res", "screensaver", "shape", "shm",
  "sync", "xf86dri", "xfixes", "xinerama", "xinput", "xkb", "xtest", "xv", "xvmc",
};

typedef struct {
  sp_mem_t mem;
  sp_str_t libxcb;
  sp_str_t xcbproto;
  sp_str_t work;
  sp_str_t out;
  sp_str_t gen;
} generate_t;

static sp_str_t join(generate_t* g, sp_str_t base, const c8* sub) {
  return sp_fs_join_path(g->mem, base, sp_cstr_as_str(sub));
}

static sp_err_t spawn(generate_t* g, sp_ps_config_t config) {
  config.env.extra[0] = (sp_env_var_t) { .key = sp_str_lit("PYTHONDONTWRITEBYTECODE"), .value = sp_str_lit("1") };
  sp_ps_output_t result = sp_ps_run(g->mem, config);
  if (result.error) {
    sp_log_err("failed to start {}", sp_fmt_str(config.command));
    return result.error;
  }
  if (result.status.exit_code) {
    sp_log_err("{} exited with {}", sp_fmt_str(config.command), sp_fmt_uint((u32)result.status.exit_code));
    return SP_ERR;
  }
  return SP_OK;
}

static sp_err_t locate(generate_t* g, sp_ps_config_t query, sp_str_t* path) {
  query.io.out.mode = SP_PS_IO_MODE_CREATE;
  sp_ps_output_t result = sp_ps_run(g->mem, query);
  if (result.error) {
    sp_log_err("failed to start {}", sp_fmt_str(query.command));
    return result.error;
  }
  if (result.status.exit_code) {
    return SP_ERR;
  }
  *path = sp_str_trim_right(result.out);
  return SP_OK;
}

static sp_err_t prepare(generate_t* g) {
  if (sp_fs_exists(g->work)) {
    sp_try(sp_fs_remove_dir(g->work));
  }
  return sp_fs_create_dir(g->out);
}

static sp_err_t bind(generate_t* g) {
  sp_str_t script = join(g, g->libxcb, "src/c_client.py");
  sp_str_t xml = join(g, g->xcbproto, "src");

  sp_carr_for(protocols, it) {
    sp_try(spawn(g, (sp_ps_config_t) {
      .command = sp_str_lit(HOST_PYTHON),
      .args = {
        script,
        sp_str_lit("-c"), sp_str_lit("libxcb 1.17.0"),
        sp_str_lit("-l"), sp_str_lit("X Version 11"),
        sp_str_lit("-s"), sp_str_lit("3"),
        sp_str_lit("-p"), g->xcbproto,
        sp_fs_join_path(g->mem, xml, sp_fmt(g->mem, "{}.xml", sp_fmt_cstr(protocols[it])).value),
      },
      .cwd = g->out,
    }));
  }
  return SP_OK;
}

static sp_err_t harvest(generate_t* g, u32* copied) {
  sp_da(sp_fs_entry_t) entries = sp_zero;
  sp_try(sp_fs_collect(g->mem, g->out, &entries));

  if (sp_fs_exists(g->gen)) {
    sp_try(sp_fs_remove_dir(g->gen));
  }
  sp_try(sp_fs_create_dir(g->gen));

  sp_da_for(entries, it) {
    sp_fs_entry_t* entry = &entries[it];
    if (!sp_str_equal(sp_fs_get_ext(entry->name), sp_str_lit("h"))) {
      continue;
    }
    sp_path_t from = sp_path_at_cwd(entry->path);
    sp_path_t to = sp_path_at_cwd(sp_fs_join_path(g->mem, g->gen, entry->name));
    sp_try(sp_fs_copy_file_at(from, to, SP_FS_ATOMIC_REPLACE));
    (*copied)++;
  }
  return SP_OK;
}

static sp_err_t generate(generate_t* g, u32* copied) {
  sp_try(locate(g, (sp_ps_config_t) {
    .command = sp_str_lit("spn"),
    .args = { sp_str_lit("path"), sp_str_lit("libxcb") },
  }, &g->libxcb));
  sp_try(locate(g, (sp_ps_config_t) {
    .command = sp_str_lit("spn"),
    .args = { sp_str_lit("path"), sp_str_lit("--tool-dep"), sp_str_lit("xcbproto") },
  }, &g->xcbproto));
  sp_try(prepare(g));
  sp_try(bind(g));
  return harvest(g, copied);
}

static s32 generate_main(s32 num_args, const c8** args) {
  generate_t g = {
    .mem = sp_mem_heap_as_allocator(sp_mem_heap_new()),
    .work = sp_str_lit("build/generate"),
    .gen = sp_str_lit("gen"),
  };
  g.out = join(&g, g.work, "out");

  u32 copied = 0;
  if (generate(&g, &copied)) {
    return 1;
  }
  sp_log("regenerated {} files in {}", sp_fmt_uint(copied), sp_fmt_str(g.gen));
  return 0;
}
SP_MAIN(generate_main)
