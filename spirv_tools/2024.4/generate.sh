#!/usr/bin/env bash
# Regenerate gen/ from a cmake configure of SPIRV-Tools. The grammar table,
# enum mapping, extension list, registry, and version generators are python,
# which can't run inside an spn build script, so the outputs are vendored and
# this script is run by hand when bumping the SPIRV-Tools pin. Keep the
# commits in sync with [package.upstream] in spn.toml and the spirv_headers
# package.
set -euo pipefail

SPIRV_TOOLS_URL="https://github.com/KhronosGroup/SPIRV-Tools.git"
SPIRV_TOOLS_COMMIT="4d2f0b40bfe290dea6c6904dafdf7fd8328ba346" # v2024.4, DXC 1.8.2502 pin
SPIRV_HEADERS_URL="https://github.com/KhronosGroup/SPIRV-Headers.git"
SPIRV_HEADERS_COMMIT="3f17b2af6784bfa2c5aa5dbb8e0e74a607dd8b3b"

GENERATED="
  build-version.inc
  core.insts-unified1.inc
  DebugInfo.h
  debuginfo.insts.inc
  enum_string_mapping.inc
  extension_enum.inc
  generators.inc
  glsl.std.450.insts.inc
  nonsemantic.clspvreflection.insts.inc
  NonSemanticShaderDebugInfo100.h
  nonsemantic.shader.debuginfo.100.insts.inc
  nonsemantic.vkspreflection.insts.inc
  OpenCLDebugInfo100.h
  opencl.debuginfo.100.insts.inc
  opencl.std.insts.inc
  operand.kinds-unified1.inc
  spv-amd-gcn-shader.insts.inc
  spv-amd-shader-ballot.insts.inc
  spv-amd-shader-explicit-vertex-parameter.insts.inc
  spv-amd-shader-trinary-minmax.insts.inc
"

ROOT="$(cd "$(dirname "$0")" && pwd)"
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

fetch() {
  git init -q "$2"
  git -C "$2" fetch -q --depth 1 "$1" "$3"
  git -C "$2" checkout -q FETCH_HEAD
}

fetch "$SPIRV_TOOLS_URL" "$WORK/spirv-tools" "$SPIRV_TOOLS_COMMIT"
fetch "$SPIRV_HEADERS_URL" "$WORK/spirv-headers" "$SPIRV_HEADERS_COMMIT"

cmake -S "$WORK/spirv-tools" -B "$WORK/build" -GNinja \
  -DCMAKE_BUILD_TYPE=Release \
  -DSPIRV_SKIP_TESTS=ON \
  -DSPIRV_SKIP_EXECUTABLES=ON \
  -DSPIRV_WERROR=OFF \
  -DSPIRV-Headers_SOURCE_DIR="$WORK/spirv-headers"

ninja -C "$WORK/build" $GENERATED

mkdir -p "$ROOT/gen"
for f in $GENERATED; do
  cp "$WORK/build/$f" "$ROOT/gen/"
done
echo "regenerated $(ls "$ROOT/gen" | wc -l) files in $ROOT/gen"
