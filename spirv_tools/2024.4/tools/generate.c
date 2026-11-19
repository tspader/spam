#define SP_IMPLEMENTATION
#include "sp.h"

#ifdef SP_WIN32
  #define HOST_PYTHON "python"
#else
  #define HOST_PYTHON "python3"
#endif

typedef struct {
  const c8* name;
  const c8* prefix;
} vendor_t;

typedef struct {
  const c8* name;
  const c8* grammar;
} header_t;

static const vendor_t vendors [] = {
  { "spv-amd-shader-explicit-vertex-parameter", "" },
  { "spv-amd-shader-trinary-minmax", "" },
  { "spv-amd-gcn-shader", "" },
  { "spv-amd-shader-ballot", "" },
  { "debuginfo", "" },
  { "opencl.debuginfo.100", "CLDEBUG100_" },
  { "nonsemantic.shader.debuginfo.100", "SHDEBUG100_" },
  { "nonsemantic.clspvreflection", "" },
  { "nonsemantic.vkspreflection", "" },
};

static const header_t headers [] = {
  { "DebugInfo.h", "extinst.debuginfo.grammar.json" },
  { "OpenCLDebugInfo100.h", "extinst.opencl.debuginfo.100.grammar.json" },
  { "NonSemanticShaderDebugInfo100.h", "extinst.nonsemantic.shader.debuginfo.100.grammar.json" },
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

static sp_ps_config_t grammar_tables(generate_t* g) {
  sp_ps_config_t config = python(g, "utils/generate_grammar_tables.py");
  sp_ps_config_add_arg(g->mem, &config, flag(g, "--spirv-core-grammar=", join(g, g->grammar, "spirv.core.grammar.json")));
  sp_ps_config_add_arg(g->mem, &config, flag(g, "--extinst-debuginfo-grammar=", join(g, g->grammar, "extinst.debuginfo.grammar.json")));
  sp_ps_config_add_arg(g->mem, &config, flag(g, "--extinst-cldebuginfo100-grammar=", join(g, g->grammar, "extinst.opencl.debuginfo.100.grammar.json")));
  return config;
}

static sp_err_t tables(generate_t* g) {
  sp_ps_config_t core = grammar_tables(g);
  sp_ps_config_add_arg(g->mem, &core, flag(g, "--core-insts-output=", join(g, g->gen, "core.insts-unified1.inc")));
  sp_ps_config_add_arg(g->mem, &core, flag(g, "--operand-kinds-output=", join(g, g->gen, "operand.kinds-unified1.inc")));
  sp_ps_config_add_arg(g->mem, &core, sp_str_lit("--output-language=c++"));
  sp_try(spawn(g, core));

  sp_ps_config_t enums = grammar_tables(g);
  sp_ps_config_add_arg(g->mem, &enums, flag(g, "--extension-enum-output=", join(g, g->gen, "extension_enum.inc")));
  sp_ps_config_add_arg(g->mem, &enums, flag(g, "--enum-string-mapping-output=", join(g, g->gen, "enum_string_mapping.inc")));
  sp_ps_config_add_arg(g->mem, &enums, sp_str_lit("--output-language=c++"));
  sp_try(spawn(g, enums));

  sp_ps_config_t opencl = python(g, "utils/generate_grammar_tables.py");
  sp_ps_config_add_arg(g->mem, &opencl, flag(g, "--extinst-opencl-grammar=", join(g, g->grammar, "extinst.opencl.std.100.grammar.json")));
  sp_ps_config_add_arg(g->mem, &opencl, flag(g, "--opencl-insts-output=", join(g, g->gen, "opencl.std.insts.inc")));
  sp_try(spawn(g, opencl));

  sp_ps_config_t glsl = python(g, "utils/generate_grammar_tables.py");
  sp_ps_config_add_arg(g->mem, &glsl, flag(g, "--extinst-glsl-grammar=", join(g, g->grammar, "extinst.glsl.std.450.grammar.json")));
  sp_ps_config_add_arg(g->mem, &glsl, flag(g, "--glsl-insts-output=", join(g, g->gen, "glsl.std.450.insts.inc")));
  sp_ps_config_add_arg(g->mem, &glsl, sp_str_lit("--output-language=c++"));
  sp_try(spawn(g, glsl));

  sp_carr_for(vendors, it) {
    sp_str_t grammar = sp_fmt(g->mem, "extinst.{}.grammar.json", sp_fmt_cstr(vendors[it].name)).value;
    sp_str_t insts = sp_fmt(g->mem, "{}.insts.inc", sp_fmt_cstr(vendors[it].name)).value;
    sp_ps_config_t vendor = python(g, "utils/generate_grammar_tables.py");
    sp_ps_config_add_arg(g->mem, &vendor, flag(g, "--extinst-vendor-grammar=", sp_fs_join_path(g->mem, g->grammar, grammar)));
    sp_ps_config_add_arg(g->mem, &vendor, flag(g, "--vendor-insts-output=", sp_fs_join_path(g->mem, g->gen, insts)));
    sp_ps_config_add_arg(g->mem, &vendor, flag(g, "--vendor-operand-kind-prefix=", sp_cstr_as_str(vendors[it].prefix)));
    sp_try(spawn(g, vendor));
  }
  return SP_OK;
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
