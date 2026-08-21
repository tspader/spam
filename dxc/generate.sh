#!/bin/sh
# @spader This is 100% slop; the shape of this package as a whole isn't that
# bad, but there's a bunch of stuff in this script in particular that we
# shouldn't pattern match against.
#
# The TLDR is that we need to do three things:
# (1) Run hctgen, which builds all the static tables needed for code generation
# and optimization. This is written in Python, and it's pretty much unavoidable
# without maintaining an insane fork. But this is exactly the case where the
# recommended spn pattern is "run your native tool offline, publish the
# vendored results"
#
# (2) Run tablegen, which is just a tool that's part of LLVM written in C++. In
# other words, spn can definitely build this as part of the build. We just need
# the new configure() stuff to land.
#
# (3) Generate code from this data. Claude chose to use CMake for this out of
# laziness, but there's zero reason for this.
#
# Anyway, here's Claude with a little more detail:
#
# Let me split generate.sh into its three actual jobs, because they have different answers.
#
# **Job 1: run hctgen.** Six of the vendored files (`DxcOptimizer.inc`, the validation tables, `gen_intrin_main_tables_15.h`…) come out of `utils/hct/hctgen.py`, which renders them from `hctdb.py` — a ~10k-line Python database of every DXIL opcode and validation rule. That's upstream's source of truth. Getting Python out of *this* means rewriting Microsoft's generator in C and maintaining the fork forever. It's the exact dear_bindings situation, and vendoring the outputs is the same answer. Python is already out of the build; it can't get out of regeneration without forking upstream tooling.
#
# **Job 2: run tablegen.** This is the one that stings, because llvm-tblgen *is* just C++ from the same pinned repo, and spn can build it. What I could not find is a working way for spn to *execute* it during a build: the host implementation of the guest API's `spn_run_ex` (`source/core/api/api.c:11`) pushes an `SPN_EVENT_API_CALL` event and returns `SPN_OK` — it never spawns anything. If that's WIP rather than my misreading, then tablegen-in-the-DAG is genuinely on the table. The other path — compile tblgen to wasm32-wasi and call it from a node fn inside the metaprogram — founders on LLVM 3.7's Support layer predating WASI (mmap, signals, threads); portable with a patch set, but that's a real project. And since hctgen forces a vendored `gen/` anyway, putting tablegen in the DAG buys purity, not python-freedom.
#
# **Job 3: CMake.** This is honestly just laziness, and it can go. I used the full cmake+ninja build for two things: driving all the codegen with correct flags in one shot, and answering the configure probes. The probes are already dead — `config/` is data now; the cross-configures were a one-time derivation aid. A tighter generate.sh is: `spn build` a `[[bin]]` tblgen target from the dxc package itself, run the 32 tablegen invocations as frozen command lines (they're stable per pin — extractable once from `ninja -t commands`), call `hctgen.py` directly (its CLI is plain: `hctgen.py DxcOptimizer --output …`), done. No cmake, no ninja, and — since the codegen targets never touch SPIR-V — no SPIRV-Tools or SPIRV-Headers checkouts at all. spirv_tools' generate.sh can likewise call the grammar-table python scripts directly instead of going through cmake.
#
# **On the manual clones:** two reasons, one good, one fixable. The reference cmake build needs the repos *as a source tree in submodule layout* (`external/SPIRV-Tools` etc. are `add_subdirectory` paths) — spn packages deliver published artifacts to consumers, not source trees at arbitrary paths, so the packages themselves can't feed it. But spn *does* have the pinned checkouts sitting in `~/.local/share/spn/cache/source/checkouts/` — I cloned instead of symlinking those because the dir names carry an internal hash (`SPIRV-Headers-<hash>`), there's no CLI I found that resolves a package to its checkout path, and I didn't want generate.sh silently coupled to a private cache layout. If spn grew something like `spn fetch <pkg> --print-path`, that's what generate.sh should use — and with the cmake-free version above, the question mostly evaporates: the only checkout it still needs is dxc itself.


# Regenerates gen/ and config/ from a reference cmake build of the pinned
# DirectXShaderCompiler commit. The cmake build runs tablegen and hctgen
# (python); their outputs are harvested into gen/, mirroring the cmake build
# tree layout. config/ holds the cmake-configured platform headers, unified
# into single tri-OS headers by branching every differing macro on
# _WIN32/__APPLE__ (plus __GLIBC__ guards for glibc-only functions, since the
# linux reference configure runs against glibc but spn defaults to musl).
#
# Requires: git, cmake, ninja, python3, a host clang, and the spn zig
# toolchain (used only to cmake-configure for the windows and macos variants
# of the config headers; nothing is built for those targets).
set -e

