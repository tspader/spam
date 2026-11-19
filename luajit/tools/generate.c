#define SP_IMPLEMENTATION
#include "sp.h"

typedef struct {
  const c8* name;
  const c8* dasc;
  const c8* mode;
  const c8* dasm [8];
} target_t;

static const target_t targets [] = {
  {
    .name = "x86_64-linux",
    .dasc = "vm_x64.dasc",
    .mode = "elfasm",
    .dasm = { "ENDIAN_LE", "P64", "JIT", "FFI", "FPU", "HFABI", "VER=" },
  },
  {
    .name = "aarch64-macos",
    .dasc = "vm_arm64.dasc",
    .mode = "machasm",
    .dasm = { "ENDIAN_LE", "P64", "JIT", "FFI", "DUALNUM", "FPU", "HFABI", "VER=80" },
  },
};

static const c8* libs [] = {
  "lib_base.c", "lib_math.c", "lib_bit.c", "lib_string.c", "lib_table.c", "lib_io.c",
  "lib_os.c", "lib_package.c", "lib_debug.c", "lib_jit.c", "lib_ffi.c", "lib_buffer.c",
};

static const c8* defs [] = { "bcdef", "ffdef", "libdef", "recdef" };

typedef struct {
  sp_mem_t mem;
  sp_str_t luajit;
  sp_str_t src;
  sp_str_t minilua;
  sp_str_t work;
  sp_str_t gen;
} generate_t;

static sp_str_t join(generate_t* g, sp_str_t base, const c8* sub) {
  return sp_fs_join_path(g->mem, base, sp_cstr_as_str(sub));
}

static sp_err_t spawn(generate_t* g, sp_ps_config_t config) {
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

static sp_err_t capture(generate_t* g, sp_ps_config_t query, sp_str_t* line) {
  query.io.out.mode = SP_PS_IO_MODE_CREATE;
  sp_ps_output_t result = sp_ps_run(g->mem, query);
  if (result.error) {
    sp_log_err("failed to start {}", sp_fmt_str(query.command));
    return result.error;
  }
  if (result.status.exit_code) {
    return SP_ERR;
  }
  *line = sp_str_trim_right(result.out);
  return SP_OK;
}

static sp_err_t version(generate_t* g) {
  sp_str_t committed = sp_zero;
  sp_try(capture(g, (sp_ps_config_t) {
    .command = sp_str_lit("git"),
    .args = { sp_str_lit("-C"), g->luajit, sp_str_lit("show"), sp_str_lit("-s"), sp_str_lit("--format=%ct") },
  }, &committed));

  sp_str_t relver = join(g, g->work, "relver.txt");
  sp_try(sp_fs_create_file_str(relver, committed));
  return spawn(g, (sp_ps_config_t) {
    .command = g->minilua,
    .args = {
      join(g, g->src, "host/genversion.lua"),
      join(g, g->src, "luajit_rolling.h"),
      relver,
      join(g, g->gen, "luajit.h"),
    },
  });
}

static sp_err_t assemble(generate_t* g, const target_t* target, sp_str_t out) {
  sp_ps_config_t dynasm = {
    .command = g->minilua,
    .args = { join(g, g->luajit, "dynasm/dynasm.lua") },
    .cwd = g->src,
  };
  sp_carr_for(target->dasm, it) {
    if (!target->dasm[it]) {
      break;
    }
    sp_ps_config_add_arg(g->mem, &dynasm, sp_str_lit("-D"));
    sp_ps_config_add_arg(g->mem, &dynasm, sp_cstr_as_str(target->dasm[it]));
  }
  sp_ps_config_add_arg(g->mem, &dynasm, sp_str_lit("-o"));
  sp_ps_config_add_arg(g->mem, &dynasm, join(g, out, "buildvm_arch.h"));
  sp_ps_config_add_arg(g->mem, &dynasm, sp_cstr_as_str(target->dasc));
  return spawn(g, dynasm);
}

static sp_err_t build(generate_t* g, const target_t* target, sp_str_t out) {
  sp_str_t buildvm = sp_zero;
  sp_try(capture(g, (sp_ps_config_t) {
    .command = sp_str_lit("spn"),
    .args = {
      sp_str_lit("path"), sp_str_lit("--tool"),
      sp_fmt(g->mem, "buildvm-{}", sp_fmt_cstr(target->name)).value,
    },
  }, &buildvm));

  sp_try(spawn(g, (sp_ps_config_t) {
    .command = buildvm,
    .args = { sp_str_lit("-m"), sp_cstr_as_str(target->mode), sp_str_lit("-o"), join(g, out, "lj_vm.S") },
    .cwd = g->src,
  }));

  sp_carr_for(defs, it) {
    sp_ps_config_t def = {
      .command = buildvm,
      .args = {
        sp_str_lit("-m"), sp_cstr_as_str(defs[it]),
        sp_str_lit("-o"), sp_fs_join_path(g->mem, out, sp_fmt(g->mem, "lj_{}.h", sp_fmt_cstr(defs[it])).value),
      },
      .cwd = g->src,
    };
    sp_carr_for(libs, lt) {
      sp_ps_config_add_arg(g->mem, &def, sp_cstr_as_str(libs[lt]));
    }
    sp_try(spawn(g, def));
  }

  return spawn(g, (sp_ps_config_t) {
    .command = buildvm,
    .args = { sp_str_lit("-m"), sp_str_lit("folddef"), sp_str_lit("-o"), join(g, out, "lj_folddef.h"), sp_str_lit("lj_opt_fold.c") },
    .cwd = g->src,
  });
}

static sp_err_t generate(generate_t* g, u32* generated) {
  sp_try(capture(g, (sp_ps_config_t) {
    .command = sp_str_lit("spn"),
    .args = { sp_str_lit("path"), sp_str_lit("luajit") },
  }, &g->luajit));
  sp_try(capture(g, (sp_ps_config_t) {
    .command = sp_str_lit("spn"),
    .args = { sp_str_lit("path"), sp_str_lit("--tool"), sp_str_lit("minilua") },
  }, &g->minilua));
  g->src = join(g, g->luajit, "src");

  if (sp_fs_exists(g->gen)) {
    sp_try(sp_fs_remove_dir(g->gen));
  }
  sp_try(sp_fs_create_dir(g->gen));
  sp_try(sp_fs_create_dir(g->work));

  sp_try(version(g));
  sp_carr_for(targets, it) {
    sp_str_t out = join(g, g->gen, targets[it].name);
    sp_try(sp_fs_create_dir(out));
    sp_try(assemble(g, &targets[it], out));
    sp_try(build(g, &targets[it], out));
  }

  sp_da(sp_fs_entry_t) entries = sp_zero;
  sp_try(sp_fs_collect_recursive(g->mem, g->gen, &entries));
  sp_da_for(entries, it) {
    if (entries[it].kind == SP_FS_KIND_FILE) {
      (*generated)++;
    }
  }
  return SP_OK;
}

static s32 generate_main(s32 num_args, const c8** args) {
  generate_t g = {
    .mem = sp_mem_heap_as_allocator(sp_mem_heap_new()),
  };
  sp_str_t root = sp_fs_get_cwd_path(g.mem);
  g.work = join(&g, root, "build/generate");
  g.gen = join(&g, root, "gen");

  u32 generated = 0;
  if (generate(&g, &generated)) {
    return 1;
  }
  sp_log("regenerated {} files in {}", sp_fmt_uint(generated), sp_fmt_str(g.gen));
  return 0;
}
SP_MAIN(generate_main)
