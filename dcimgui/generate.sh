#!/usr/bin/env bash
# Regenerate gen/ with dear_bindings. dear_bindings is python (ply), which
# can't run inside an spn build script, so the outputs are vendored and this
# script is run by hand when bumping the imgui pin. Keep IMGUI_COMMIT in sync
# with [package].version in spn.toml.
set -euo pipefail

IMGUI_URL="https://github.com/ocornut/imgui.git"
IMGUI_COMMIT="b61e56346a92cfcaf1f43a545ca37b0b32239654" # v1.92.8-docking
DEAR_BINDINGS_URL="https://github.com/dearimgui/dear_bindings.git"
DEAR_BINDINGS_COMMIT="c9ff64913915df41c0f4beef485b98a1c685eda5"
BACKENDS="sdl2 opengl3 sdl3 sdlgpu3"

ROOT="$(cd "$(dirname "$0")" && pwd)"
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

fetch() {
  git init -q "$2"
  git -C "$2" fetch -q --depth 1 "$1" "$3"
  git -C "$2" checkout -q FETCH_HEAD
}

fetch "$IMGUI_URL" "$WORK/imgui" "$IMGUI_COMMIT"
fetch "$DEAR_BINDINGS_URL" "$WORK/dear_bindings" "$DEAR_BINDINGS_COMMIT"

python3 -m venv "$WORK/venv"
"$WORK/venv/bin/pip" -q install "ply==3.11"

db() {
  "$WORK/venv/bin/python" "$WORK/dear_bindings/dear_bindings.py" "$@"
}

mkdir -p "$WORK/gen"
db --generateunformattedfunctions -o "$WORK/gen/dcimgui" "$WORK/imgui/imgui.h"
db --generateunformattedfunctions -o "$WORK/gen/dcimgui_internal" \
  --include "$WORK/imgui/imgui.h" "$WORK/imgui/imgui_internal.h"
for backend in $BACKENDS; do
  db --backend --include "$WORK/imgui/imgui.h" \
    --imconfig-path "$WORK/imgui/imconfig.h" \
    -o "$WORK/gen/dcimgui_impl_$backend" \
    "$WORK/imgui/backends/imgui_impl_$backend.h"
done

mkdir -p "$ROOT/gen"
cp "$WORK/gen/"*.h "$WORK/gen/"*.cpp "$ROOT/gen/"
echo "regenerated $(ls "$ROOT/gen" | wc -l) files in $ROOT/gen"