DXC_URL=https://github.com/libsdl-org/DirectXShaderCompiler.git
DXC_COMMIT=80a7375441f416f6b3e3df815c14ac9132725450
SPIRV_HEADERS_COMMIT=3f17b2af6784bfa2c5aa5dbb8e0e74a607dd8b3b
SPIRV_TOOLS_COMMIT=4d2f0b40bfe290dea6c6904dafdf7fd8328ba346
DIRECTX_HEADERS_COMMIT=980971e835876dc0cde415e8f9bc646e64667bf7

HERE=$(cd "$(dirname "$0")" && pwd)
WORK=${WORK:-$(mktemp -d)}
ZIG=${ZIG:-$(find ~/.local/share/spn/cache/toolchain -maxdepth 2 -name zig -type f | head -1)}
echo "work dir: $WORK"
cd "$WORK"

fetch() {
  mkdir -p "$2" && cd "$2"
  git init -q 2>/dev/null || true
  git remote add origin "$1" 2>/dev/null || true
  git fetch -q --depth 1 origin "$3"
  git checkout -q FETCH_HEAD
  cd "$WORK"
}

if [ ! -d dxc ]; then
  fetch "$DXC_URL" dxc "$DXC_COMMIT"
  fetch https://github.com/KhronosGroup/SPIRV-Headers dxc/external/SPIRV-Headers "$SPIRV_HEADERS_COMMIT"
  fetch https://github.com/KhronosGroup/SPIRV-Tools dxc/external/SPIRV-Tools "$SPIRV_TOOLS_COMMIT"
  fetch https://github.com/microsoft/DirectX-Headers.git dxc/external/DirectX-Headers "$DIRECTX_HEADERS_COMMIT"
fi

COMMON="-GNinja -DCMAKE_BUILD_TYPE=Release -C $WORK/dxc/cmake/caches/PredefinedParams.cmake -DHLSL_INCLUDE_TESTS=OFF -DSPIRV_BUILD_TESTS=OFF -DLLVM_APPEND_VC_REV=OFF -DLLVM_INCLUDE_TESTS=OFF"

mkdir -p build-linux build-win build-mac bin
cd build-linux
cmake $COMMON -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ "$WORK/dxc" > cmake.log
ninja dxcompiler
cd "$WORK"

printf '#!/bin/sh\nexec %s cc -target x86_64-windows-gnu "$@"\n' "$ZIG" > bin/cc-win
printf '#!/bin/sh\nexec %s c++ -target x86_64-windows-gnu "$@"\n' "$ZIG" > bin/cxx-win
printf '#!/bin/sh\nexec %s cc -target aarch64-macos-none "$@"\n' "$ZIG" > bin/cc-mac
printf '#!/bin/sh\nexec %s c++ -target aarch64-macos-none "$@"\n' "$ZIG" > bin/cxx-mac
chmod +x bin/*

cd build-win
cmake $COMMON -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_SYSTEM_PROCESSOR=AMD64 -DCMAKE_C_COMPILER="$WORK/bin/cc-win" -DCMAKE_CXX_COMPILER="$WORK/bin/cxx-win" -DHLSL_BUILD_DXILCONV=OFF "$WORK/dxc" > cmake.log
cd ../build-mac
cmake $COMMON -DCMAKE_SYSTEM_NAME=Darwin -DCMAKE_SYSTEM_PROCESSOR=arm64 -DCMAKE_C_COMPILER="$WORK/bin/cc-mac" -DCMAKE_CXX_COMPILER="$WORK/bin/cxx-mac" "$WORK/dxc" > cmake.log
cd "$WORK"

rm -rf "$HERE/gen"
python3 - "$WORK" "$HERE" <<'EOF'
import os, shutil, sys
work, here = sys.argv[1], sys.argv[2]
build = f"{work}/build-linux"
names = ("config.h", "llvm-config.h", "abi-breaking.h", "DataTypes.h", "SharedLibAffix.h", "Version.inc", "GitCommitInfo.inc", "dxcversion.inc")
skip = ("config.h", "llvm-config.h", "abi-breaking.h", "DataTypes.h", "SharedLibAffix.h")
for root, dirs, files in os.walk(build):
    dirs[:] = [d for d in dirs if d not in ("CMakeFiles", "tmp", ".cmake") and not d.startswith("SPIRV")]
    rel = os.path.relpath(root, build)
    if rel.startswith(("external", "bin", "test", "tools/llvm-config", "projects")):
        continue
    for f in files:
        if not (f.endswith((".inc", ".gen", ".def")) or f in names):
            continue
        if f.endswith(".inc.gen") or "Test" in rel or f in skip:
            continue
        if f.endswith(".def") and "Config" in rel:
            continue
        dst = os.path.join(here, "gen", rel, f)
        os.makedirs(os.path.dirname(dst), exist_ok=True)
        shutil.copy2(os.path.join(root, f), dst)
extra = f"{build}/tools/clang/lib/Sema/gen_intrin_main_tables_15.h"
dst = f"{here}/gen/tools/clang/lib/Sema/gen_intrin_main_tables_15.h"
os.makedirs(os.path.dirname(dst), exist_ok=True)
shutil.copy2(extra, dst)
EOF

rm -rf "$HERE/config"
python3 - "$WORK" "$HERE" <<'EOF'
import os, re, shutil, sys
work, here = sys.argv[1], sys.argv[2]
GLIBC_ONLY = {"HAVE_BACKTRACE": None, "HAVE_MALLINFO": None, "HAVE_MALLINFO2": (2, 33), "HAVE_DECL_ARC4RANDOM": (2, 36)}
FORCE = {"HAVE_LIBZ": None, "LLVM_ENABLE_ZLIB": "0", "HAVE_LIBEDIT": None, "HAVE_TERMINFO": None, "CLANG_HAVE_LIBXML": None, "HOST_LINK_VERSION": None}
def parse(p):
    vals, order = {}, []
    for line in open(p):
        m = re.match(r"#define (\w+) ?(.*)", line.strip())
        u = re.match(r"/\* #undef (\w+) \*/", line.strip())
        if m:
            vals[m.group(1)] = m.group(2).strip() or "1"
            order.append((m.group(1), line.rstrip("\n")))
        elif u:
            vals[u.group(1)] = None
            order.append((u.group(1), line.rstrip("\n")))
        else:
            order.append((None, line.rstrip("\n")))
    return vals, order
