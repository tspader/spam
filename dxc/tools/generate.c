#define SP_IMPLEMENTATION
#include "sp.h"

#ifdef SP_WIN32
  #define HOST_PYTHON "python"
#else
  #define HOST_PYTHON "python3"
#endif

#define CLANG "tools/clang/include/clang/"

typedef struct {
  const c8* out;
  const c8* td;
  const c8* mode;
} table_t;

typedef struct {
  const c8* mode;
  const c8* out;
} hct_t;

static const table_t llvm_tables [] = {
  { "include/llvm/IR/Intrinsics.gen", "include/llvm/IR/Intrinsics.td", "-gen-intrinsic" },
  { "include/dxc/Support/HLSLOptions.inc", "include/dxc/Support/HLSLOptions.td", "-gen-opt-parser-defs" },
  { CLANG "Driver/Options.inc", CLANG "Driver/Options.td", "-gen-opt-parser-defs" },
};

static const c8* diagnostics [] = {
  "Analysis", "AST", "Comment", "Common", "Driver", "Frontend", "Lex", "Parse", "Sema", "Serialization",
};

static const table_t attr_tables [] = {
  { CLANG "Basic/AttrList.inc", CLANG "Basic/Attr.td", "-gen-clang-attr-list" },
  { CLANG "Basic/AttrHasAttributeImpl.inc", CLANG "Basic/Attr.td", "-gen-clang-attr-has-attribute-impl" },
  { CLANG "AST/Attrs.inc", CLANG "Basic/Attr.td", "-gen-clang-attr-classes" },
  { CLANG "AST/AttrImpl.inc", CLANG "Basic/Attr.td", "-gen-clang-attr-impl" },
  { CLANG "AST/AttrDump.inc", CLANG "Basic/Attr.td", "-gen-clang-attr-dump" },
  { CLANG "AST/AttrVisitor.inc", CLANG "Basic/Attr.td", "-gen-clang-attr-ast-visitor" },
  { CLANG "Sema/AttrTemplateInstantiate.inc", CLANG "Basic/Attr.td", "-gen-clang-attr-template-instantiate" },
  { CLANG "Sema/AttrParsedAttrList.inc", CLANG "Basic/Attr.td", "-gen-clang-attr-parsed-attr-list" },
  { CLANG "Sema/AttrParsedAttrKinds.inc", CLANG "Basic/Attr.td", "-gen-clang-attr-parsed-attr-kinds" },
  { CLANG "Sema/AttrSpellingListIndex.inc", CLANG "Basic/Attr.td", "-gen-clang-attr-spelling-index" },
  { CLANG "Sema/AttrParsedAttrImpl.inc", CLANG "Basic/Attr.td", "-gen-clang-attr-parsed-attr-impl" },
  { CLANG "Parse/AttrParserStringSwitches.inc", CLANG "Basic/Attr.td", "-gen-clang-attr-parser-string-switches" },
  { CLANG "Serialization/AttrPCHRead.inc", CLANG "Basic/Attr.td", "-gen-clang-attr-pch-read" },
  { CLANG "Serialization/AttrPCHWrite.inc", CLANG "Basic/Attr.td", "-gen-clang-attr-pch-write" },
};

static const table_t clang_tables [] = {
  { CLANG "Basic/DiagnosticGroups.inc", CLANG "Basic/Diagnostic.td", "-gen-clang-diag-groups" },
  { CLANG "Basic/DiagnosticIndexName.inc", CLANG "Basic/Diagnostic.td", "-gen-clang-diags-index-name" },
  { CLANG "AST/StmtNodes.inc", CLANG "Basic/StmtNodes.td", "-gen-clang-stmt-nodes" },
  { CLANG "AST/DeclNodes.inc", CLANG "Basic/DeclNodes.td", "-gen-clang-decl-nodes" },
  { CLANG "AST/CommentNodes.inc", CLANG "Basic/CommentNodes.td", "-gen-clang-comment-nodes" },
  { CLANG "AST/CommentHTMLTags.inc", CLANG "AST/CommentHTMLTags.td", "-gen-clang-comment-html-tags" },
  { CLANG "AST/CommentHTMLTagsProperties.inc", CLANG "AST/CommentHTMLTags.td", "-gen-clang-comment-html-tags-properties" },
  { CLANG "AST/CommentHTMLNamedCharacterReferences.inc", CLANG "AST/CommentHTMLNamedCharacterReferences.td", "-gen-clang-comment-html-named-character-references" },
  { CLANG "AST/CommentCommandInfo.inc", CLANG "AST/CommentCommands.td", "-gen-clang-comment-command-info" },
  { CLANG "AST/CommentCommandList.inc", CLANG "AST/CommentCommands.td", "-gen-clang-comment-command-list" },
};

static const hct_t hct_tables [] = {
  { "DxilPIXPasses", "lib/DxilPIXPasses/DxilPIXPasses.inc" },
  { "DxcOptimizer", "lib/HLSL/DxcOptimizer.inc" },
  { "DxilValidationInc", "lib/DxilValidation/DxilValidation.inc" },
  { "DxilValidation", "lib/DxilValidation/DxilValidationImpl.inc" },
  { "DxilIntrinsicTables", "tools/clang/lib/Sema/gen_intrin_main_tables_15.h" },
  { "DxcDisassembler", "tools/clang/tools/dxcompiler/DxcDisassembler.inc" },
};

