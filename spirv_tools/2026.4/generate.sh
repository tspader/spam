#!/usr/bin/env bash
set -euo pipefail

SPIRV_TOOLS_URL="https://github.com/KhronosGroup/SPIRV-Tools.git"
SPIRV_TOOLS_COMMIT="ef96ed763b43b59b33b31b362f09a02b729fa1c9"
SPIRV_HEADERS_URL="https://github.com/KhronosGroup/SPIRV-Headers.git"
SPIRV_HEADERS_COMMIT="04fd3caa1e8267e4d95c806cad901181728e1006"

GENERATED="
  build-version.inc
  core_tables_body.inc
  core_tables_header.inc
  DebugInfo.h
  generators.inc
  OpenCLDebugInfo100.h
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