def unify(rel, out):
    lin, order = parse(f"{work}/build-linux/{rel}")
    win, _ = parse(f"{work}/build-win/{rel}")
    mac, _ = parse(f"{work}/build-mac/{rel}")
    res, emitted = [], set()
    for k, raw in order:
        if k is None:
            res.append(raw)
            continue
        if k in emitted:
            continue
        emitted.add(k)
        l, w, m = lin.get(k), win.get(k), mac.get(k)
        if k in FORCE:
            l = w = m = FORCE[k]
        def d(v):
            return f"#define {k} {v}" if v is not None else f"/* #undef {k} */"
        def dl(v):
            if v is not None and k in GLIBC_ONLY:
                ver = GLIBC_ONLY[k]
                if ver:
                    cond = f"defined(__GLIBC__) && (__GLIBC__ > {ver[0]} || (__GLIBC__ == {ver[0]} && __GLIBC_MINOR__ >= {ver[1]}))"
                else:
                    cond = "defined(__GLIBC__)"
                return f"#if {cond}\n{d(v)}\n#endif"
            return d(v)
        if l == w == m:
            res.append(dl(l))
        else:
            res.append(f"#if defined(_WIN32)\n{d(w)}\n#elif defined(__APPLE__)\n{d(m)}\n#else\n{dl(l)}\n#endif")
    dst = os.path.join(here, "config", out)
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    open(dst, "w").write("\n".join(res) + "\n")
unify("include/llvm/Config/config.h", "llvm/Config/config.h")
unify("include/llvm/Config/llvm-config.h", "llvm/Config/llvm-config.h")
unify("include/llvm/Support/DataTypes.h", "llvm/Support/DataTypes.h")
unify("tools/clang/include/clang/Config/config.h", "clang/Config/config.h")
affix = os.path.join(here, "config", "dxc/Support/SharedLibAffix.h")
os.makedirs(os.path.dirname(affix), exist_ok=True)
open(affix, "w").write("""#pragma once

#if defined(_WIN32)
#define CMAKE_SHARED_LIBRARY_PREFIX ""
#define CMAKE_SHARED_LIBRARY_SUFFIX ".dll"
#elif defined(__APPLE__)
#define CMAKE_SHARED_LIBRARY_PREFIX "lib"
#define CMAKE_SHARED_LIBRARY_SUFFIX ".dylib"
#else
#define CMAKE_SHARED_LIBRARY_PREFIX "lib"
#define CMAKE_SHARED_LIBRARY_SUFFIX ".so"
#endif
""")
for rel, out in (("include/llvm/Config/abi-breaking.h", "llvm/Config/abi-breaking.h"),
                 ("include/dxc/config.h", "dxc/config.h"),
                 ("include/llvm/Config/AsmParsers.def", "llvm/Config/AsmParsers.def"),
                 ("include/llvm/Config/AsmPrinters.def", "llvm/Config/AsmPrinters.def"),
                 ("include/llvm/Config/Disassemblers.def", "llvm/Config/Disassemblers.def"),
                 ("include/llvm/Config/Targets.def", "llvm/Config/Targets.def")):
    dst = os.path.join(here, "config", out)
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    shutil.copy2(f"{work}/build-linux/{rel}", dst)
EOF

echo "regenerated $HERE/gen and $HERE/config from $WORK"
