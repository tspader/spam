#define SP_IMPLEMENTATION
#include "sp.h"

#ifdef SP_WIN32
  #define HOST_PYTHON "python"
#else
  #define HOST_PYTHON "python3"
#endif

typedef struct {
  const c8* prefix;
  const c8* grammar;
} extinst_t;

typedef struct {
  const c8* name;
  const c8* grammar;
} header_t;

static const extinst_t extinsts [] = {
  { "", "extinst.glsl.std.450.grammar.json" },
  { "", "extinst.opencl.std.100.grammar.json" },
  { "CLDEBUG100_", "extinst.opencl.debuginfo.100.grammar.json" },
  { "SHDEBUG100_", "extinst.nonsemantic.shader.debuginfo.grammar.json" },
  { "", "extinst.spv-amd-shader-explicit-vertex-parameter.grammar.json" },
  { "", "extinst.spv-amd-shader-trinary-minmax.grammar.json" },
  { "", "extinst.spv-amd-gcn-shader.grammar.json" },
  { "", "extinst.spv-amd-shader-ballot.grammar.json" },
  { "", "extinst.debuginfo.grammar.json" },
  { "", "extinst.nonsemantic.clspvreflection.grammar.json" },
  { "", "extinst.nonsemantic.vkspreflection.grammar.json" },
  { "TOSA_", "extinst.tosa.001000.1.grammar.json" },
  { "", "extinst.arm.motion-engine.100.grammar.json" },
  { "", "extinst.nonsemantic.graph.debuginfo.grammar.json" },
  { "", "extinst.arm.experimental-ml-operations.grammar.json" },
  { "", "extinst.nonsemantic.debugprintf.grammar.json" },
};

static const header_t headers [] = {
  { "DebugInfo.h", "extinst.debuginfo.grammar.json" },
  { "OpenCLDebugInfo100.h", "extinst.opencl.debuginfo.100.grammar.json" },
};

typedef struct {
  sp_mem_t mem;
  sp_str_t tools;
  sp_str_t headers;
  sp_str_t grammar;
  sp_str_t gen;
} generate_t;

static sp_str_t join(generate_t* g, sp_str_t base, const c8* sub) {
  return sp_fs_join_path(g->mem, base, sp_cstr_as_str(sub));
}

static sp_str_t flag(generate_t* g, const c8* name, sp_str_t value) {
  return sp_str_concat(g->mem, sp_cstr_as_str(name), value);
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

static sp_ps_config_t python(generate_t* g, const c8* script) {
  return (sp_ps_config_t) {
    .command = sp_str_lit(HOST_PYTHON),
    .args = { join(g, g->tools, script) },
  };
}

static sp_err_t tables(generate_t* g) {
  sp_ps_config_t ggt = python(g, "utils/ggt.py");
  sp_ps_config_add_arg(g->mem, &ggt, flag(g, "--core-tables-body-output=", join(g, g->gen, "core_tables_body.inc")));
  sp_ps_config_add_arg(g->mem, &ggt, flag(g, "--core-tables-header-output=", join(g, g->gen, "core_tables_header.inc")));
  sp_ps_config_add_arg(g->mem, &ggt, flag(g, "--spirv-core-grammar=", join(g, g->grammar, "spirv.core.grammar.json")));
  sp_carr_for(extinsts, it) {
    sp_str_t grammar = join(g, g->grammar, extinsts[it].grammar);
    sp_ps_config_add_arg(g->mem, &ggt, sp_fmt(g->mem, "--extinst={},{}", sp_fmt_cstr(extinsts[it].prefix), sp_fmt_str(grammar)).value);
  }
  return spawn(g, ggt);
}

static sp_err_t language_headers(generate_t* g) {
  sp_carr_for(headers, it) {
    sp_ps_config_t header = python(g, "utils/generate_language_headers.py");
    sp_ps_config_add_arg(g->mem, &header, flag(g, "--extinst-grammar=", join(g, g->grammar, headers[it].grammar)));
    sp_ps_config_add_arg(g->mem, &header, flag(g, "--extinst-output-path=", join(g, g->gen, headers[it].name)));
    sp_try(spawn(g, header));
  }
  return SP_OK;
}

static sp_err_t registry(generate_t* g) {
  sp_ps_config_t generators = python(g, "utils/generate_registry_tables.py");
  sp_ps_config_add_arg(g->mem, &generators, flag(g, "--xml=", join(g, g->headers, "include/spirv/spir-v.xml")));
  sp_ps_config_add_arg(g->mem, &generators, flag(g, "--generator-output=", join(g, g->gen, "generators.inc")));
  return spawn(g, generators);
}

static sp_err_t version(generate_t* g) {
  sp_ps_config_t build_version = python(g, "utils/update_build_version.py");
  sp_ps_config_add_arg(g->mem, &build_version, join(g, g->tools, "CHANGES"));
  sp_ps_config_add_arg(g->mem, &build_version, join(g, g->gen, "build-version.inc"));
  return spawn(g, build_version);
}

static sp_err_t generate(generate_t* g, u32* generated) {
  sp_try(locate(g, (sp_ps_config_t) {
    .command = sp_str_lit("spn"),
    .args = { sp_str_lit("path"), sp_str_lit("spirv_tools") },
  }, &g->tools));
  sp_try(locate(g, (sp_ps_config_t) {
    .command = sp_str_lit("spn"),
    .args = { sp_str_lit("path"), sp_str_lit("spirv_headers") },
  }, &g->headers));
  g->grammar = join(g, g->headers, "include/spirv/unified1");

  if (sp_fs_exists(g->gen)) {
    sp_try(sp_fs_remove_dir(g->gen));
  }
  sp_try(sp_fs_create_dir(g->gen));

  sp_try(tables(g));
  sp_try(language_headers(g));
  sp_try(registry(g));
  sp_try(version(g));

  sp_da(sp_fs_entry_t) entries = sp_zero;
  sp_try(sp_fs_collect(g->mem, g->gen, &entries));
  *generated = (u32)sp_da_size(entries);
  return SP_OK;
}

static s32 generate_main(s32 num_args, const c8** args) {
  generate_t g = {
    .mem = sp_mem_heap_as_allocator(sp_mem_heap_new()),
    .gen = sp_str_lit("gen"),
  };

  u32 generated = 0;
  if (generate(&g, &generated)) {
    return 1;
  }
  sp_log("regenerated {} files in {}", sp_fmt_uint(generated), sp_fmt_str(g.gen));
  return 0;
}
SP_MAIN(generate_main)