typedef struct {
  sp_mem_t mem;
  sp_str_t dxc;
  sp_str_t llvm_tblgen;
  sp_str_t clang_tblgen;
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

static sp_err_t output(generate_t* g, sp_str_t sub, sp_str_t* path) {
  *path = sp_fs_join_path(g->mem, g->gen, sub);
  return sp_fs_create_dir(sp_fs_parent_path(*path));
}

static sp_err_t tblgen(generate_t* g, sp_str_t sub, const c8* td, sp_ps_config_t config) {
  sp_str_t out = sp_zero;
  sp_try(output(g, sub, &out));

  sp_ps_config_add_arg(g->mem, &config, sp_str_lit("-I"));
  sp_ps_config_add_arg(g->mem, &config, sp_fs_parent_path(sp_fs_join_path(g->mem, g->dxc, sub)));
  sp_ps_config_add_arg(g->mem, &config, sp_str_lit("-I"));
  sp_ps_config_add_arg(g->mem, &config, join(g, g->dxc, "lib/Target"));
  sp_ps_config_add_arg(g->mem, &config, sp_str_lit("-I"));
  sp_ps_config_add_arg(g->mem, &config, join(g, g->dxc, "include"));
  sp_ps_config_add_arg(g->mem, &config, join(g, g->dxc, td));
  sp_ps_config_add_arg(g->mem, &config, sp_str_lit("-o"));
  sp_ps_config_add_arg(g->mem, &config, out);
  return spawn(g, config);
}

static sp_err_t tables(generate_t* g) {
  sp_carr_for(llvm_tables, it) {
    sp_try(tblgen(g, sp_cstr_as_str(llvm_tables[it].out), llvm_tables[it].td, (sp_ps_config_t) {
      .command = g->llvm_tblgen,
      .args = { sp_cstr_as_str(llvm_tables[it].mode) },
    }));
  }

  sp_carr_for(diagnostics, it) {
    sp_str_t kinds = sp_fmt(g->mem, CLANG "Basic/Diagnostic{}Kinds.inc", sp_fmt_cstr(diagnostics[it])).value;
    sp_try(tblgen(g, kinds, CLANG "Basic/Diagnostic.td", (sp_ps_config_t) {
      .command = g->clang_tblgen,
      .args = {
        sp_str_lit("-gen-clang-diags-defs"),
        sp_fmt(g->mem, "-clang-component={}", sp_fmt_cstr(diagnostics[it])).value,
      },
    }));
  }

  sp_carr_for(attr_tables, it) {
    sp_try(tblgen(g, sp_cstr_as_str(attr_tables[it].out), attr_tables[it].td, (sp_ps_config_t) {
      .command = g->clang_tblgen,
      .args = {
        sp_cstr_as_str(attr_tables[it].mode),
        sp_str_lit("-I"), join(g, g->dxc, "tools/clang/include"),
      },
    }));
  }

  sp_carr_for(clang_tables, it) {
    sp_try(tblgen(g, sp_cstr_as_str(clang_tables[it].out), clang_tables[it].td, (sp_ps_config_t) {
      .command = g->clang_tblgen,
      .args = { sp_cstr_as_str(clang_tables[it].mode) },
    }));
  }
  return SP_OK;
}

static sp_err_t hctgen(generate_t* g) {
  sp_carr_for(hct_tables, it) {
    sp_str_t out = sp_zero;
    sp_try(output(g, sp_cstr_as_str(hct_tables[it].out), &out));
    sp_try(spawn(g, (sp_ps_config_t) {
      .command = sp_str_lit(HOST_PYTHON),
      .args = {
        join(g, g->dxc, "utils/hct/hctgen.py"),
        sp_cstr_as_str(hct_tables[it].mode),
        sp_str_lit("--output"), out,
      },
    }));
  }
  return SP_OK;
}

static sp_err_t version(generate_t* g) {
  sp_str_t commit = sp_zero;
  sp_try(output(g, sp_str_lit("tools/clang/lib/Basic/GitCommitInfo.inc"), &commit));
  sp_try(spawn(g, (sp_ps_config_t) {
    .command = sp_str_lit(HOST_PYTHON),
    .args = { join(g, g->dxc, "utils/GetCommitInfo.py"), g->dxc, commit },
  }));

  sp_str_t fixed = sp_zero;
  sp_try(output(g, sp_str_lit("utils/version/dxcversion.inc"), &fixed));
  sp_path_t from = sp_path_at_cwd(join(g, g->dxc, "utils/version/version.inc"));
  return sp_fs_copy_file_at(from, sp_path_at_cwd(fixed), SP_FS_ATOMIC_REPLACE);
}

static sp_err_t generate(generate_t* g, u32* generated) {
  sp_try(locate(g, (sp_ps_config_t) {
    .command = sp_str_lit("spn"),
    .args = { sp_str_lit("path"), sp_str_lit("dxc") },
  }, &g->dxc));
  sp_try(locate(g, (sp_ps_config_t) {
    .command = sp_str_lit("spn"),
    .args = { sp_str_lit("path"), sp_str_lit("--tool"), sp_str_lit("llvm-tblgen") },
  }, &g->llvm_tblgen));
  sp_try(locate(g, (sp_ps_config_t) {
    .command = sp_str_lit("spn"),
    .args = { sp_str_lit("path"), sp_str_lit("--tool"), sp_str_lit("clang-tblgen") },
  }, &g->clang_tblgen));

  if (sp_fs_exists(g->gen)) {
    sp_try(sp_fs_remove_dir(g->gen));
  }

  sp_try(tables(g));
  sp_try(hctgen(g));
  sp_try(version(g));

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
